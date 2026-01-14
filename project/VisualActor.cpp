#include "VisualActor.h"

using namespace MyMath;

VisualActor::VisualActor(){}
VisualActor::~VisualActor() {}

void VisualActor::Initialize(const std::string objectName) {
	baseObject_ = std::make_unique<Object_glTF>();
	baseObject_->Initialize();
	baseObject_->SetModelFile(objectName  + ".gltf");
	objectName_ = objectName;
}

void VisualActor::Update() {
	wt_.UpdateMatrix(transform_);
	baseObject_->Update(wt_);
}

void VisualActor::Draw() {
	baseObject_->Draw();
}

