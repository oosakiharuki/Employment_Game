#include "Needle.h"
#include "UseEveryOne.h"

#include "Object3dCommon.h"
#include "GLTFCommon.h"


using namespace MyMath;
using namespace UseEveryOne;

void Needle::Initialize() {
	objectNeedle_ = std::make_unique<Object_glTF>();
	objectNeedle_->Initialize();
	objectNeedle_->SetModelFile("needle.gltf");

	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();

	wt_.Initialize();
	transform_ = wt_.UpdateTransform();

	collisionType_ = CollisionTypes::TypeEnemyBullet;//ダメージ
}

void Needle::Update() {

	Move();

	AnimationRotate();

	wt_.UpdateMatrix(transform_);
	objectNeedle_->Update(wt_);

	shadow_->SetTranslate(transform_.translate);
	shadow_->SetScale(transform_.scale);
	shadow_->Update();

	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
}

void Needle::Draw() {
	GLTFCommon::GetInstance().Command();

	objectNeedle_->Draw();
	
	Object3dCommon::GetInstance().Command();

	shadow_->Draw();
}

void Needle::SetTravelRoute(const Vector3& nowPoint, const Vector3& pointS, const Vector3& pointE) {
	//現在の位置を設定
	nowPoint_ = nowPoint;
	//移動箇所を設定(現在二か所)
	startPoint_ = pointS;
	endPoint_ = pointE;
	//
	movePoint_.origin = nowPoint;
	movePoint_.diff = pointE;
}

void Needle::Move() {
	//現在のポイントに移行
	transform_.translate = nowPoint_;

	//全体の長さを加算
	Vector3 length = Length(movePoint_.origin, movePoint_.diff);
	float addLength = (length.x + length.y + length.z) / moveMaxTime_;

	timer_ += kDeltaTime_ / addLength;//時間を移動の長さ分割る
	timer_ = std::clamp(timer_, 0.0f, 1.0f);
	//現在ポイントの変更
	nowPoint_ = Lerp(movePoint_.diff, movePoint_.origin, timer_);
	
	//移動方法(始点終点を往復する)
	//現在ポイントが終点に着いたとき
	if (nowPoint_ == endPoint_) {
		//終点から始点に移動
		movePoint_.origin = endPoint_;
		movePoint_.diff = startPoint_;
		timer_ = 0.0f;//タイマーリセット
	}//現在ポイントが始点に着いたとき
	else if (nowPoint_ == startPoint_) {
		//始点から終点に移動
		movePoint_.origin = startPoint_;
		movePoint_.diff = endPoint_;
		timer_ = 0.0f;//タイマーリセット
	}
}

void Needle::AnimationRotate() {
	animationTimer_ += kAnimationTimeSpeed_;
	transform_.rotate.z = animationTimer_;
}


void Needle::OnCollision(CollisionSource* collisionSource) {
	//無敵だからないかも
}

bool Needle::TypeCheckUp(const CollisionTypes& collisionType) {
	return false;
}

