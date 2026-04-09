#include "ReinforceGauge.h"
#include "UIManager.h"

#include "ImGuiManager.h"

using namespace MyMath;

void ReinforceGauge::Initialize() {
	
	gaugeSprite_ = std::make_unique<Sprite>();
	gaugeSprite_->Initialize("ReinforceGauge.png");
	gaugeSprite_->SetPosition(GaugePosition_);

	for (int i = 0; i < int(kGaugePointSpriteNum_); i++) {
		std::unique_ptr<Sprite> gaugePointSprite = std::make_unique<Sprite>();
		gaugePointSprite->Initialize("ReinforcePoint.png");
		gaugePointSprites_.push_back(std::move(gaugePointSprite));
	}

	reaction_ = std::make_unique<Reaction>();
}

void ReinforceGauge::Update() {

	//ポイントの上限、下限
	gaugePoint_ = std::clamp(gaugePoint_,uint32_t(0),kGaugePointMax_);
	
	//ゲージのUI
	UIManager::GetInstance().FrameSprite(&*gaugeSprite_);

	//3ポイント貯めたら拡縮リアクション
	reaction_->ScaleReaction(moveSize_, isChargeSuccess_, moveSizePower_, timer_, timerMax_);

	int32_t i = 1;
	for (auto& gaugePointSprite : gaugePointSprites_) {
		//0ポイント
		float sizeY = 0;
		//3ポイント(Max)
		if(gaugePoint_ >= kUseGaugeCost_ * i) {
			sizeY = kTextureSize_;//サイズそのまま
		}//2ポイント分
		else if (gaugePoint_ >= kUseGaugeCost_ * i - 1) {
			sizeY = kTextureSize_ / kUseGaugeCost_ * 2;//サイズの3分の2
		}//1ポイント分
		else if (gaugePoint_ >= kUseGaugeCost_ * i - 2) {
			sizeY = kTextureSize_ / kUseGaugeCost_;//サイズの3分の1
		}

		Vector2 pointPos = GaugePosition_;

		pointPos.x += (i - 1) * kTextureSize_ + (kTextureSize_ * 0.5f);
		pointPos.y += kTextureSize_ - kTextureSize_ * 0.5f;

		gaugePointSprite->SetPosition(pointPos);
		gaugePointSprite->SetAnchorPoint({0.5f,0.5f});

		Vector2 size = { kTextureSize_, sizeY };
		if (gaugePoint_ - i * kUseGaugeCost_ == 0 && isChargeSuccess_) {
			size += moveSize_;
		}

		gaugePointSprite->SetSize(size);
		
		i++;
		UIManager::GetInstance().FrameSprite(&*gaugePointSprite);
	}

#ifdef USE_IMGUI

	ImGui::Begin("gauge");
	ImGui::Text("gaugePoint %f",float(gaugePoint_));
	ImGui::End();

#endif // USE_IMGUI

}

void ReinforceGauge::AddPoint() { 
	gaugePoint_++;
	if (std::remainder(gaugePoint_, kUseGaugeCost_) == 0) {
		isChargeSuccess_ = true;
	}
}
