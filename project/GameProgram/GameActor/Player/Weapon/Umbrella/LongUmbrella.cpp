#include "LongUmbrella.h"
#include "UseEveryOne.h"
#include "Player.h"

#include "ParticleManager.h"

using namespace UseEveryOne;
using namespace MyMath;

void LongUmbrella::Initialize() {
	BaseUmbrella::Initialize();

	//モデル設定
	object_->SetModelFile("longUmbrella_Close.gltf");

	//AABBの大きさ
	umbrellaAABB_.min = -kAABBSize_ * kDivideByTwo_;
	umbrellaAABB_.max = kAABBSize_ * kDivideByTwo_;

	Audio::GetInstance().LoadWave(kChargeSoundName_);

	particles_[kChargeParticle_] = ParticleManager::GetInstance().InitParticle(kChargeParticle_);

}

void LongUmbrella::Update() {

	BaseUmbrella::Update();

	//防御状態の場合
	if (!isShield_) {
		object_->ChangeAnimation("longUmbrella_Close.gltf");//閉じた傘
	}
	else {
		object_->ChangeAnimation("longUmbrella_Open.gltf");//閉じた傘
	}

	//更新
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void LongUmbrella::Fire() {

	chargeTimer_ += kDeltaTime_;
	chargeTimer_ = std::clamp(chargeTimer_, 0.0f, kChargeMaxTime_ + kDeltaTime_);

	if (chargeTimer_ >= kChargeMaxTime_ && chargeTimer_ < kChargeMaxTime_ + kDeltaTime_) {
		Audio::GetInstance().StopWave(kChargeSoundName_);
		Audio::GetInstance().SoundPlayWave(kChargeSoundName_, 0.3f);

		particles_[kChargeParticle_]->SetColor({ 1.0f,1.0f,0.0f,1.0f });//黄色
	}

	//チャージする時間に達したら
	if (chargeTimer_ >= kChargeMaxTime_ && (!Input::GetInstance().PushKey(DIK_K) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_X) && !Input::GetInstance().RightTriggerLongPress())) {
		Vector3 translate = player_->GetTranslate();
		Vector3 bulletVelocity = { 0.0f, 0.0f, kBulletSpeed_ };

		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_);

		//パーティクル
		player_->ParticleFire(translate);
		//ノックバック
		player_->KnockBackUmbrella({ 0,0,kBulletSpeed_ * kDivideByThree_ }, kBulletSpeed_);

		player_->FireFinish();//発砲終了
		chargeTimer_ = 0.0f;
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::Stop);
		particles_[kChargeParticle_]->SetColor({ 1.0f,1.0f,1.0f,1.0f });//白色

	}
	else if (!Input::GetInstance().PushKey(DIK_K) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_X) && !Input::GetInstance().RightTriggerLongPress()) {
		//失敗
		player_->FireFinish();//発砲終了
		chargeTimer_ = 0.0f;
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::Stop);
	}
	else {
		//チャージパーティクル発動
		particles_[kChargeParticle_]->SetTranslate(transform_.translate);
		particles_[kChargeParticle_]->SetScale(Vector3{ chargeTimer_,chargeTimer_,chargeTimer_ });
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::TimerMode);
	}
}

void LongUmbrella::PowerFire() {

	chargeTimer_ += kDeltaTime_ * 3.0f;//速い
	chargeTimer_ = std::clamp(chargeTimer_, 0.0f, kChargeMaxTime_ + kDeltaTime_ * 3.0f);

	//チャージ時間の半減
	if (chargeTimer_ >= kChargeMaxTime_ && chargeTimer_ < kChargeMaxTime_ + kDeltaTime_ * 3.0f) {
		Audio::GetInstance().StopWave(kChargeSoundName_);
		Audio::GetInstance().SoundPlayWave(kChargeSoundName_, 0.3f);
		
		particles_[kChargeParticle_]->SetColor({ 1.0f,1.0f,0.0f,1.0f });//黄色
	}

	//チャージする時間に達したら
	if (chargeTimer_ >= kChargeMaxTime_ && (!Input::GetInstance().PushKey(DIK_K) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_X) && !Input::GetInstance().RightTriggerLongPress())) {

		Vector3 translate = player_->GetTranslate();
		Vector3 bulletVelocity = { 0.0f, 0.0f, kBulletSpeed_ };

		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_ * uint32_t(kTwice_));

		//パーティクル
		player_->ParticleFire(translate);
		//ノックバック
		player_->KnockBackUmbrella({ 0,0,kBulletSpeed_ * kDivideByThree_ }, kBulletSpeed_);

		player_->FireFinish();//発砲終了
		player_->SubGaugePoint();//ゲージポイント減少
		chargeTimer_ = 0.0f;
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::Stop);
		particles_[kChargeParticle_]->SetColor({ 1.0f,1.0f,1.0f,1.0f });//白色
	}
	else if (!Input::GetInstance().PushKey(DIK_K) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_X) && !Input::GetInstance().RightTriggerLongPress()) {
		player_->FireFinish();//発砲終了
		chargeTimer_ = 0.0f;
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::Stop);
	}
	else {
		//チャージパーティクル発動
		particles_[kChargeParticle_]->SetTranslate(transform_.translate);
		particles_[kChargeParticle_]->SetScale(Vector3{ chargeTimer_,chargeTimer_,chargeTimer_ } * kTwice_);
		particles_[kChargeParticle_]->SetParticleBorn(ParticleBorn::TimerMode);
	}
}
