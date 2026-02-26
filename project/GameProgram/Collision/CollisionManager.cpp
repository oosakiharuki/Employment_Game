#include "CollisionManager.h"
#include "MyMath.h"
#include "CheckPoint.h"
#include <Goal.h>
#include <WarpGate.h>
#include <NextStageSave.h>

#include "GameActor.h"
#include "GravityActor.h"

using namespace MyMath;
using namespace UseEveryOne;

std::unique_ptr<CollisionManager> CollisionManager::sInstance_ = nullptr;

CollisionManager& CollisionManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<CollisionManager>();
	}
	return *sInstance_;
}

void CollisionManager::Finalize() {
	sInstance_.reset();
}

void CollisionManager::AddCollisions(CollisionSource* addCollision) {
	collisions_.push_back(addCollision);
}

void CollisionManager::CreateCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type) {
	std::unique_ptr<CollisionSource> collision;
	collision = std::make_unique<CollisionSource>();

	collision->SetAABB(collisionAABB);
	collision->SetCenter(center);
	collision->SetType(type);

	collisions_.push_back(&*collision);
}

void CollisionManager::CreateStageCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type) {

	if (isAlreadyInStage_) {
		//新しくステージ全体の当たり判定を作るためリセット
		stageCollisions_.clear();
		isAlreadyInStage_ = false;
	}

	std::unique_ptr<CollisionSource> collision;
	collision = std::make_unique<CollisionSource>();

	collision->SetAABB(collisionAABB);
	collision->SetCenter(center);
	collision->SetType(type);

	stageCollisions_.push_back(std::move(collision));
}

void CollisionManager::CollisionUpdate() {

	isAlreadyInStage_ = true;

	for (auto& stageCollision : stageCollisions_) {
		collisions_.push_back(&*stageCollision);
	}

	for (uint32_t i = 0; i < collisions_.size();i++) {
		for (uint32_t j = 0; j < collisions_.size(); j++) {
			//iとjが同じ値ではしない(自分自身なので)
			if (i == j) {
				continue;
			}
			//当たり判定
			if (IsCollisionAABB(collisions_[i]->GetAABB(), collisions_[j]->GetAABB()) && collisions_[i]->GetType() != collisions_[j]->GetType()) {
				EachCollision(*collisions_[i], *collisions_[j]);
			}
		}
	}
	//リセット
	collisions_.clear();
}

void CollisionManager::EachCollision(CollisionSource& collisionA, CollisionSource& collisionB) {
	//プレイヤー
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::enemyBullet);
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::enemyEye);
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::boss);
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::bombExplotion); 
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::stage);
	DetermineType(collisionA, CollisionTypes::player, collisionB, CollisionTypes::event);
	//弾丸(player)
	DetermineType(collisionA,CollisionTypes::playerBullet,collisionB,CollisionTypes::stage);
	//傘
	DetermineType(collisionA, CollisionTypes::umbrella, collisionB, CollisionTypes::enemyBullet);
	DetermineType(collisionA, CollisionTypes::umbrellaParry, collisionB, CollisionTypes::enemyBullet);
	//敵
	DetermineType(collisionA, CollisionTypes::enemy, collisionB, CollisionTypes::playerBullet);
	DetermineType(collisionA, CollisionTypes::enemy, collisionB, CollisionTypes::parryBullet);
	DetermineType(collisionA, CollisionTypes::enemy, collisionB, CollisionTypes::stage);
	//敵のサーチ範囲
	DetermineType(collisionA, CollisionTypes::enemyEye, collisionB, CollisionTypes::player);
	DetermineType(collisionA, CollisionTypes::enemyEye, collisionB , CollisionTypes::stage);
	//弾丸(enemy)
	DetermineType(collisionA, CollisionTypes::enemyBullet, collisionB, CollisionTypes::stage);
	//ステージオブジェクト
	DetermineType(collisionA, CollisionTypes::stageObject, collisionB, CollisionTypes::player);
	DetermineType(collisionA, CollisionTypes::stageObject, collisionB, CollisionTypes::stage);//最初のワープゲートで使う
	//ボス
	DetermineType(collisionA, CollisionTypes::boss, collisionB, CollisionTypes::playerBullet);
	DetermineType(collisionA, CollisionTypes::boss, collisionB, CollisionTypes::parryBullet);
	//影
	DetermineType(collisionA,CollisionTypes::shadow,collisionB,CollisionTypes::stage);
}

void CollisionManager::DetermineType(CollisionSource& collisionA, const CollisionTypes& typeA, CollisionSource& collisionB, const CollisionTypes& typeB) {
	if (collisionA.GetType() == typeA && collisionB.GetType() == typeB) {
		collisionA.OnCollision(&collisionB);
		collisionB.OnCollision(&collisionA);
	}
}


void CollisionManager::UnderCollision(float& minUnder, const Vector3& actorPosition, const AABB& stageAABB) const {
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

void CollisionManager::GameActorAndStageCollision(CollisionOverlap& collisionOverlap, GameActor& gameActor, GravityActor& gravityActor, const AABB& otherCollisionAABB) {

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

void CollisionManager::BackPosition(CollisionOverlap& collisionOverlap) {

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


		//床 or 天井 (0以上は床、0未満は天井)
		if (push >= 0.0f) {
			// 着地判定を立てる
			collisionOverlap.isGround = true;
		}
		collisionOverlap.position.y += push;
	}
	//z軸はいらない

}

CollisionOverlap CollisionManager::SetTarget(const Vector3& position, const AABB& aabb) {
	CollisionOverlap result;

	result.position = position;//座標
	result.targetAABB = aabb;  //当たり判定AABB
	result.isGround = false;   //地面判定フラグ
	result.isWall = false;     //壁判定フラグ

	return result;
}
