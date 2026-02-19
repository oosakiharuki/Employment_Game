#include "Umbrella.h"
#include "ImGuiManager.h"
#include "UseEveryOne.h"

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

	collisionType_ = CollisionTypes::umbrella;
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

	//防御状態の場合
	if (isShieldMode_) {
		object_->ChangeAnimation("umbrella_Open.gltf");//開いた傘
	}
	else {
		object_->ChangeAnimation("umbrella_Close.gltf");//閉じた傘
	}

	//更新
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	//当たり判定設定
	collisionAABB_.min = transform_.translate + umbrellaAABB_.min;
	collisionAABB_.max = transform_.translate + umbrellaAABB_.max;
	center_ = transform_.translate;

	CollisionManager::GetInstance().AddCollisions(this);
}

void Umbrella::Draw() {
	//描画
	object_->Draw();
}

void Umbrella::HitReaction(bool& isShieldMode) {
	reaction_->ScaleReaction(transform_.scale,isShieldMode,kScalePower_, scaleTimer_,kReactionMaxTime_);
}

void Umbrella::OnCollision(CollisionSource* collision) {}
