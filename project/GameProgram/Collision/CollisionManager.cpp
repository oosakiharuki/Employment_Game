#include "CollisionManager.h"
#include "MyMath.h"
#include "CheckPoint.h"
#include <Goal.h>
#include <WarpGate.h>
#include <NextStageSave.h>

using namespace MyMath;
using namespace UseEveryOne;

std::shared_ptr<CollisionManager> CollisionManager::sInstance = nullptr;

std::shared_ptr<CollisionManager> CollisionManager::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<CollisionManager>();
	}
	return sInstance;
}


void CollisionManager::PlayerAndEnemy(Player* player,
	const std::vector<std::shared_ptr<BaseEnemy>>& enemies) {
	// - プレイヤーと敵 -
	for (auto& enemy : enemies) {
		//プレイヤーを見つける範囲
		LookPlayer(player, enemy);

		//プレイヤーが敵の弾に当たったら
		PlayerBulletAndEnemy(player, enemy);

		for (auto& bulletE : enemy->GetBullets()) {
			//敵がプレイヤーの弾に当たったら
			EnemyBulletAndPlayer(player, bulletE);
			//敵が跳ね返った敵の弾に当たったら
			EnemyAndPariBullet(enemy, bulletE);
		}

		//ボムの敵 : 爆発範囲
		EnemyBombCollision(player, enemy);
	}
}

void CollisionManager::LookPlayer(Player* player, std::shared_ptr<BaseEnemy> enemy) {
	//見える範囲にプレイヤーがいたら
	if (IsCollisionAABB(player->GetAABB(), enemy->GetEyeAABB()) && !player->GetIsDead()) {
		enemy->SetFoundTarget(true);//見える
	}
	else {
		enemy->SetFoundTarget(false);//見えない
	}
}

void CollisionManager::PlayerBulletAndEnemy(Player* player, std::shared_ptr<BaseEnemy> enemy) {
	for (auto& bullet : player->GetBullets()) {
		if (IsCollisionAABB(bullet->GetAABB(), enemy->GetAABB()) && !enemy->GetIsDead()) {
			enemy->IsDamage();//敵にダメージ
			bullet->IsHit();//弾の消滅
		}
	}
}

void CollisionManager::EnemyBulletAndPlayer(Player* player, std::shared_ptr<EnemyBullet> bulletE) {
	//傘の当たり判定
	if (IsCollisionAABB(bulletE->GetAABB(), player->GetUmbrella()->GetAABB()) && player->GetIsShield()) {
		//パリィフラグが立ってるなら
		if (player->GetIsPari()) {
			bulletE->Pari_Mode();//弾が跳ね返る(敵に当たるとダメージ判定になる)
			player->PariSuccess();//パリィ成功
		}
		else {//跳ね返さず防ぐのみ
			bulletE->IsHit();//当たって消える
			player->KnockBackUmbrella(kUmbrellaKnockBackPower_, kUmbrellaKnockBackTime_);//ノックバックする
		}
		player->IsShildMosion();//傘のリアクションフラグをtrueに
	}

	//プレイヤーの当たり判定
	if (IsCollisionAABB(bulletE->GetAABB(), player->GetAABB()) && !player->GetIsDead()) {
		bulletE->IsHit();//当たって消える
		player->IsDamage(bulletE->GetDistance());//プレイヤーにダメージ
	}
}

void CollisionManager::EnemyBombCollision(Player* player, std::shared_ptr<BaseEnemy> enemy) {
	if (IsCollisionAABB(enemy->GetBombAABB(), player->GetAABB()) &&
		enemy->GetIsDead() && !enemy->IsExplosion()) {
		player->IsDamage(enemy->GetDistance());//プレイヤーにダメージ	
	}

	if (enemy->GetIsDead() && !enemy->IsExplosion()) {
		enemy->ExplosionEnd();
	}
}

void CollisionManager::EnemyAndPariBullet(std::shared_ptr<BaseEnemy> enemy, std::shared_ptr<EnemyBullet> bulletE) {
	//跳ね返った弾の当たり判定
	if (IsCollisionAABB(bulletE->GetAABB(), enemy->GetAABB()) && bulletE->GetIsPari() && !enemy->GetIsDead()) {
		bulletE->IsHit();//当たって消える
		enemy->IsDamage();//敵にダメージ
	}
}

void CollisionManager::PlayerAndStageObject(Player* player,
	const std::list<std::shared_ptr<IStageObject>>& stageObjects) {
	// - ステージオブジェクト -
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject->GetObjectName() == "Checkpoint") {
				player->SetInit_Position(stageObject->GetPosition(), player->GetRotate());
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
				player->IsGround(true);
				break;
			}
			else if (stageObject->GetObjectName() == "MoveGround") {
				player->IsGround(true);
			}
		}
	}
}

void CollisionManager::PlayerAndStage(Player* player, const std::vector<AABB>& stagesAABB) {
	// - プレイヤーとステージ -
	GameActorAndStageCollision(*player, stagesAABB);
	// - 弾とステージ -
	for (auto& stage : stagesAABB) {
		//プレイヤーの弾丸
		for (auto& bullet : player->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), stage)) {
				bullet->IsHit();//弾の消滅
			}
		}
	}

	// - 影とステージの当たり判定 -
	Vector3 shadowPos = {};
	shadowPos = UnderCollision(stagesAABB, player->GetShadowAABB(), player->GetTranslate());

	player->SetShadowPosition(shadowPos);
	player->ShadowUpdate();
}

void CollisionManager::EnemyAndStage(const std::vector<std::shared_ptr<BaseEnemy>>& enemies,
	const std::vector<AABB>& stagesAABB) {
	// - 敵とステージ -
	for (auto& enemy : enemies) {
		GameActorAndStageCollision(*enemy.get(), stagesAABB);
	}

	// - 弾とステージ -
	for (auto& stage : stagesAABB) {
		//敵の弾丸
		for (auto& enemy : enemies) {
			for (auto& bulletE : enemy->GetBullets()) {
				if (IsCollisionAABB(bulletE->GetAABB(), stage)) {
					bulletE->IsHit();//弾の消滅
				}
			}
		}
	}

	// - 影とステージの当たり判定 -
	Vector3 shadowPos = {};
	for (auto& enemy : enemies) {
		shadowPos = UnderCollision(stagesAABB, enemy->GetShadowAABB(), enemy->GetTranslate());

		enemy->SetShadowPosition(shadowPos);
		enemy->ShadowUpdate();
	}
}

void CollisionManager::BossAndPlayer(Player& player, Boss& boss) {
	//プレイヤーがボスに当たった時
	if (IsCollisionAABB(player.GetAABB(), boss.GetAABB()) && !boss.IsDeadMosion()) {
		player.IsDamage(Length(player.GetTranslate(),boss.GetTranslate()));
	}

	//
	for (auto& playerBullet : player.GetBullets()) {
		if (IsCollisionAABB(playerBullet->GetAABB(), boss.GetAABB())) {
			//ボスのダメージ
			playerBullet->IsHit();//消滅
			boss.IsDamage();
		}
	}

	//
	for (auto& bossBullet : boss.GetBullets()) {
		//敵がプレイヤーの弾に当たったら
		EnemyBulletAndPlayer(&player, bossBullet);

		//跳ね返った弾の当たり判定
		if (IsCollisionAABB(bossBullet->GetAABB(), boss.GetAABB()) && bossBullet->GetIsPari()) {
			bossBullet->IsHit();//当たって消える
			boss.IsDamage();
		}
	}
}


void CollisionManager::PlayerAndEventTrigger(Player* player, const std::vector<std::shared_ptr<EventTrigger>>& eventTriggers,
	CameraControl* cameraControl_, Levelediter& levelediter) {
	CollisionOverlap playerCollisionOverlap;
	playerCollisionOverlap = SetTarget(player->GetTranslate(), player->GetAABB());

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
			Vector3 move_range = player->GetTranslate();
			Vector3 size = player->GetScale();
			//動ける範囲制限
			move_range.x = std::clamp(move_range.x, data.aabb.min.x + size.x, data.aabb.max.x - size.x);
			move_range.y = std::clamp(move_range.y, data.aabb.min.y + size.y, data.aabb.max.y - size.y);
			move_range.z = std::clamp(move_range.z, data.aabb.min.z + size.z, data.aabb.max.z - size.z);
			player->SetTranslate(move_range);
		}
		//イベントトリガーに入った直前(順番1)
		else if (IsCollisionAABB(player->GetAABB(), data.aabb)) {
			cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit[data.cameraName], true);
			eventTrigger->StartEvent();
		}
	}
}

Vector3 CollisionManager::UnderCollision(const std::vector<AABB>& stageAABB, const AABB& shadowAABB, const Vector3& position) const {

	//できる限り下の値
	float underY = kMaxUnder;
	//プレイヤーとその値の距離(基準点)
	float lengthMin = Length(position.y, underY);

	for (auto& stage : stageAABB) {
		//影の範囲がステージ部分と衝突判定を取った時
		if (IsCollisionAABB(shadowAABB, stage)) {
			//プレイヤーとステージの上の長さ
			float length = Length(position.y, stage.max.y);

			//プレイヤーと足場の長さが一番短いところを影の場所とする
			if (length < lengthMin) {
				//値が変更
				lengthMin = length;
				underY = stage.max.y + kShadowUp_;
			}
		}
	}
	//x,z軸は現在のプレイヤー位置と同じ
	Vector3 result = position;
	//決定した影の位置を代入
	result.y = underY;

	return result;
}

void CollisionManager::GameActorAndStageCollision(GameActor& gameActor, const std::vector<AABB>& stagesAABB) {
	CollisionOverlap collisionOverlap;
	collisionOverlap = SetTarget(gameActor.GetTranslate(), gameActor.GetAABB());
	//演出や死んだときは発動しない
	if (!gameActor.GetIsDead() && !gameActor.GetPerformanceMode()) {
		StageCollisions(collisionOverlap, stagesAABB);
	}
	//地面にいる判定(床の判定がtrueの場合)
	gameActor.IsGround(collisionOverlap.isGround);
	//戻った場所を代入
	gameActor.SetTranslate(collisionOverlap.position);
}

void CollisionManager::StageCollisions(CollisionOverlap& collisionOverlap , const std::vector<AABB>& stagesAABB) {

	//プレイヤーとステージ
	for (auto& stage : stagesAABB) {
		//当たり判定AABBが作動した時
		if (IsCollisionAABB(collisionOverlap.targetAABB, stage)) {

			//ステージ判定代入
			collisionOverlap.stageAABB = stage;
			//重なった部分
			collisionOverlap.overlap = OverAABB(collisionOverlap.targetAABB, stage);
			//場所を戻す・壁と床の判定
			BackPosition(collisionOverlap);

			//両方ともtrueの時
			if (collisionOverlap.isWall && collisionOverlap.isGround) {
				break;
			}
		}
	}
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

void CollisionManager::ResetFlag() {
	//ゴールした時フラグ
	isGoal_ = false;
	//ワープで次のステージに進むフラグ
	isWarp_ = false;
}