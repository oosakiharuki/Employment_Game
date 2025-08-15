#include "Umbrella.h"
#include "ImGuiManager.h"

using namespace MyMath;

Umbrella::~Umbrella() {
	delete object_Open;
	delete object_Close;
}

void Umbrella::Initialize() {
	wt.Initialize();
	object_Open = new Object3d();
	object_Open->Initialize();
	object_Open->SetModelFile("umbrella_Open.obj");

	object_Close = new Object3d();
	object_Close->Initialize();
	object_Close->SetModelFile("umbrella_Close.obj");

	umbrellaAABB.min = { -0.5f,-1,-0.5f };
	umbrellaAABB.max = { 0.5f,1,0.5f };
}

void Umbrella::Update() {
	
#ifdef _DEBUG

	ImGui::Begin("um");

	ImGui::InputFloat3("worldTransform.translate", &wt.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &wt.rotation_.x);
	ImGui::SliderFloat("RotateX", &wt.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &wt.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &wt.rotation_.z, -360.0f, 360.0f);

	ImGui::End();

#endif // _DEBUG

	wt.UpdateMatrix();
}


void Umbrella::Draw() {
	if (isShieldMode) {
		object_Open->Draw(wt);
	}
	else {
		object_Close->Draw(wt);
	}
}


AABB Umbrella::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + umbrellaAABB.min;
	aabb.max = wt.translation_ + umbrellaAABB.max;
	return aabb;
}