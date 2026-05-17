/// ----------------------------------
///
/// 壊れる箱
/// プレイヤーの弾丸が当たれば壊れる
/// 
/// ----------------------------------
#include "BrokenBox.h"
#include "UseEveryOne.h"
#include "MyMath.h"

#include "GLTFCommon.h"
#include "Object3dCommon.h"
#include <TimeScale.h>

using namespace UseEveryOne;
using namespace MyMath;

void BrokenBox::Initialize() {
	objectBox_ = std::make_unique<EngineLayer::Object_glTF>();
	objectBox_->Initialize();
	objectBox_->SetModelFile("Box.gltf");

	collisionType_ = CollisionTypes::TypeStage;
}

void BrokenBox::Update() {	
	if (isFinish_) return;

	if (isBroken_) {
		//壊れたアニメーションに変更
		objectBox_->ChangeAnimation("BrokenBox.gltf");
		objectBox_->SetAnimationTime(TimeScale::GetInstance().GetTimeScale());
		
		if (timer >= kAnimationTimeMax_) {
			isFinish_ = true;
		}
		else {
			timer += TimeScale::GetInstance().GetTimeScale();
		}
	}

	wt_.UpdateMatrix(transform_);
	objectBox_->Update(wt_);

	if (isBroken_) return;

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);

}
void BrokenBox::Draw() {
	if (isFinish_) return;
	
	EngineLayer::GLTFCommon::GetInstance().Command();

	objectBox_->Draw();
	
	EngineLayer::Object3dCommon::GetInstance().Command();

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
