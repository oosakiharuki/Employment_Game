#include "Umbrella.h"
#include "ImGuiManager.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

Umbrella::~Umbrella() {}

void Umbrella::Initialize() {
	Actor_InitializeCommon();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("umbrella_Close.gltf");

	actorAABB_.min = -kAABBSize_ * kDivideByTwo_;
	actorAABB_.max = kAABBSize_ * kDivideByTwo_;
}

void Umbrella::Update() {
	
#ifdef USE_IMGUI

	ImGui::Begin("um");

	ImGui::InputFloat3("worldTransform.translate", &wt_.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &wt_.rotation_.x);
	ImGui::SliderFloat("RotateX", &wt_.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &wt_.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &wt_.rotation_.z, -360.0f, 360.0f);

	ImGui::End();

#endif // USE_IMGUI

	//防御状態の場合
	if (isShieldMode_) {
		object_->ChangeAnimation("umbrella_Open.gltf");//開いた傘
	}
	else {
		object_->ChangeAnimation("umbrella_Close.gltf");//閉じた傘
	}

	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void Umbrella::Draw() {
	object_->Draw();
}
