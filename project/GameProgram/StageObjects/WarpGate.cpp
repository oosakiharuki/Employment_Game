#include "WarpGate.h"
using namespace MyMath;

WarpGate::~WarpGate() {}


void WarpGate::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
	//Transform更新処理
	transform_ = wt_.UpdateTransform();
}

void WarpGate::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void WarpGate::Draw() {
	//完全に小さくなったら映さない
	if (transform_.scale.x <= 0 && transform_.scale.y <= 0 && transform_.scale.z <= 0) {
		return;
	}
	
	object_->Draw();
}

void WarpGate::Vanish() {
	if (transform_.scale.x > 0 && transform_.scale.y > 0 && transform_.scale.z > 0) {
		smallingTimer_ += 0.1f;
		transform_.scale -= EaseIn(0.1f, kLittleLarge_ + smallingTimer_);
	}
}