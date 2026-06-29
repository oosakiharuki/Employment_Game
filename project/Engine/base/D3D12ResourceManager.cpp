#include "D3D12ResourceManager.h"
#include "DirectXCommon.h"
#include "WinApp.h"
#include <externals/DirectXTex/d3dx12.h>

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	std::unique_ptr<D3D12ResourceManager>  D3D12ResourceManager::sInstance_ = nullptr;

	D3D12ResourceManager& D3D12ResourceManager::GetInstance() {
		if (sInstance_ == nullptr) {
			sInstance_ = std::make_unique<D3D12ResourceManager>(InstanceKey());
		}
		return *sInstance_;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceManager::CreateBufferResource(size_t sizeInBytes) {
		//VertexResource
		//頂点シェーダを作る
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC vertexResourceDesc{};

		vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexResourceDesc.Width = sizeInBytes;

		vertexResourceDesc.Height = 1;
		vertexResourceDesc.DepthOrArraySize = 1;
		vertexResourceDesc.MipLevels = 1;
		vertexResourceDesc.SampleDesc.Count = 1;

		vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//実際に頂点リソースを作る
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
		HRESULT hr = DirectXCommon::GetInstance().GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
		assert(SUCCEEDED(hr));

		return vertexResource;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width);//幅
		resourceDesc.Height = UINT(metadata.height);//高さ
		resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
		resourceDesc.DepthOrArraySize = UINT(metadata.arraySize);//奥行き　Textureの配置数
		resourceDesc.Format = metadata.format;//format
		resourceDesc.SampleDesc.Count = 1;//サンプリングカウント(1固定)
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// textureの次元数


		//利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//Resourceの生成
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		HRESULT hr = DirectXCommon::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&resource)
		);
		assert(SUCCEEDED(hr));
		return resource;

	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceManager::CreateDepthStencilResource() {

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = WinApp::kClientWidth_;
		resourceDesc.Height = WinApp::kClientHeight_;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//二次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencil

		//利用するHeap
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAN上で作る


		//深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;


		//resourceの生成
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		HRESULT hr = DirectXCommon::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		return resource;
	}

	//戻り値を破棄してはならない
	//UploadTextureData(…) ×  / ID3D12Resource* a = UploadTextureData(…) 〇
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
		//中間リソース
		std::vector<D3D12_SUBRESOURCE_DATA> subResources;
		DirectX::PrepareUpload(DirectXCommon::GetInstance().GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subResources.size()));
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize).Get();
		//データ転送
		UpdateSubresources(DirectXCommon::GetInstance().GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subResources.size()), subResources.data());

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture.Get();//こいつ
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		DirectXCommon::GetInstance().GetCommandList()->ResourceBarrier(1, &barrier);
		return intermediateResource;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceManager::CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& clearColor) {

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = WinApp::kClientWidth_;
		resourceDesc.Height = WinApp::kClientHeight_;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Format = format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//二次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;//RenderTargetとして利用可能

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		clearValue.Color[0] = clearColor.x;
		clearValue.Color[1] = clearColor.y;
		clearValue.Color[2] = clearColor.z;
		clearValue.Color[3] = clearColor.s;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;

		//resourceの生成
		DirectXCommon::GetInstance().GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&resource));

		return resource;
	}
}
