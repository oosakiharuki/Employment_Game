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

	shadowAABB_ = {
		{ kShadowWidth_,kShadowMinY_,kShadowWidth_ },
		{ -kShadowWidth_,0.0f,-kShadowWidth_ }
	};
}


void Shadow::Update() {

#ifdef USE_IMGUI

	ImGui::Begin("player_shadow");
	ImGui::Text("worldTransform.translate : %f, %f, %f", &wt_.translation_.x, &wt_.translation_.y, &wt_.translation_.z);
	ImGui::End();

#endif // USE_IMGUI

	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void Shadow::Draw() {
	Object3dCommon::GetInstance()->Command();
	object_->Draw();
}

AABB Shadow::GetAABB() {
	AABB aabb;
	aabb.min = wt_.translation_ + shadowAABB_.min;
	aabb.max = wt_.translation_ + shadowAABB_.max;
	return aabb;
}
