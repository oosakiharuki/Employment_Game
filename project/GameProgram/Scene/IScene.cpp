#include "IScene.h"
using namespace MyMath;

std::string IScene::sceneNo = "Game";

std::string IScene::nextSceneNo = "Game";

IScene::~IScene(){}

std::string IScene::GetSceneNo() { return sceneNo; }

void IScene::InputGamePad() {
	input_->GetJoyStickState(0,state);
	input_->GetJoystickStatePrevious(0, preState);
}

void IScene::LevelEditorObjectSetting(const std::string leveleditor_file) {

	levelediter.LoadLevelediter(leveleditor_file);

	camera = std::make_unique<Camera>();

	MainCamera();

	Object3dCommon::GetInstance()->SetDefaultCamera(camera.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera.get());

	player_ = std::make_unique<Player>();
	player_->Initialize();

	//プレイヤー配置データがあるときプレイヤーを配置
	if (!levelediter.GetLevelData()->players.empty()) {
		auto& playerData = levelediter.GetLevelData()->players[0];
		player_->SetTranslate(playerData.translation);
		player_->SetRotate(playerData.rotation);
		player_->SetAABB(playerData.colliderAABB);

		player_->SetRespownPosition(playerData.translation);
	}


	if (!levelediter.GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelediter.GetLevelData()->spawnEnemies) {

			std::unique_ptr<IEnemy> enemy;
			if (enemyData.EnemyName == "Turret") {
				enemy = std::make_unique<Enemy_Turret>();
			}
			else if (enemyData.EnemyName == "Bomb") {
				enemy = std::make_unique<Enemy_Bomb>();
			}
			else {
				enemy = std::make_unique<Enemy_Soldier>();
			}


			enemy->Initialize();

			enemy->SetTranslate(enemyData.translation);
			enemy->SetRotate(enemyData.rotation);
			enemy->SetInit_Position(enemyData.translation, enemyData.rotation);

			enemy->SetAABB(enemyData.colliderAABB);
			enemy->SetRoutePoint1(enemyData.Point1);
			enemy->SetRoutePoint2(enemyData.Point2);

			enemy->DirectionDegree();

			enemies.push_back(std::move(enemy));
		}
	}

	//
	if (!levelediter.GetLevelData()->eventTriggerDatas.empty()) {
		for (auto& eventTriggerData : levelediter.GetLevelData()->eventTriggerDatas) {
			EventData iterator;
			iterator.aabb = eventTriggerData.collisionAABB;
			iterator.center = eventTriggerData.center;
			iterator.csvFile = eventTriggerData.csvFile;

			std::unique_ptr<EventTrigger> eventTrigger;
			eventTrigger = std::make_unique<EventTrigger>();
			eventTrigger->SetEventData(iterator);

			eventTriggers.push_back(std::move(eventTrigger));
		}
	}

	if (!levelediter.GetLevelData()->objects.empty()) {
		for (auto& object : levelediter.GetLevelData()->objects) {

			Vector3 position = object.translation;

			AABB aabb;
			aabb.min = position + object.colliderAABB.min;
			aabb.max = position + object.colliderAABB.max;

			stagesAABB.push_back(aabb);
		}
	}

	//ステージオブジェクト地点
	if (!levelediter.GetLevelData()->stageObjects.empty()) {
		for (auto& stageObjectData : levelediter.GetLevelData()->stageObjects) {
			std::unique_ptr<IStageObject> stageObject;
			
			if (stageObjectData.ObjectName == "WarpGate") {
				stageObject = std::make_unique<WarpGate>();
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				stageObject = std::make_unique<CheckPoint>();
			}
			else if (stageObjectData.ObjectName == "Goal") {
				stageObject = std::make_unique<Goal>();
			}
			stageObject->Initialize();
			stageObject->SetPosition(stageObjectData.translation);
			stageObject->SetAABB(stageObjectData.colliderAABB);

			if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get())) {
				WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
				warpGate->SetNextStage(stageObjectData.fileName);
			}

			stageObjects.push_back(std::move(stageObject));
		}
	}

	worldTransformCamera_.Initialize();
	worldTransformCamera_.translation_ = cameraTranslate;
	worldTransformCamera_.rotation_ = cameraRotate;

	for (auto& enemy : enemies) {
		enemy->SetStages(stagesAABB);
	}

}

void IScene::CollisionCommon() {
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
					player_->KnockBackUmbrella(Vector3(0.0f, 0.0f, 0.3f), 0.0f);
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
			if (IsCollisionAABB(enemy_Bomb->GetBombAABB(), player_->GetAABB()) && !enemy->IsDead()) {
				player_->IsDamage();
				player_->KnockBackPlayer(enemy_Bomb->GetDistance(), 0.8f);
			}
			//enemyから値を入れているためdeleteの必要はない
		}
	}

	///当たり判定

	player_->IsGround(false);

	CollisionOverlap playerCollisionOverlap;
	playerCollisionOverlap.targetAABB = player_->GetAABB();
	playerCollisionOverlap.position = player_->GetTranslate();

	//プレイヤーとステージ
	for (auto& stage : stagesAABB) {
		//当たり判定AABBが作動した時(プレイヤーがやられていない)
		if (IsCollisionAABB(playerCollisionOverlap.targetAABB, stage) &&
			!player_->GetIsPlayerDown() && !player_->GetPerformanceMode()) {

			//ステージ判定代入
			playerCollisionOverlap.stageAABB = stage;
			//重なった部分
			playerCollisionOverlap.overlap = OverAABB(player_->GetAABB(), stage);
			//場所を戻す・壁と床の判定
			playerCollisionOverlap = BackPosition(playerCollisionOverlap);

			//床の判定がtrueの場合
			if (playerCollisionOverlap.isGround) {
				//地面にいる判定
				player_->IsGround(true);
			}

			//戻った場所を代入
			player_->SetTranslate(playerCollisionOverlap.position);

			//両方ともtrueの時
			if (playerCollisionOverlap.isWall && playerCollisionOverlap.isGround) {
				break;
			}
		}
	}


	for (auto& enemy : enemies) {	
		enemy->IsGround(false);

		CollisionOverlap enemyCollisionOverlap;
		enemyCollisionOverlap.targetAABB = enemy->GetAABB();
		enemyCollisionOverlap.position = enemy->GetTranslate();

		for (auto& stage : stagesAABB) {

			if (IsCollisionAABB(enemyCollisionOverlap.targetAABB, stage)) {

				enemyCollisionOverlap.stageAABB = stage;
				enemyCollisionOverlap.overlap = OverAABB(enemyCollisionOverlap.targetAABB, stage);

				enemyCollisionOverlap = BackPosition(enemyCollisionOverlap);

				enemy->SetTranslate(enemyCollisionOverlap.position);

				if (enemyCollisionOverlap.isGround) {
					enemy->IsGround(true);
					enemy->GrabityZero();
				}

				//両方ともtrueの時
				if (enemyCollisionOverlap.isWall && enemyCollisionOverlap.isGround) {
					break;
				}
			}
		}
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


	playerCollisionOverlap.targetAABB = player_->GetAABB();
	playerCollisionOverlap.position = player_->GetTranslate();

	//イベントトリガー
	for (auto& eventTrigger : eventTriggers) {
		//イベントが発動した時
		if (eventTrigger->GetEventData().isEvent) {
			//イベントトリガーの範囲外に出ないように(!IsCollisionAABB()によって外に出た判定をとる)
			if (!IsCollisionAABB(playerCollisionOverlap.targetAABB, eventTrigger->GetEventData().aabb)) {
				//ステージ判定代入
				playerCollisionOverlap.stageAABB = eventTrigger->GetEventData().aabb;
				//重なった部分
				playerCollisionOverlap.overlap = OverAABB(player_->GetAABB(), eventTrigger->GetEventData().aabb);
				//場所を戻す・壁と床の判定
				playerCollisionOverlap = BackPosition(playerCollisionOverlap);

				if (playerCollisionOverlap.isGround) {
					player_->IsGround(true);
				}

				//戻った場所を代入
				player_->SetTranslate(playerCollisionOverlap.position);

				//両方ともtrueの時
				if (playerCollisionOverlap.isWall && playerCollisionOverlap.isGround) {
					break;
				}
			}
		}
		else if (IsCollisionAABB(player_->GetAABB(), eventTrigger->GetEventData().aabb) && !eventTrigger->EventEnd()) {
			cameraRotate = levelediter.GetLevelData()->cameraInit[1].rotation;
			cameraTranslate = levelediter.GetLevelData()->cameraInit[1].translation;

			//カメラの最小/最大地点
			cameraPoint1 = levelediter.GetLevelData()->cameraInit[1].Point1;
			cameraPoint2 = levelediter.GetLevelData()->cameraInit[1].Point2;


			camera->SetRotate(cameraRotate);
			camera->SetTranslate(cameraTranslate);

			eventTrigger->StartEvent();
		}

		if (eventTrigger->EventEnd()) {
			MainCamera();
		}

	}




	Vector3 shadowPos = { 0,0,0 };

	shadowPos = UnderCollision(stagesAABB, player_->GetShadowAABB(), player_->GetTranslate());

	player_->SetShadowPosition(shadowPos);
	player_->ShadowUpdate();

	for (auto& enemy : enemies) {
		shadowPos = UnderCollision(stagesAABB, enemy->GetShadowAABB(), enemy->GetTranslate());

		enemy->SetShadowPosition(shadowPos);
		enemy->ShadowUpdate();
	}

	
}

Vector3 IScene::UnderCollision(std::vector<AABB> stageAABB, AABB shadowAABB, Vector3 position) {
	
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

void IScene::MainCamera() {
	//座標と回転
	cameraRotate = levelediter.GetLevelData()->cameraInit[0].rotation;
	cameraTranslate = levelediter.GetLevelData()->cameraInit[0].translation;

	//カメラの最小/最大地点
	cameraPoint1 = levelediter.GetLevelData()->cameraInit[0].Point1;
	cameraPoint2 = levelediter.GetLevelData()->cameraInit[0].Point2;

	//導入
	camera->SetRotate(cameraRotate);
	camera->SetTranslate(cameraTranslate);

}

CollisionOverlap IScene::BackPosition(CollisionOverlap collisionOverlap) {

	CollisionOverlap collisionOverlap_ = collisionOverlap;

	// 重なりが一番小さい軸の押し戻しを行う	
	if (collisionOverlap_.overlap.x < collisionOverlap_.overlap.y) {
		//真ん中の座標を代入
		float targetCenterX = (collisionOverlap_.targetAABB.min.x + collisionOverlap_.targetAABB.max.x) * 0.5f;
		float areaCenterX = (collisionOverlap_.stageAABB.min.x + collisionOverlap_.stageAABB.max.x) * 0.5f;
		//真ん中から 右の場合 - / 左の場合 +
		float push = (targetCenterX < areaCenterX) ? -collisionOverlap_.overlap.x : collisionOverlap_.overlap.x;

		collisionOverlap_.position.x += push;
		collisionOverlap_.isWall = true;
	}
	else if (collisionOverlap_.overlap.y < collisionOverlap_.overlap.x) {
		// 真ん中の座標を代入
		float targetCenterY = (collisionOverlap_.targetAABB.min.y + collisionOverlap_.targetAABB.max.y) * 0.5f;
		float areaCenterY = (collisionOverlap_.stageAABB.min.y + collisionOverlap_.stageAABB.max.y) * 0.5f;
		//真ん中から 右の場合 - / 左の場合 +
		float push = (targetCenterY < areaCenterY) ? -collisionOverlap_.overlap.y : collisionOverlap_.overlap.y;

		//床 or 天井 (0以上は床、0未満は天井)
		if (push >= 0.0f) {
			collisionOverlap_.position.y += push;
			// 着地判定を立てる
			collisionOverlap_.isGround = true;
		}
		else if (push < 0.0f) {
			collisionOverlap_.position.y += push;
		}
	}
	//z軸はいらないかも

	return collisionOverlap_;
}

void IScene::NextSceneFadeInStart(const std::string& name) {
	FadeScreen::GetInstance()->FadeStart(type_fadeIn);
	nextSceneNo = name;
}

bool IScene::NextSceneFlag() {
	if (sceneNo != nextSceneNo) {
		return true;
	}
	return false;
}

void IScene::ChangeScene() {
	sceneNo = nextSceneNo;
}