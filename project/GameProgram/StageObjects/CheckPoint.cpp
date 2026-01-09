#include "CheckPoint.h"
using namespace MyMath;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");
}

void CheckPoint::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void CheckPoint::Draw() {
	object_->Draw();
}
