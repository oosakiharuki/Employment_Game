/// ----------------------
///
/// 影生成マネージャ
/// スポットライトの減衰
/// 
/// ----------------------
#include "ShadowManager.h"
#include "DirectXCommon.h"
#include <numbers>
#include "ImGuiManager.h"

#include <algorithm>


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
		resource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(ShadowFactory));
		resource_->Map(0,nullptr,reinterpret_cast<void**>(&shadowFactory_));
	}

	void ShadowManager::Update() {
		//vector配列をファクトリーの配列に
		for (size_t i = 0; i < shadowDatas_.size(); ++i) {
			shadowFactory_->shadowData[i] = shadowDatas_[i];
		}
	}

	void ShadowManager::Draw() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
			7,
			resource_->GetGPUVirtualAddress());
	}

	void ShadowManager::AddShadow(const Vector3& shadowPosition,const Vector3& shadowSize, float distance) {
		ShadowData shadowData{};
		shadowData.color = {1,1,1,1};
		shadowData.position = shadowPosition;
		shadowData.direction = Normalize(kDirection_);		
		shadowData.distance = std::clamp(distance, 0.0f, kMaxDistance_) + kMinDistance_ * shadowSize.y;//長さ + 影が移せる最大限の長さ * Y軸の大きさ
		shadowData.decay = kDecay_;
		shadowData.intensity = 1.0f;
		shadowData.cosAngle = std::cos(std::numbers::pi_v<float> / kAngle_ * ((shadowSize.x + shadowSize.z) / 2.0f));//XZ軸で大きさの変動
		shadowData.cosFalloffStart = std::cos(std::numbers::pi_v<float> / kAngle_ * ((shadowSize.x + shadowSize.z) / 2.0f));//XZ軸で大きさの変動

		shadowDatas_.push_back(shadowData);
	}

	void ShadowManager::Reset() {
		shadowDatas_.clear();
		for (size_t i = 0; i < kMaxShadow; ++i) {
			shadowFactory_->shadowData[i] = {};
		}
	}
}