#include "BackGroundObject.h"

void BackGroundObject::Initialize() {
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("skybox.gltf");

	wt_.Initialize();

	//サイズを大きく
	transform_.scale = { kSize_, kSize_, kSize_ };
	wt_.UpdateMatrix(transform_);
}

void BackGroundObject::Update() {
	object_->Update(wt_);
}

void BackGroundObject::Draw() {
	object_->Draw();
}
