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

void Umbrella::BornBullet() {

	Vector3 translate = player_->GetUmbrellaTranslate();

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 bulletVelocity = { 0.0f, kDispersionBetween_, kBulletSpeed_ };
		//弾が分散するように
		bulletVelocity.y *= i;
		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();//初期化
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocity(bulletVelocity);//速さ
		player_->AddBullet(std::move(bullet));
	}
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);

	player_->FireFinish();
}

void Umbrella::BornPowerBullet() {

	Vector3 translate = player_->GetUmbrellaTranslate();

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 bulletVelocity = { 0.0f, kDispersionBetween_ * kDivideByTwo_, kBulletSpeed_ * kTwice_ };
		//弾が分散するように
		bulletVelocity.y *= i;
		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();//初期化
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocity(bulletVelocity);//速さ
		
		bullet->StrongPower();//強さ
		
		player_->AddBullet(std::move(bullet));
	}
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
	player_->SubGaugePoint();//ゲージポイント減少

	player_->FireFinish();

}


