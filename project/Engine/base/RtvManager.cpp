/// ------------------------------------
///
/// レンダーターゲットビューマネージャ
/// 
/// ------------------------------------
#include "RtvManager.h"
#include "DirectXCommon.h"

namespace EngineLayer {
	std::unique_ptr<RtvManager> RtvManager::sInstance_ = nullptr;

	RtvManager& RtvManager::GetInstance() {
		if (sInstance_ == nullptr) {
			sInstance_ = std::make_unique<RtvManager>();
		}
		return *sInstance_;
	}

	void RtvManager::Initialize() {
		descriptorSizeRTV_ = DirectXCommon::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		rtvDescriptorHeap_ = DirectXCommon::GetInstance().CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);
	}

	void RtvManager::Finalize() {
		sInstance_.reset();
	}

	void RtvManager::CreateRTV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource) {
		rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		//CPUの最初の位置
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

		rtvHandles_[useIndex_] = rtvStartHandle;
		if (useIndex_ > 0) {
			rtvHandles_[useIndex_].ptr = rtvHandles_[useIndex_ - 1].ptr + descriptorSizeRTV_;
		}

		DirectXCommon::GetInstance().GetDevice()->CreateRenderTargetView(resource.Get(),&rtvDesc_, rtvHandles_[useIndex_]);	
		useIndex_++;//追加
	}
}
