#include "Goal.h"
#include "CollisionManager.h"
#include "Input.h"

using namespace MyMath;

Goal::~Goal() {}

void Goal::Initialize(){
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("goal.obj");

	collisionType_ = CollisionTypes::TypeStageObject;
}
	
void Goal::Update(){
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
}
	
void Goal::Draw(){
	object_->Draw();
}

void Goal::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayer) {
		CollisionUtility::GetInstance().SuccessGoal(center_);//当たったらゴール
	}
}

bool Goal::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}
