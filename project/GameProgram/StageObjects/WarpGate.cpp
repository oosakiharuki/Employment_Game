#include "WarpGate.h"
using namespace MyMath;

WarpGate::~WarpGate() {}


void WarpGate::Initialize() {
	wt.Initialize();
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
}

void WarpGate::Update() {
	object_->Update(wt);
	wt.UpdateMatrix();
}

void WarpGate::Draw() {
	object_->Draw();
}
