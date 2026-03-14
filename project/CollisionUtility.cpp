#include "CollisionUtility.h"

#include "GameActor.h"
#include "GravityActor.h"

#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

std::unique_ptr<CollisionUtility> CollisionUtility::sInstance_ = nullptr;

CollisionUtility& CollisionUtility::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<CollisionUtility>();
	}
	return *sInstance_;
}

void CollisionUtility::Finalize() {
	sInstance_.reset();
}

void CollisionUtility::UnderCollision(float& minUnder, const Vector3& actorPosition, const AABB& stageAABB) const {
	//できる限り下の値
	float underY = stageAABB.max.y + kShadowUp_;

	//プレイヤーと現段階短い距離
	float lengthMin = Length(actorPosition.y, minUnder);

	//プレイヤーとステージの上の長さ
	float length = Length(actorPosition.y, underY);

	//プレイヤーと足場の長さが一番短いところを影の場所とする
	if (length < lengthMin) {
		//値が変更
		minUnder = underY;
	}
}

void CollisionUtility::GameActorAndStageCollision(CollisionOverlap& collisionOverlap, GameActor& gameActor, GravityActor& gravityActor, const AABB& otherCollisionAABB) {

	//両方ともtrueの時
	if (collisionOverlap.isWall && collisionOverlap.isGround) {
		return;
	}

	//演出や死んだときは発動しない
	if (!gameActor.GetHp() == 0 && !gameActor.GetPerformanceMode()) {
		collisionOverlap.stageAABB = otherCollisionAABB;
		//重なった部分
		collisionOverlap.overlap = OverAABB(collisionOverlap.targetAABB, otherCollisionAABB);
		//場所を戻す・壁と床の判定
		BackPosition(collisionOverlap);
	}
	//地面にいる判定(床の判定がtrueの場合)
	gravityActor.IsGround(collisionOverlap.isGround);
	//戻った場所を代入
	gameActor.SetTranslate(collisionOverlap.position);
}

void CollisionUtility::BackPosition(CollisionOverlap& collisionOverlap) {

	// 重なりが一番小さい軸の押し戻しを行う	
	if (collisionOverlap.overlap.x < collisionOverlap.overlap.y) {
		//真ん中の座標を代入
		float targetCenterX = (collisionOverlap.targetAABB.min.x + collisionOverlap.targetAABB.max.x) * kDivideByTwo_;
		float areaCenterX = (collisionOverlap.stageAABB.min.x + collisionOverlap.stageAABB.max.x) * kDivideByTwo_;
		//真ん中から 右の場合 - / 左の場合 +
		float push = (targetCenterX < areaCenterX) ? -collisionOverlap.overlap.x : collisionOverlap.overlap.x;

		collisionOverlap.position.x += push;
		collisionOverlap.isWall = true;
	}
	else if (collisionOverlap.overlap.y < collisionOverlap.overlap.x) {
		// 真ん中の座標を代入
		float targetCenterY = (collisionOverlap.targetAABB.min.y + collisionOverlap.targetAABB.max.y) * kDivideByTwo_;
		float areaCenterY = (collisionOverlap.stageAABB.min.y + collisionOverlap.stageAABB.max.y) * kDivideByTwo_;
		//真ん中から 下の場合 - / 上の場合 +
		float push = (targetCenterY < areaCenterY) ? -collisionOverlap.overlap.y : collisionOverlap.overlap.y;

		//床 or 天井 (targetCenterYが上は床、areaCenterYが上は天井)
		if (targetCenterY > areaCenterY) {
			// 着地判定を立てる
			collisionOverlap.isGround = true;
		}
		collisionOverlap.position.y += push;
	}
	//z軸はいらない

}

CollisionOverlap CollisionUtility::SetTarget(const Vector3& position, const AABB& aabb) {
	CollisionOverlap result;

	result.position = position;//座標
	result.targetAABB = aabb;  //当たり判定AABB
	result.isGround = false;   //地面判定フラグ
	result.isWall = false;     //壁判定フラグ

	return result;
}


void CollisionUtility::SuccessGoal(const Vector3& center) {
	isGoal_ = true;
	SetZoomPoint(center);
}

void CollisionUtility::SuccessWarp(const Vector3& center) {
	isWarp_ = true;
	SetZoomPoint(center);
}

void CollisionUtility::SetZoomPoint(const Vector3& point) { zoomPoint_ = point; }

void CollisionUtility::ResetFrag() {
	isWarp_ = false;
	isGoal_ = false;
}