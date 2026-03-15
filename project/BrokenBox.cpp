#include "BrokenBox.h"
#include "UseEveryOne.h"
#include "MyMath.h"

using namespace UseEveryOne;
using namespace MyMath;

void BrokenBox::Initialize() {
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("Box.gltf");

	wt_.Initialize();
	transform_ = wt_.UpdateTransform();
	transform_.translate.y += 5.0f;

	collisionType_ = CollisionTypes::TypeStage;
}

void BrokenBox::Update() {	
	if (isFinish_) return;

	if (isBroken_) {
		//壊れたアニメーションに変更
		object_->ChangeAnimation("BrokenBox.gltf");
		
		if (timer >= kAnimationTimeMax_) {
			isFinish_ = true;
		}
		else {
			timer += kDeltaTime_;
		}
	}

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	if (isBroken_) return;

	//当たり判定設定
	collisionAABB_.min = transform_.translate - (kDefaultScale_);
	collisionAABB_.max = transform_.translate + (kDefaultScale_);
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);

}
void BrokenBox::Draw() {
	if (isFinish_) return;
	object_->Draw();
	
}

void BrokenBox::OnCollision(CollisionSource* collision) {
	isBroken_ = true;//壊れるフラグオン
}

bool BrokenBox::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayerBullet) {
		return true;
	}
	return false;
}
