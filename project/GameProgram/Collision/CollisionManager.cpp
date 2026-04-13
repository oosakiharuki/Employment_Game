#include "CollisionManager.h"
#include "MyMath.h"
#include "CheckPoint.h"
#include <Goal.h>
#include <WarpGate.h>
#include <NextStageSave.h>

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

void CollisionManager::FrameCollision(CollisionSource* addCollision) {
	collisions_.push_back(addCollision);//追加
}

void CollisionManager::FrameCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type) {
	//出し入れする
	if (collisionTemplate) {
		collisionTemplate.reset();//リセット
	}
	//継承じゃなくても作れる
	collisionTemplate = std::make_unique<CollisionSource>();
	collisionTemplate->SetAABB(collisionAABB);
	collisionTemplate->SetCenter(center);
	collisionTemplate->SetType(type);
	collisions_.push_back(&*collisionTemplate);//追加
}

void CollisionManager::FixedCollision(std::unique_ptr<CollisionSource> addCollision) {
	fixedCollisions_.push_back(std::move(addCollision));//追加
}

void CollisionManager::FixedCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type) {	
	if (isAlready_) {
		//新しくステージ全体の当たり判定を作るためリセット
		fixedCollisions_.clear();
		isAlready_ = false;
	}

	std::unique_ptr<CollisionSource> collision;
	collision = std::make_unique<CollisionSource>();

	collision->SetAABB(collisionAABB);
	collision->SetCenter(center);
	collision->SetType(type);

	fixedCollisions_.push_back(std::move(collision));//追加
}

void CollisionManager::CollisionUpdate() {

	isAlready_ = true;
	//「毎フレーム」と「一度のみ」を合わせる
	for (auto& fixedCollision : fixedCollisions_) {
		collisions_.push_back(&*fixedCollision);
	}
	//当たり判定を確認
	for (uint32_t i = 0; i < collisions_.size();i++) {
		for (uint32_t j = 0; j < collisions_.size(); j++) {
			//該当するタイプがない場合、iとjが同じ値ではしない(自分自身なので)
			if (!collisions_[i]->TypeCheckUp(collisions_[j]->GetType()) || i == j) {
				continue;
			}
			//当たり判定
			//AABB同士が触れたとき+タイプが同じでないとき
			if (IsCollisionAABB(collisions_[i]->GetAABB(), collisions_[j]->GetAABB())) {
				collisions_[i]->OnCollision(collisions_[j]);
			}
		}
	}
	//リセット
	collisions_.clear();
}
