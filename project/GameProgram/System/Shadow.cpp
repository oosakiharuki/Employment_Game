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

	shadowAABB_ = {
		{ kShadowWidth_,kShadowMinY_,kShadowWidth_ },
		{ -kShadowWidth_,0.0f,-kShadowWidth_ }
	};
}


void Shadow::Update() {

#ifdef USE_IMGUI

	ImGui::Begin("player_shadow");
	ImGui::Text("worldTransform.translate : %f, %f, %f", &transform_.translate.x, &transform_.translate.y, &transform_.translate.z);
	ImGui::End();

#endif // USE_IMGUI

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void Shadow::Draw() {
	Object3dCommon::GetInstance().Command();
	object_->Draw();
}

AABB Shadow::GetAABB() {
	AABB aabb;
	aabb.min = transform_.translate + shadowAABB_.min;
	aabb.max = transform_.translate + shadowAABB_.max;
	return aabb;
}
