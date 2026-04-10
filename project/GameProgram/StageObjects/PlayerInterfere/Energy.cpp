#include "Energy.h"
#include "MyMath.h"

using namespace MyMath;

void Energy::Initialize() {
	//モデル作成
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("energy.obj");

	wt_.Initialize();
	collisionType_ = TypeEnergy;
}

void Energy::Update() {
	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	//当たり判定の設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
}

void Energy::Draw() {
	object_->Draw();
}

void Energy::OnCollision(CollisionSource* collisionSource) {
	if (collisionSource->GetType() == CollisionTypes::TypePlayer) {

	}
}

bool Energy::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}


