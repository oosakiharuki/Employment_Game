#include "CheckPoint.h"
using namespace MyMath;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");
}

void CheckPoint::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void CheckPoint::Draw() {
	object_->Draw();
}
