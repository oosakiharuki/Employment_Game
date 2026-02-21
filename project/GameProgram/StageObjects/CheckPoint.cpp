#include "CheckPoint.h"
#include "CollisionManager.h"

using namespace MyMath;

CheckPoint::~CheckPoint() {}


void CheckPoint::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("checkpoint.obj");

	collisionType_ = CollisionTypes::stageObject;
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
	if (collision->GetType() == CollisionTypes::player) {
		transform_.rotate.y += 10.0f;
	}
}
