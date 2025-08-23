#include "WarpGate.h"
using namespace MyMath;

WarpGate::~WarpGate() {
	delete object_;
}


void WarpGate::Initialize() {
	wt.Initialize();
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
}

void WarpGate::Update() {
	wt.UpdateMatrix();
}

void WarpGate::Draw() {
	object_->Draw(wt);
}

AABB WarpGate::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + warpGateAABB.min;
	aabb.max = wt.translation_ + warpGateAABB.max;
	return aabb;
}