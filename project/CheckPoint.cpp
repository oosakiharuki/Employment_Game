#include "CheckPoint.h"
using namespace MyMath;

CheckPoint::~CheckPoint() {
	delete object_;
}


void CheckPoint::Initialize() {
	wt.Initialize();
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("playerHead.obj");
}

void CheckPoint::Update() {
	wt.UpdateMatrix();
}

void CheckPoint::Draw() {
	object_->Draw(wt);
}

AABB CheckPoint::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + checkPointAABB.min;
	aabb.max = wt.translation_ + checkPointAABB.max;
	return aabb;
}