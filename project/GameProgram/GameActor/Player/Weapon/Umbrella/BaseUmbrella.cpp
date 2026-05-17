/// ------------------
///
/// 傘銃の基盤クラス
/// 
/// ------------------
#include "BaseUmbrella.h"
#include "Player.h"
#include "UseEveryOne.h"
#include "ImGuiManager.h"

#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

void BaseUmbrella::Initialize() {

	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	//オブジェクト初期化(モデル設定以外)
	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();

	//リアクション
	reaction_ = std::make_unique<Reaction>();
	//当たりタイプ
	collisionType_ = CollisionTypes::TypeUmbrella;

	particles_[kParticleParry_] = EngineLayer::ParticleManager::GetInstance().InitParticle(kParticleParry_);

	//傘を開くSE
	EngineLayer::Audio::GetInstance().LoadWave(kUmbrellaOpenSoundName_);
	//発砲攻撃
	EngineLayer::Audio::GetInstance().LoadWave(kFireSoundName_);
	//パリィ
	EngineLayer::Audio::GetInstance().LoadWave(kParrySoundName_);
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

	for (auto& particle : particles_) {
		particle.second->Update();
	}

	//パリィ更新処理
	ParryUpdate();
}

void BaseUmbrella::Draw() {
	object_->Draw();

	EngineLayer::ParticleCommon::GetInstance().Command();

	for (auto& particle : particles_) {
		particle.second->Draw();
	}

	EngineLayer::GLTFCommon::GetInstance().Command();
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

void BaseUmbrella::FireCommand() {

	if (player_->UseGaugePoint()) {
		//弾丸速度が二倍、より遠くに飛ばせる
		PowerFire();
	}
	else {
		Fire();//発砲攻撃
	}

	EngineLayer::Audio::GetInstance().StopWave(kFireSoundName_);
	EngineLayer::Audio::GetInstance().SoundPlayWave(kFireSoundName_, kVolume_);

}

void BaseUmbrella::BornBullet(const Vector3& translate, const Vector3& velocity, uint32_t bulletPower) {
	//弾丸を生み出す
	std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
	bullet->Initialize();//初期化
	bullet->SetTranslate(translate);//発泡初期位置
	bullet->SetVelocity(velocity);//速さ	
	bullet->SetStrongPower(bulletPower);//弾丸の攻撃力を導入
	player_->AddBullet(std::move(bullet));
}


void BaseUmbrella::OffShield() {
	isShield_ = false;
	EngineLayer::Audio::GetInstance().StopWave(kUmbrellaOpenSoundName_);
}

void BaseUmbrella::ShieldMode() {
	//既に開いている場合はスキップ
	if (!isShield_) {
		isParry_ = true;
		EngineLayer::Audio::GetInstance().SoundPlayWave(kUmbrellaOpenSoundName_, kVolume_ * kTwice_);
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
	EngineLayer::Audio::GetInstance().StopWave(kParrySoundName_);//パリィが続くとき一度止めてから再生させるようにする
	EngineLayer::Audio::GetInstance().SoundPlayWave(kParrySoundName_, kVolume_);//SE再生:パリィ
	//パリィエフェクトを出す座標
	Vector3 translate = TransformNormal(kParryParticleFrontPoint_, wt_.GetMatWorld());
	particles_[kParticleParry_]->SetTranslate(transform_.translate + translate);//生成場所を設定
	particles_[kParticleParry_]->SetRotate(transform_.rotate + kParryParticleRotate_);//回転を設定(パリィ向きの調整)
	particles_[kParticleParry_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);//一瞬だけ出すモード
}