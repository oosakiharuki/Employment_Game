#include "Shadow.h"
#include "Object3dCommon.h"
#include "GLTFCommon.h"
#include "ImGuiManager.h"

using namespace MyMath;

Shadow::~Shadow() {}

void Shadow::Initialize() {

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("shadow.obj");
	//色を黒に
	object_->SetColor(kColor_);

	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	collisionType_ = CollisionTypes::TypeShadow;
}


void Shadow::Update() {

#ifdef USE_IMGUI

	ImGui::Begin("player_shadow");
	ImGui::Text("worldTransform.translate : %f, %f, %f", &transform_.translate.x, &transform_.translate.y, &transform_.translate.z);
	ImGui::End();

#endif // USE_IMGUI

	transform_.translate = actorPosition_;
	transform_.translate.y = minUnder_;

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	collisionAABB_.min = actorPosition_ - Vector3{ kShadowWidth_, kShadowMinY_, kShadowWidth_ };
	collisionAABB_.max = actorPosition_ + Vector3{ kShadowWidth_, 0.0f, kShadowWidth_ };
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
	minUnder_ = -kShadowMinY_;
}

void Shadow::Draw() {
	Object3dCommon::GetInstance().Command();
	object_->Draw();
}

void Shadow::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeStage) {
		CollisionManager::GetInstance().UnderCollision(minUnder_,actorPosition_,collision->GetAABB());
	}
}

bool Shadow::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypeStage) {
		return true;
	}
	return false;
}