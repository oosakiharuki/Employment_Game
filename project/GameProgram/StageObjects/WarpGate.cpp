#include "WarpGate.h"
#include "UseEveryOne.h"
#include "CollisionManager.h"
#include <Input.h>
#include <NextStageSave.h>

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

	collisionType_ = CollisionTypes::stageObject;
}

void WarpGate::Update() {

	if (scaleFlag_ && transform_.scale != kDefaultScale_ && !largeFlag_) {
		scaleFlag_ = false;
		scaleTimer_ = 0.0f;
	}

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().AddCollisions(this);
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

void WarpGate::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::player) {
		scaleTimer_ += kDeltaTime_ * 2.0f;
		scaleTimer_ = std::clamp(scaleTimer_, 0.0f, 1.0f);

		transform_.scale = EaseOut(kDefaultScale_ * 1.5f,kDefaultScale_, scaleTimer_);
		scaleFlag_ = true;
		if (Input::GetInstance().TriggerKey(DIK_E) || a) {
			CollisionManager::GetInstance().SuccessWarp();
			NextStageSave::GetInstance().SetNextStageFile(fileName_);
			a = true;
		}
	}
}
