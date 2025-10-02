#include "Shadow.h"
#include "Object3dCommon.h"
#include "GLTFCommon.h"
#include "ImGuiManager.h"

using namespace MyMath;

Shadow::~Shadow() {
	delete object_;
}

void Shadow::Initialize() {

	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("shadow.obj");
	object_->SetColor({0,0,0,1});

	wt.Initialize();

	shadowAABB = {
		{ -0.1f,-1000.0f,-0.1f },
		{ -0.1f,0.0f,-0.1f }
	};
}


void Shadow::Update() {

#ifdef _DEBUG

	ImGui::Begin("player_shadow");

	ImGui::InputFloat3("worldTransform.translate", &wt.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt.translation_.x, -30.0f, 30.0f);

	ImGui::End();

#endif // _DEBUG

	object_->Update(wt);
	wt.UpdateMatrix();
}

void Shadow::Draw() {
	if (object_) {

		Object3dCommon::GetInstance()->Command();
		object_->Draw();

	}
}

AABB Shadow::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + shadowAABB.min;
	aabb.max = wt.translation_ + shadowAABB.max;
	return aabb;
}
