#include "FoldingUmbrella.h"
#include "Player.h"

using namespace MyMath;
using namespace UseEveryOne;

void FoldingUmbrella::Initialize() {
	BaseUmbrella::Initialize();

	//モデル設定
	object_->SetModelFile("umbrella_Close.gltf");

	//AABBの大きさ
	umbrellaAABB_.min = -kAABBSize_ * kDivideByTwo_;
	umbrellaAABB_.max = kAABBSize_ * kDivideByTwo_;
}

void FoldingUmbrella::Update() {
	BaseUmbrella::Update();

	//防御状態の場合
	if (!isShield_) {
		object_->ChangeAnimation("umbrella_Close.gltf");//閉じた傘
	}
	else {
		object_->ChangeAnimation("umbrella_Open.gltf");//閉じた傘
	}

	//更新
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void FoldingUmbrella::Fire() {

	if(rapidTimer_ == kRapidTimeMax_){
		Vector3 translate = player_->GetTranslate();
		Vector3 bulletVelocity = { 0.0f, 0.0f, kBulletSpeed_ };

		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_);

		//パーティクル
		player_->ParticleFire(translate);
	}
	
	rapidTimer_ -= kDeltaTime_;
	if (rapidTimer_ <= 0.0f) {
		rapidCount_++;
		rapidTimer_ = kRapidTimeMax_;
	}

	if (rapidCount_ == kMaxRapidCount_) {
		player_->FireFinish();
		rapidCount_ = 0;
	}
}

void FoldingUmbrella::PowerFire() {

	if (rapidTimer_ == kRapidTimeMax_) {
		Vector3 translate = player_->GetTranslate();
		Vector3 bulletVelocity = { 0.0f, 0.0f, kBulletSpeed_ * kBulletPowerUpSpeed_ };

		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_);

		//パーティクル
		player_->ParticleFire(translate);
	}
		
	rapidTimer_ -= kDeltaTime_ * kTwice_;
	if (rapidTimer_ <= 0.0f) {
		rapidCount_++;
		rapidTimer_ = kRapidTimeMax_;
	}

	//弾丸の2倍増加
	if (rapidCount_ >= kMaxRapidCount_ * uint32_t(kTwice_)) {
		player_->FireFinish();
		player_->SubGaugePoint();//ゲージポイント減少
		rapidCount_ = 0;
	}
}
