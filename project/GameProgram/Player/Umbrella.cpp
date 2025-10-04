#include "Umbrella.h"
#include "ImGuiManager.h"

using namespace MyMath;

Umbrella::~Umbrella() {}

void Umbrella::Initialize() {
	wt.Initialize();

	object = std::make_unique<Object_glTF>();
	object->Initialize();
	object->SetModelFile("umbrella_Close.gltf");

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

	if (isShieldMode) {
		object->ChangeAnimation("umbrella_Open.gltf");
	}
	else {
		object->ChangeAnimation("umbrella_Close.gltf");
	}

	object->Update(wt);
	wt.UpdateMatrix();
}


void Umbrella::Draw() {
	object->Draw();
}


AABB Umbrella::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + umbrellaAABB.min;
	aabb.max = wt.translation_ + umbrellaAABB.max;
	return aabb;
}

void Umbrella::ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime) {
	if (scaleTimer >= maxTime / 2.0f) {
		wt.scale_ -= scale;
		if (scaleTimer >= maxTime) {
			scaleTimer = 0.0f;
			wt.scale_ = { 1,1,1 };
			//モーションを終了する
			*mosionOn = false;
		}
	}
	else {
		wt.scale_ += scale;
	}
	scaleTimer += deltaTime;
}