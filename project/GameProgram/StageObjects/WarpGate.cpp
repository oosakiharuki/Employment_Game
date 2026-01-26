#include "WarpGate.h"
#include "UseEveryOne.h"
using namespace MyMath;
using namespace UseEveryOne;
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
	//小さくて見えないためすぐに返す
	if (smallingTimer_ < 1.0f) {

		if (largeFlag_) {
			largeTimer_ += kDeltaTime_;
			transform_.scale = EaseOut(kLargeMax_, kDefaultScale_, largeTimer_);
		}
		else {
			smallingTimer_ += kDeltaTime_;
			transform_.scale = EaseIn(kLargeMax_, { 0,0,0 }, smallingTimer_);
		}
		//補間がMaxに達した時
		if (largeTimer_ >= 1.0f) {
			largeFlag_ = false;//拡大を終了
		}
	}
}
