#include "WarpGate.h"
using namespace MyMath;

WarpGate::~WarpGate() {}


void WarpGate::Initialize() {
	wt.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
}

void WarpGate::Update() {
	object_->Update(wt);
	wt.UpdateMatrix();
}

void WarpGate::Draw() {
	//完全に小さくなったら映さない
	if (wt.scale_.x <= 0 && wt.scale_.y <= 0 && wt.scale_.z <= 0) {
		return;
	}
	
	object_->Draw();
}

void WarpGate::Vanish() {
	if (wt.scale_.x > 0 && wt.scale_.y > 0 && wt.scale_.z > 0) {
		t += 0.1f;
		wt.scale_ -= EaseIn(0.1f, littleLarge + t);
	}
}

void WarpGate::SetObjectName() {
	objectName = "WarpGate";
}