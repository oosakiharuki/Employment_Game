#include "BaseUmbrella.h"
#include "Player.h"
#include "UseEveryOne.h"
#include "ImGuiManager.h"

using namespace MyMath;
using namespace UseEveryOne;

void BaseUmbrella::Initialize() {

	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	//オブジェクト初期化(モデル設定以外)
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();

	//リアクション
	reaction_ = std::make_unique<Reaction>();
	//当たりタイプ
	collisionType_ = CollisionTypes::TypeUmbrella;

	//傘を開くSE
	umbrellaOpenSound_ = Audio::GetInstance().LoadWave("resource/Sound/umbrellaOpen.mp3");
	//発砲攻撃
	fireSound_ = Audio::GetInstance().LoadWave("resource/Sound/fire.mp3");
	//パリィ
	parrySound_ = Audio::GetInstance().LoadWave("resource/Sound/bane.mp3");
}

void BaseUmbrella::Update() {

#ifdef USE_IMGUI

	ImGui::Begin("um");

	ImGui::InputFloat3("worldTransform.translate", &transform_.translate.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &transform_.translate.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &transform_.rotate.x);
	ImGui::SliderFloat("RotateX", &transform_.rotate.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &transform_.rotate.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &transform_.rotate.z, -360.0f, 360.0f);

	ImGui::End();

#endif // USE_IMGUI
	//傘に弾丸が触れた時
	reaction_->ScaleReaction(transform_.scale, isHit_, kScalePower_, scaleTimer_, kReactionMaxTime_);

	//パリィ更新処理
	ParryUpdate();
}

void BaseUmbrella::Draw() {
	object_->Draw();
}


void BaseUmbrella::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeEnemyBullet && isShield_) {
		isHit_ = true;
		scaleTimer_ = 0.0f;
		transform_.scale = kDefaultScale_;

		//強化ゲージポイント加算
		player_->AddGaugePoint();

		if (collisionType_ == CollisionTypes::TypeUmbrellaParry) {
			parryTime_ = kParryTimeMax_;//連続で跳ね返せるように
			ParrySuccess();//パリィ成功処理
			return;
		}
		//通常防御の場合、プレイヤーがノックバック
		player_->KnockBackUmbrella(kUmbrellaKnockBackPower_, kUmbrellaKnockBackTime_);
	}
}

bool BaseUmbrella::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypeEnemyBullet && isShield_) {
		return true;
	}
	return false;
}

void BaseUmbrella::Fire() {

	if (player_->UseGaugePoint()) {
		//弾丸速度が二倍、より遠くに飛ばせる
		BornPowerBullet();
	}
	else {
		BornBullet();//発砲攻撃
	}

	Audio::GetInstance().StopWave(*fireSound_);
	Audio::GetInstance().SoundPlayWave(*fireSound_, kVolume_);

}

void BaseUmbrella::OffShield() {
	isShield_ = false;
	Audio::GetInstance().StopWave(*umbrellaOpenSound_);
}

void BaseUmbrella::ShieldMode() {
	//既に開いている場合はスキップ
	if (!isShield_) {
		isParry_ = true;
		Audio::GetInstance().SoundPlayWave(*umbrellaOpenSound_, kVolume_ * kTwice_);
	}
	isShield_ = true;

	//当たり判定設定
	collisionAABB_.min = transform_.translate + umbrellaAABB_.min;
	collisionAABB_.max = transform_.translate + umbrellaAABB_.max;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
}

void BaseUmbrella::ParryUpdate() {
	if (!isParry_) {
		parryTime_ = kParryTimeMax_;
		collisionType_ = CollisionTypes::TypeUmbrella;
		return;
	}

	collisionType_ = CollisionTypes::TypeUmbrellaParry;
	parryTime_ -= kDeltaTime_;

	if (parryTime_ <= 0.0f) {
		isParry_ = false;
	}
}

//パリィ成功
void BaseUmbrella::ParrySuccess() {
	//SE
	Audio::GetInstance().StopWave(*parrySound_);//パリィが続くとき一度止めてから再生させるようにする
	Audio::GetInstance().SoundPlayWave(*parrySound_, kVolume_);//SE再生:パリィ
	////傘の座標を読み取る
	//Vector3 translate = GetTranslate();
	//translate += TransformNormal(GetTranslate(), wt_.GetMatWorld());//出す場所をwtGun_の向きの前に
	//particles_[particleParry_]->SetTranslate(translate);
	//particles_[particleParry_]->SetRotate(transform_.rotate);
	//particles_[particleParry_]->SetParticleBorn(ParticleBorn::MomentMode);
}