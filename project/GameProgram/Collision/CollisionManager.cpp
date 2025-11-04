#include "CollisionManager.h"
#include "MyMath.h"

using namespace MyMath;

std::shared_ptr<CollisionManager> CollisionManager::instance = nullptr;

std::shared_ptr<CollisionManager> CollisionManager::GetInstance() {
	if (instance == nullptr) {
		instance = std::make_unique<CollisionManager>();
	}
	return instance;
}

void CollisionManager::AllCollisions(Player* player_, std::vector<std::shared_ptr<IEnemy>> enemies,
	std::list<std::shared_ptr<IStageObject>> stageObjects, std::vector<AABB> stagesAABB,
	std::vector<std::shared_ptr<EventTrigger>> eventTriggers, CameraControl* cameraControl_, Levelediter levelediter)
{


	for (auto& enemy : enemies) {

		for (auto& bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), enemy->GetAABB()) && !enemy->IsDead()) {
				enemy->IsDamage();
				bullet->IsHit();
			}
		}

		//弾丸
		for (EnemyBullet* bulletE : enemy->GetBullets()) {

			//傘の当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetUmbrella()->GetAABB()) && player_->GetIsShield()) {

				if (player_->GetIsPari()) {
					bulletE->Pari_Mode();
					player_->PariSuccess();
				}
				else {
					bulletE->IsHit();
					player_->KnockBackUmbrella(UmbrellaKnockBackPower, UmbrellaKnockBackTime);
				}
				player_->IsShildMosion();
			}

			//プレイヤーの当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetAABB()) && !player_->GetIsPlayerDown()) {
				bulletE->IsHit();
				player_->IsDamage();
			}

			//跳ね返った弾の当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), enemy->GetAABB()) && bulletE->GetIsPari()) {
				bulletE->IsHit();
				enemy->IsDamage();
			}
		}

		//ダウンキャスト
		//親から子(基盤クラスから派生クラス)に変換し派生クラスの関数を使えることができる
		//if(enemyが<派生クラス>と同じ) = true
		if (enemy.get() == dynamic_cast<Enemy_Bomb*>(enemy.get())) {
			Enemy_Bomb* enemy_Bomb = dynamic_cast<Enemy_Bomb*>(enemy.get());
			if (IsCollisionAABB(enemy_Bomb->GetBombAABB(), player_->GetAABB()) && 
				enemy_Bomb->IsDead() && !enemy_Bomb->IsExplosion()) {
				player_->IsDamage();
				player_->KnockBackPlayer(enemy_Bomb->GetDistance(), 0.8f);
			}
			//enemyから値を入れているためdeleteの必要はない
		}
	}

	///当たり判定

	CollisionOverlap playerCollisionOverlap;
	playerCollisionOverlap = SetTarget(player_->GetTranslate(),player_->GetAABB());

	//演出や死んだときは発動しない
	if (!player_->GetIsPlayerDown() && !player_->GetPerformanceMode()) {
		StageCollisions(&playerCollisionOverlap, stagesAABB);
	}

	//地面にいる判定(床の判定がtrueの場合)
	player_->IsGround(playerCollisionOverlap.isGround);

	//戻った場所を代入
	player_->SetTranslate(playerCollisionOverlap.position);


	for (auto& enemy : enemies) {
		CollisionOverlap enemyCollisionOverlap;
		enemyCollisionOverlap = SetTarget(enemy->GetTranslate(),enemy->GetAABB());

		StageCollisions(&enemyCollisionOverlap, stagesAABB);

		//地面にいる判定(床の判定がtrueの場合)
		enemy->IsGround(enemyCollisionOverlap.isGround);

		//戻った場所を代入
		enemy->SetTranslate(enemyCollisionOverlap.position);
	}


	for (auto& stage : stagesAABB) {
		for (auto& bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), stage)) {
				bullet->IsHit();
			}
		}

		for (auto& enemy : enemies) {
			for (EnemyBullet* bulletE : enemy->GetBullets()) {
				if (IsCollisionAABB(bulletE->GetAABB(), stage)) {
					bulletE->IsHit();
				}
			}
		}

	}

	playerCollisionOverlap = SetTarget(player_->GetTranslate(), player_->GetAABB());

	//イベントトリガー
	for (auto& eventTrigger : eventTriggers) {

		//イベントが終了した時(順番3)
		if (eventTrigger->EventEnd()) {
			//一瞬だけ通す
			if (cameraControl_->IsFixed()) {
				//カメラの最小/最大地点
				cameraControl_->FixedMode(false);
				cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit["MainCamera"], false);
			}
		}
		//イベントが発動している時(順番2)
		else if (eventTrigger->GetEventData().isEvent) {
			//イベントトリガーの範囲外に出ないように(!IsCollisionAABB()によって外に出た判定をとる)
			if (!IsCollisionAABB(playerCollisionOverlap.targetAABB, eventTrigger->GetEventData().aabb)) {
				//ステージ判定代入
				playerCollisionOverlap.stageAABB = eventTrigger->GetEventData().aabb;
				//重なった部分
				playerCollisionOverlap.overlap = OverAABB(player_->GetAABB(), eventTrigger->GetEventData().aabb);
				//場所を戻す・壁と床の判定
				BackPosition(&playerCollisionOverlap);

				//地面にいる判定(床の判定がtrueの場合)
				player_->IsGround(playerCollisionOverlap.isGround);

				//戻った場所を代入
				player_->SetTranslate(playerCollisionOverlap.position);

				//両方ともtrueの時
				if (playerCollisionOverlap.isWall && playerCollisionOverlap.isGround) {
					break;
				}
			}
		}
		//イベントトリガーに入った直前(順番1)
		else if (IsCollisionAABB(player_->GetAABB(), eventTrigger->GetEventData().aabb)) {
			cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit[eventTrigger->GetEventData().cameraName], true);
			eventTrigger->StartEvent();
		}
	}

	//影とステージの当たり判定
	Vector3 shadowPos = {};

	shadowPos = UnderCollision(stagesAABB, player_->GetShadowAABB(), player_->GetTranslate());

	player_->SetShadowPosition(shadowPos);
	player_->ShadowUpdate();

	for (auto& enemy : enemies) {
		shadowPos = UnderCollision(stagesAABB, enemy->GetShadowAABB(), enemy->GetTranslate());

		enemy->SetShadowPosition(shadowPos);
		enemy->ShadowUpdate();
	}
}



Vector3 CollisionManager::UnderCollision(std::vector<AABB> stageAABB, AABB shadowAABB, Vector3 position) {

	float minY = 1000.0f;
	float lengthMax = Length(position.y, minY);

	for (auto& stage : stageAABB) {

		if (IsCollisionAABB(shadowAABB, stage)) {
			float length = Length(position.y, stage.max.y);

			//プレイヤーと足場の長さが短いところを影に
			if (length < lengthMax) {
				lengthMax = length;
				minY = stage.max.y + 0.01f;
			}
		}
	}
	Vector3 result = position;
	result.y = minY;

	return result;
}

void CollisionManager::StageCollisions(CollisionOverlap* collisionOverlap , std::vector<AABB> stagesAABB) {

	//プレイヤーとステージ
	for (auto& stage : stagesAABB) {
		//当たり判定AABBが作動した時
		if (IsCollisionAABB(collisionOverlap->targetAABB, stage)) {

			//ステージ判定代入
			collisionOverlap->stageAABB = stage;
			//重なった部分
			collisionOverlap->overlap = OverAABB(collisionOverlap->targetAABB, stage);
			//場所を戻す・壁と床の判定
			BackPosition(collisionOverlap);

			//両方ともtrueの時
			if (collisionOverlap->isWall && collisionOverlap->isGround) {
				break;
			}
		}
	}
}


void CollisionManager::BackPosition(CollisionOverlap* collisionOverlap) {
	float half = 0.5f;//中心を求める用に使う

	// 重なりが一番小さい軸の押し戻しを行う	
	if (collisionOverlap->overlap.x < collisionOverlap->overlap.y) {
		//真ん中の座標を代入
		float targetCenterX = (collisionOverlap->targetAABB.min.x + collisionOverlap->targetAABB.max.x) * half;
		float areaCenterX = (collisionOverlap->stageAABB.min.x + collisionOverlap->stageAABB.max.x) * half;
		//真ん中から 右の場合 - / 左の場合 +
		float push = (targetCenterX < areaCenterX) ? -collisionOverlap->overlap.x : collisionOverlap->overlap.x;

		collisionOverlap->position.x += push;
		collisionOverlap->isWall = true;
	}
	else if (collisionOverlap->overlap.y < collisionOverlap->overlap.x) {
		// 真ん中の座標を代入
		float targetCenterY = (collisionOverlap->targetAABB.min.y + collisionOverlap->targetAABB.max.y) * half;
		float areaCenterY = (collisionOverlap->stageAABB.min.y + collisionOverlap->stageAABB.max.y) * half;
		//真ん中から 下の場合 - / 上の場合 +
		float push = (targetCenterY < areaCenterY) ? -collisionOverlap->overlap.y : collisionOverlap->overlap.y;


		//床 or 天井 (0以上は床、0未満は天井)
		if (push >= 0.0f) {
			// 着地判定を立てる
			collisionOverlap->isGround = true;
		}
		collisionOverlap->position.y += push;
	}
	//z軸はいらない

}

CollisionOverlap CollisionManager::SetTarget(Vector3 position, AABB aabb) {
	CollisionOverlap result;

	result.position = position;
	result.targetAABB = aabb;
	result.isGround = false;
	result.isWall = false;

	return result;
}
