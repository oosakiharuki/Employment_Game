#include "ShadowManager.h"
#include "DirectXCommon.h"
#include <numbers>


using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	std::unique_ptr<ShadowManager> ShadowManager::sInstance_ = nullptr;

	ShadowManager& ShadowManager::GetInstance() {
		if (sInstance_ == nullptr) {
			sInstance_ = std::make_unique<ShadowManager>();
		}
		return *sInstance_;
	}
	void ShadowManager::Finalize() {
		sInstance_.reset();
	}


	void ShadowManager::Initialize() {
		resources_.clear();
	}

	void ShadowManager::Update() {

	}

	void ShadowManager::Draw() {
		for (auto& resource : resources_) {
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
				7,
				resource->GetGPUVirtualAddress());
		}
	}

	void ShadowManager::AddShadow(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource) {
		resources_.push_back(resource);
	}

	void ShadowManager::Reset() {
		resources_.clear();
	}
}