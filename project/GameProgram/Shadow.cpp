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

	wt_.Initialize();
}


void Shadow::Update() {

	ImGui::Begin("player_shadow");

	ImGui::InputFloat3("worldTransform.translate", &wt_.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt_.translation_.x, -30.0f, 30.0f);

	ImGui::End();


	wt_.UpdateMatrix();
}

void Shadow::Draw() {
	if (object_) {

		Object3dCommon::GetInstance()->Command();
		object_->Draw(wt_);

	}
}


