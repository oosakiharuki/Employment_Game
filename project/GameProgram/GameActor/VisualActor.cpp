#include "VisualActor.h"

using namespace MyMath;

VisualActor::VisualActor(){}
VisualActor::~VisualActor() {}

void VisualActor::Initialize(const std::string objectName) {
	baseObject_ = std::make_unique<Object_glTF>();
	baseObject_->Initialize();
	baseObject_->SetModelFile(objectName  + ".gltf");
	objectName_ = objectName;//オブジェクト名を設定しておく
	baseObject_->LightSwitch(false);//ライトのスイッチ初期化
}

void VisualActor::Update() {
	wt_.UpdateMatrix(transform_);
	baseObject_->Update(wt_);
}

void VisualActor::Draw() {
	baseObject_->Draw();
}

void VisualActor::LightOn() {
	baseObject_->LightSwitch(true);
}

