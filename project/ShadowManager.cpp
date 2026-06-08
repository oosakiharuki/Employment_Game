#include "ShadowManager.h"
#include "DirectXCommon.h"
#include <numbers>
#include "ImGuiManager.h"

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
		resource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(ShadowFactory));
		resource_->Map(0,nullptr,reinterpret_cast<void**>(&shadowFactory_));
	}

	void ShadowManager::Update() {
		for (size_t i = 0; i < shadowDatas_.size(); ++i) {
			shadowFactory_->shadowData[i] = shadowDatas_[i];
		}
	}

	void ShadowManager::Draw() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
			7,
			resource_->GetGPUVirtualAddress());
	}

	void ShadowManager::AddShadow(const Vector3& shadowPosition) {
		ShadowData shadowData{};
		shadowData.color = {1,1,1,1};
		shadowData.position = shadowPosition;
		shadowData.direction = Normalize({ 0.0f,-1.0f,0.0f });
		shadowData.distance = 10.0f;
		shadowData.decay = 0.1f;
		shadowData.intensity = 1.0f;
		shadowData.cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
		shadowData.cosFalloffStart = std::cos(std::numbers::pi_v<float> / 6.0f);

		shadowDatas_.push_back(shadowData);
	}

	void ShadowManager::Reset() {
		shadowDatas_.clear();
		for (size_t i = 0; i < kMaxShadow; ++i) {
			shadowFactory_->shadowData[i] = {};
		}
	}
}