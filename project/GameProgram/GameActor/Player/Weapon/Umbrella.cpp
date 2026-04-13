#include "Umbrella.h"
#include "ImGuiManager.h"
#include "UseEveryOne.h"

#include "Player.h"

using namespace MyMath;
using namespace UseEveryOne;

Umbrella::~Umbrella() {}

void Umbrella::Initialize() {
	
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("umbrella_Close.gltf");

	//AABBの大きさ
	umbrellaAABB_.min = -kAABBSize_ * kDivideByTwo_;
	umbrellaAABB_.max = kAABBSize_ * kDivideByTwo_;

	reaction_ = std::make_unique<Reaction>();

	collisionType_ = CollisionTypes::TypeUmbrella;
}

void Umbrella::Update() {

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

	//防御状態の場合
	if (!isShield_) {
		object_->ChangeAnimation("umbrella_Close.gltf");//閉じた傘
	}

	//パリィ更新処理
	ParryUpdate();

	//更新
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void Umbrella::Draw() {
	//描画
	object_->Draw();
}

void Umbrella::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeEnemyBullet && isShield_) {
		isHit_ = true;
		scaleTimer_ = 0.0f;
		transform_.scale = kDefaultScale_;

		//強化ゲージポイント加算
		player_->AddGaugePoint();

		if (collisionType_ == CollisionTypes::TypeUmbrellaParry) {
			parryTime_ = kParryTimeMax_;//連続で跳ね返せるように
			player_->ParrySuccess();//パリィ成功処理
			return;
		}
		//通常防御の場合、プレイヤーがノックバック
		player_->KnockBackUmbrella(kUmbrellaKnockBackPower_,kUmbrellaKnockBackTime_);
	}
}

bool Umbrella::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypeEnemyBullet && isShield_) {
		return true;
	}
	return false;
}

void Umbrella::ShieldMode() {
	//既に開いている場合はスキップ
	if (!isShield_) {		
		isParry_ = true;
	}
	isShield_ = true;

	//当たり判定設定
	collisionAABB_.min = transform_.translate + umbrellaAABB_.min;
	collisionAABB_.max = transform_.translate + umbrellaAABB_.max;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);

	object_->ChangeAnimation("umbrella_Open.gltf");//開いた傘
}

void Umbrella::ParryUpdate() {
	if (!isParry_) {
		parryTime_ = kParryTimeMax_;
		collisionType_ = CollisionTypes::TypeUmbrella;
		return;
	}

	collisionType_ = CollisionTypes::TypeUmbrellaParry;
	parryTime_ -= kDeltaTime_;

	if(parryTime_ <= 0.0f) {
		isParry_ = false;
	}
}