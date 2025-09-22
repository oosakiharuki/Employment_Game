#include "CheckPoint.h"
using namespace MyMath;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt.Initialize();
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");
}

void CheckPoint::Update() {
	wt.UpdateMatrix();
}

void CheckPoint::Draw() {
	object_->Draw(wt);
}
