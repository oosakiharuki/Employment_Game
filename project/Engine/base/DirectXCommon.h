#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
//ComPtr
#include <wrl.h>
#include "Logger.h"
#include "StringUtility.h"
#include "WinApp.h"

#include <array>
#include <dxcapi.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <chrono>


#include "Vector4.h"

class DirectXCommon {
public:
	static DirectXCommon* GetInstance();

	void Initialize();
	void Device();
	void Command();
	void SwapChain();
	void ZBuffer();
	void DescriptorHeap();

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesciptors, bool shaderVisible);

	void RTV();

	//void SRV();
	void DSV();//深度ステンシルビュー
	void Fence();
	void ViewPort();
	void Siccer();
	void DXC();

	void SetWinApp(WinApp* winApp) { winApp_ = winApp; }

	//コンパイルシェーダ
	Microsoft::WRL::ComPtr <IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);


	void PreDraw();
	void PostDraw();

	ID3D12Device*  GetDevice()const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() { return dsvHandle; }
	HANDLE GetFenceEvent() { return fenceEvent; }

	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }
	//DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; };
	size_t GetSwapChainResourceNum() { return swapChainDesc.BufferCount; }

	//device,width,heightは省略
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& clearColor);
	
	ID3D12Resource* GetRenderTexture() { return renderTextureResource.Get(); }
	ID3D12Resource* GetOutlineResource() { return depthStencilResource.Get(); }

	void RenderTexturePreDraw();
	void RenderTexturePostDraw();

	void Finalize();

	//最大SRV数
	static const uint32_t kMaxSRVCount;

private:

	HRESULT hr;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	///コマンド-----------------

	//コマンドキュー
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	//コマンドアロケータ
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;


	//スワップチェーン
	Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	static const uint32_t MaxResource = 2;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	
	WinApp* winApp_ = nullptr;


	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;


	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;


	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> srvDescriptorHeap;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> dsvDescriptorHeap;


	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[MaxResource];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandlesRT;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandlesOutline;

	//ビューポート
	D3D12_VIEWPORT viewport;

	//シザリング短径(シザー)
	D3D12_RECT scissorRect{};


	//DXC
	Microsoft::WRL::ComPtr <IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr <IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler = nullptr;

	//Update


	Microsoft::WRL::ComPtr <ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	//Fix = 固定
	void InitializeFixFPS();
	void UpdateFixFPS();
	//逆行しないタイマー
	std::chrono::steady_clock::time_point reference_;


	static DirectXCommon* instance;

	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(DirectXCommon&) = default;
	DirectXCommon& operator=(DirectXCommon&) = default;


	static uint32_t kSRVIndexTop;

	//書き込み可能なテクスチャ レンダーテクスチャ
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource;
	
	D3D12_CLEAR_VALUE clearValue;
};