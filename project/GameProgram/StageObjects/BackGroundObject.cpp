/// ----------------------
///
/// ゲーム背景
/// 
/// ----------------------
#include "BackGroundObject.h"

void BackGroundObject::Initialize() {
	//オブジェクト初期化
	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("backGroundObject.gltf");
}

void BackGroundObject::Update() {
	object_->Update();
}

void BackGroundObject::Draw() {
	object_->Draw();
}
