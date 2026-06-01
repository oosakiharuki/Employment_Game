/// ----------------------
///
/// ゲーム背景
/// 
/// ----------------------
#include "BackGroundObject.h"

void BackGroundObject::Initialize() {
	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("backGroundObject.gltf");

	wt_.Initialize();
	transform_ = wt_.UpdateTransform();
	transform_.translate.y = -10.0f;
	transform_.translate.z += 100.0f;
	wt_.UpdateMatrix(transform_);
}

void BackGroundObject::Update() {
	object_->Update(wt_);
}

void BackGroundObject::Draw() {
	object_->Draw();
}
