#include "Umbrella.h"
#include "ImGuiManager.h"

using namespace MyMath;

Umbrella::~Umbrella() {
	delete object_;
}

void Umbrella::Initialize() {
	wt.Initialize();
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("umbrella.obj");

	umbrellaAABB.min = { -1,-1,-1 };
	umbrellaAABB.max = { 1,1,1 };
}

void Umbrella::Update() {
	
	ImGui::Begin("um");

	ImGui::InputFloat3("worldTransform.translate", &wt.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &wt.rotation_.x);
	ImGui::SliderFloat("RotateX", &wt.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &wt.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &wt.rotation_.z, -360.0f, 360.0f);

	ImGui::End();

	wt.UpdateMatrix();
}


void Umbrella::Draw() {
	object_->Draw(wt);
}


AABB Umbrella::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + umbrellaAABB.min;
	aabb.max = wt.translation_ + umbrellaAABB.max;
	return aabb;
}