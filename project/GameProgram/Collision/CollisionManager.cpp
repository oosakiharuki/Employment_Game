#include "CollisionManager.h"
#include "MyMath.h"
#include "CheckPoint.h"
#include <Goal.h>
#include <WarpGate.h>
#include <NextStageSave.h>

using namespace MyMath;

std::shared_ptr<CollisionManager> CollisionManager::sInstance = nullptr;

std::shared_ptr<CollisionManager> CollisionManager::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<CollisionManager>();
	}
	return sInstance;
}

void CollisionManager::AllCollisions(Player* player_, std::vector<std::shared_ptr<IEnemy>> enemies,
	std::list<std::shared_ptr<IStageObject>> stageObjects, std::vector<AABB> stagesAABB,
	std::vector<std::shared_ptr<EventTrigger>> eventTriggers, CameraControl* cameraControl_, Levelediter levelediter)
{
	// - プレイヤーと敵 -
	for (auto& enemy : enemies) {
		
		//見える範囲にプレイヤーがいたら
		if (IsCollisionAABB(player_->GetAABB(), enemy->GetEyeAABB()) && !player_->GetIsDead()) {
			enemy->IsFoundTarget(true);//見える
		}
		else {
			enemy->IsFoundTarget(false);//見えない
		}

		//プレイヤーが敵の弾に当たったら
		for (auto& bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), enemy->GetAABB()) && !enemy->GetIsDead()) {
				enemy->IsDamage();//敵にダメージ
				bullet->IsHit();//弾の消滅
			}
		}

		//弾丸
		for (auto* bulletE : enemy->GetBullets()) {

			//傘の当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetUmbrella()->GetAABB()) && player_->GetIsShield()) {

				//パリィフラグが立ってるなら
				if (player_->GetIsPari()) {
					bulletE->Pari_Mode();//弾が跳ね返る
					player_->PariSuccess();//パリィ成功
				}
				else {//跳ね返さず防ぐのみ
					bulletE->IsHit();//当たって消える
					player_->KnockBackUmbrella(kUmbrellaKnockBackPower_, kUmbrellaKnockBackTime_);//ノックバックする
				}
				player_->IsShildMosion();//傘のリアクションフラグをtrueに
			}

			//プレイヤーの当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetAABB()) && !player_->GetIsDead()) {
				bulletE->IsHit();//当たって消える
				player_->IsDamage(bulletE->GetDistance());//プレイヤーにダメージ
			}

			//跳ね返った弾の当たり判定
			if (IsCollisionAABB(bulletE->GetAABB(), enemy->GetAABB()) && bulletE->GetIsPari()) {
				bulletE->IsHit();//当たって消える
				enemy->IsDamage();//敵にダメージ
			}
		}

		//ボムの敵 : 爆発範囲
		if (IsCollisionAABB(enemy->GetBombAABB(), player_->GetAABB()) && 
			enemy->GetIsDead() && !enemy->IsExplosion()) {
			player_->IsDamage(enemy->GetDistance());//プレイヤーにダメージ	
		}
		
		if (enemy->GetIsDead() && !enemy->IsExplosion()) {
			enemy->ExplosionEnd();
		}

	}

	// - ステージの当たり判定 -
	//プレイヤーとステージ
	GameActorAndStageCollision(player_, stagesAABB);
	//敵とステージ
	for (auto& enemy : enemies) {
		GameActorAndStageCollision(enemy.get(), stagesAABB);
	}

	// - 弾とステージ -
	for (auto& stage : stagesAABB) {
		//プレイヤーの弾丸
		for (auto& bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), stage)) {
				bullet->IsHit();//弾の消滅
			}
		}

		//敵の弾丸
		for (auto& enemy : enemies) {
			for (EnemyBullet* bulletE : enemy->GetBullets()) {
				if (IsCollisionAABB(bulletE->GetAABB(), stage)) {
					bulletE->IsHit();//弾の消滅
				}
			}
		}
	}

	CollisionOverlap playerCollisionOverlap;
	playerCollisionOverlap = SetTarget(player_->GetTranslate(), player_->GetAABB());

	// - イベントトリガー -
	for (auto& eventTrigger : eventTriggers) {
		EventData data = eventTrigger->GetEventData();

		//イベントが終了した時(順番3)
		if (eventTrigger->EventEnd()) {
			//一瞬だけ通す
			if (cameraControl_->IsFixed()) {
				cameraControl_->FixedMode(false);//カメラを固定しない
				cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit["MainCamera"], false);//メインカメラに戻す
			}
		}
		//イベントが発動している時(順番2)
		else if (data.isEvent) {
			Vector3 move_range = player_->GetTranslate();
			Vector3 size = player_->GetSize();
			//動ける範囲制限
			move_range.x = std::clamp(move_range.x, data.aabb.min.x + size.x, data.aabb.max.x - size.x);
			move_range.y = std::clamp(move_range.y, data.aabb.min.y + size.y, data.aabb.max.y - size.y);
			move_range.z = std::clamp(move_range.z, data.aabb.min.z + size.z, data.aabb.max.z - size.z);
			player_->SetTranslate(move_range);
		}
		//イベントトリガーに入った直前(順番1)
		else if (IsCollisionAABB(player_->GetAABB(), data.aabb)) {
			cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit[data.cameraName], true);
			eventTrigger->StartEvent();
		}
	}

	// - ステージオブジェクト -
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject->GetObjectName() == "Checkpoint") {
				player_->SetInit_Position(stageObject->GetPosition(), player_->GetRotate());
			}
			//ゴール
			else if (stageObject->GetObjectName() == "Goal" && Input::GetInstance()->TriggerKey(DIK_E)) {
				isGoal_ = true;
			}
			//stageObjectsの中でワープゲートである場合
			else if (stageObject->GetObjectName() == "WarpGate" && Input::GetInstance()->TriggerKey(DIK_E)) {
				//プレイヤーとワープゲートの当たり判定 + Eキーを押した時
				isWarp_ = true;
				//次のステージに持ってくる情報
				NextStageSave::GetInstance()->SetNextStageFile(stageObject->GetNextStage());
				NextStageSave::GetInstance()->SetPlayerHp(player_->GetHp());
				player_->IsGround(true);
				break;
			}
		}
	}


	// - 影とステージの当たり判定 -
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
				minY = stage.max.y + kShadowUp_;
			}
		}
	}
	Vector3 result = position;
	result.y = minY;

	return result;
}

void CollisionManager::GameActorAndStageCollision(GameActor* gameActor, std::vector<AABB> stagesAABB) {
	CollisionOverlap collisionOverlap;
	collisionOverlap = SetTarget(gameActor->GetTranslate(), gameActor->GetAABB());
	//演出や死んだときは発動しない
	if (!gameActor->GetIsDead() && !gameActor->GetPerformanceMode()) {
		StageCollisions(&collisionOverlap, stagesAABB);
	}
	//地面にいる判定(床の判定がtrueの場合)
	gameActor->IsGround(collisionOverlap.isGround);
	//戻った場所を代入
	gameActor->SetTranslate(collisionOverlap.position);
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

void CollisionManager::ResetFlag() {
	//ゴールした時フラグ
	isGoal_ = false;
	//ワープで次のステージに進むフラグ
	isWarp_ = false;
}