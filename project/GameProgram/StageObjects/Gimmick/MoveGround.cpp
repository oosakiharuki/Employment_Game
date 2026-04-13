#include "MoveGround.h"

using namespace UseEveryOne;
using namespace MyMath;

MoveGround::~MoveGround() {}

void MoveGround::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("moveGround.obj");

	collisionType_ = TypeMoveGround;//地面判定を付けるため
}

void MoveGround::Update() {
	//移動
	Move();

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);

	CollisionUtility::GetInstance().ResetMoveValue();
}

void MoveGround::Draw() {
	object_->Draw();
}

void MoveGround::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayer || collision->GetType() == CollisionTypes::TypeEnemy) {
		//移動量を配列に追加する(座標、移動量(現在位置と前回位置の減算))
		CollisionUtility::GetInstance().AddMoveValue(nowPoint_,nowPoint_ - transform_.translate);
	}
}

bool MoveGround::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer || collisionType == CollisionTypes::TypeEnemy) {
		return true;
	}
	return false;
}


void MoveGround::SetTravelRoute(const Vector3& nowPoint, const Vector3& pointS, const Vector3& pointE) {
	//現在の位置を設定
	nowPoint_ = nowPoint;
	//移動箇所を設定(現在二か所)
	startPoint_ = pointS;
	endPoint_ = pointE;
	//
	movePoint_.origin = nowPoint;
	movePoint_.diff = pointE;
}

void MoveGround::Move() {
	//現在のポイントに移行
	transform_.translate = nowPoint_;

	interpolation_ += kDeltaTime_ / kMoveFrame_;//時間経過(4秒間)
	interpolation_ = std::clamp(interpolation_, 0.0f, 1.0f);
	//現在ポイントの変更
	nowPoint_ = EaseInOut(movePoint_.diff, movePoint_.origin, interpolation_);

	//移動方法(始点終点を往復する)
	//現在ポイントが終点に着いたとき
	if (nowPoint_ == endPoint_) {
		//終点から始点に移動
		movePoint_.origin = endPoint_;
		movePoint_.diff = startPoint_;
		interpolation_ = 0.0f;//補間リセット
	}//現在ポイントが始点に着いたとき
	else if (nowPoint_ == startPoint_) {
		//始点から終点に移動
		movePoint_.origin = startPoint_;
		movePoint_.diff = endPoint_;
		interpolation_ = 0.0f;//補間リセット
	}

}
