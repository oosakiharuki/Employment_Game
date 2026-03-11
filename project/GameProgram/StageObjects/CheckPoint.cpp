#include "CheckPoint.h"
#include "CollisionManager.h"
#include "NextStageSave.h"

using namespace MyMath;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");

	collisionType_ = CollisionTypes::TypeStageObject;
}

void CheckPoint::Update() {
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
	
	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().AddCollisions(this);
}

void CheckPoint::Draw() {
	object_->Draw();
}

void CheckPoint::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayer) {
		transform_.rotate.x -= 45.0f;
		transform_.rotate.x = std::clamp(transform_.rotate.x,-90.0f,0.0f);
		NextStageSave::GetInstance().SetCheckPoint(transform_.translate);
	}
}

bool CheckPoint::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}
