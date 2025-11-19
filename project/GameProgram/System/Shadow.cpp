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
	object_->SetColor({0,0,0,1});

	wt_.Initialize();

	shadowAABB_ = {
		{ -0.1f,-1000.0f,-0.1f },
		{ -0.1f,0.0f,-0.1f }
	};
}


void Shadow::Update() {

#ifdef USE_IMGUI

	ImGui::Begin("player_shadow");

	ImGui::InputFloat3("worldTransform.translate", &wt_.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt_.translation_.x, -30.0f, 30.0f);

	ImGui::End();

#endif // USE_IMGUI

	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void Shadow::Draw() {
	if (object_) {

		Object3dCommon::GetInstance()->Command();
		object_->Draw();

	}
}

AABB Shadow::GetAABB() {
	AABB aabb;
	aabb.min = wt_.translation_ + shadowAABB_.min;
	aabb.max = wt_.translation_ + shadowAABB_.max;
	return aabb;
}
