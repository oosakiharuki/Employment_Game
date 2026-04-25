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

void FoldingUmbrella::BornBullet() {

	Vector3 translate = player_->GetTranslate();

	const float kBulletSpeed_ = 0.5f;//弾丸の前方向の速さ
	
	Vector3 bulletVelocity = { 0.0f, 0.0f, kBulletSpeed_ * kTwice_ };

	//飛ばす向きをwtGun_に合わせる
	bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

	//弾丸を生み出す
	std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
	bullet->Initialize();
	bullet->SetTranslate(translate);//発泡初期位置
	bullet->SetVelocity(bulletVelocity);//速さ

	bullet->StrongPower();//強さ

	bullets_.push_back(std::move(bullet));
	
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella({0,0,0.5f}, kBulletSpeed_);

	if (rapidCount_ == kMaxRapidCount_) {
		isFireFinish_ = true;
		rapidCount_ = 0;
	}
	else {
		rapidCount_++;
	}
}

void FoldingUmbrella::BornPowerBullet() {

}
