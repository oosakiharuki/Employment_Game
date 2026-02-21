#include "EnemyEye.h"
#include "CollisionManager.h"
#include "ImGuiManager.h"

using namespace MyMath;

void EnemyEye::Update(const Vector3& enemyPos, const Vector3& foundReach) {

	collisionType_ = CollisionTypes::enemyEye;

	collisionAABB_.min = enemyPos - foundReach;
	collisionAABB_.max = enemyPos + foundReach;

	//リーチは向きによって変更される
	//プラスの時
	if (foundReach.x > 0.0f) {
		collisionAABB_.min.x = enemyPos.x;
		collisionAABB_.max.x = enemyPos.x + foundReach.x;
	}
	else {
		//マイナスの時
		collisionAABB_.min.x = enemyPos.x + foundReach.x;
		collisionAABB_.max.x = enemyPos.x;
	}

	center_ = Length(enemyPos / 2.0f, foundReach);

	CollisionManager::GetInstance().AddCollisions(this);

	enemyPosition_ = enemyPos;
	isFound_ = false;
	isWall_ = false;

#ifdef USE_IMGUI
	ImGui::Begin("EnemyEye");
	ImGui::Separator();
	ImGui::Text("enemy");
	ImGui::Text("Eye_Min : %f,%f,%f", collisionAABB_.min.x, collisionAABB_.min.y, collisionAABB_.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", collisionAABB_.max.x, collisionAABB_.max.y, collisionAABB_.max.z);
	ImGui::End();
#endif // USE_IMGUI
}

void EnemyEye::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == player && !isWall_) {
		isFound_ = true;
		playerPosition_ = collision->GetCenter();
	}

	//順番による
	if (collision->GetType() == stage) {

		Segment segment;
		segment.origin = enemyPosition_;      //敵座標
		segment.diff = playerPosition_; //プレイヤー座標

		if (IsCollisionAABB_Segment(collision->GetAABB(), segment)) {
			isFound_ = false;
			isWall_ = true;
		}	
	}
}
