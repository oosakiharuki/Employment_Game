#include "Umbrella.h"
#include "ImGuiManager.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

Umbrella::~Umbrella() {}

void Umbrella::Initialize() {
	wt_.Initialize();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("umbrella_Close.gltf");

	umbrellaAABB_.min = -kAABBSize_ * kDivideByTwo_;
	umbrellaAABB_.max = kAABBSize_ * kDivideByTwo_;
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


AABB Umbrella::GetAABB() {
	AABB aabb;
	aabb.min = wt_.translation_ + umbrellaAABB_.min;
	aabb.max = wt_.translation_ + umbrellaAABB_.max;
	return aabb;
}

void Umbrella::ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime) {
	if (scaleTimer_ >= maxTime / 2.0f) {
		wt_.scale_ -= scale;
		if (scaleTimer_ >= maxTime) {
			scaleTimer_ = 0.0f;
			wt_.scale_ = kDefaultScale_;
			//モーションを終了する
			*mosionOn = false;
		}
	}
	else {
		wt_.scale_ += scale;
	}
	scaleTimer_ += kDeltaTime;
}