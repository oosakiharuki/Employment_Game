/// ---------------------
///
/// チェックポイント
/// 
/// ---------------------
#include "CheckPoint.h"
#include "CollisionManager.h"
#include "NextStageSave.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<EngineLayer::Object3d>();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");

	collisionType_ = CollisionTypes::TypeStageObject;
}

void CheckPoint::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
	
	if (isTouch_) {
		rotateTimer_ += kDeltaTime_ * kTwice_;//二倍速
		rotateTimer_ = std::clamp(rotateTimer_, 0.0f, 1.0f);
		transform_.rotate.x = EaseOut(kRotateMax_, 0.0f, rotateTimer_);
	}

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);		
}

void CheckPoint::Draw() {
	object_->Draw();
}

void CheckPoint::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayer) {
		isTouch_ = true;
		NextStageSave::GetInstance().SetCheckPoint(transform_.translate);
	}
}

bool CheckPoint::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}
