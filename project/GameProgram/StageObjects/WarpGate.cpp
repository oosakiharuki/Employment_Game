#include "WarpGate.h"
using namespace MyMath;

WarpGate::~WarpGate() {}


void WarpGate::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
}

void WarpGate::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void WarpGate::Draw() {
	//完全に小さくなったら映さない
	if (wt_.scale_.x <= 0 && wt_.scale_.y <= 0 && wt_.scale_.z <= 0) {
		return;
	}
	
	object_->Draw();
}

void WarpGate::Vanish() {
	if (wt_.scale_.x > 0 && wt_.scale_.y > 0 && wt_.scale_.z > 0) {
		timer_ += 0.1f;
		wt_.scale_ -= EaseIn(0.1f, kLittleLarge_ + timer_);
	}
}