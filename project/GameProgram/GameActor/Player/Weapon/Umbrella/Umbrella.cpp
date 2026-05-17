/// ------------------------
///
/// 三方向に弾丸を放つ傘銃
/// 
/// ------------------------
#include "Umbrella.h"
#include "ImGuiManager.h"
#include "UseEveryOne.h"

#include "Player.h"

using namespace MyMath;
using namespace UseEveryOne;

Umbrella::~Umbrella() {}

void Umbrella::Initialize() {
	BaseUmbrella::Initialize();

	//モデル設定
	object_->SetModelFile("umbrella_Close.gltf");

	//AABBの大きさ
	umbrellaAABB_.min = -kAABBSize_ * kDivideByTwo_;
	umbrellaAABB_.max = kAABBSize_ * kDivideByTwo_;
}

void Umbrella::Update() {
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

void Umbrella::Fire() {

	Vector3 translate = player_->GetUmbrellaTranslate();

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 bulletVelocity = { 0.0f, kDispersionBetween_, kBulletSpeed_ };
		//弾が分散するように
		bulletVelocity.y *= i;
		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_);
	}
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);

	player_->FireFinish();
}

void Umbrella::PowerFire() {

	Vector3 translate = player_->GetUmbrellaTranslate();

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 bulletVelocity = { 0.0f, kDispersionBetween_ * kDivideByTwo_, kBulletSpeed_ * kTwice_ };
		//弾が分散するように
		bulletVelocity.y *= i;
		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());
		//弾丸を生み出す
		BornBullet(translate, bulletVelocity, kBulletPower_ * uint32_t(kTwice_));
	}
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
	player_->SubGaugePoint();//ゲージポイント減少

	player_->FireFinish();

}


