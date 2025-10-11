#include "IScene.h"
using namespace MyMath;

int IScene::sceneNo = Title;

IScene::~IScene(){}

int IScene::GetSceneNo() { return sceneNo; }

void IScene::LevelEditorObjectSetting(const std::string leveleditor_file) {

	levelediter.LoadLevelediter(leveleditor_file);

	camera = std::make_unique<Camera>();

	cameraRotate = levelediter.GetLevelData()->cameraInit.rotation;
	cameraTranslate = levelediter.GetLevelData()->cameraInit.translation;

	//カメラの最小/最大地点
	cameraPoint1 = levelediter.GetLevelData()->cameraInit.Point1;
	cameraPoint2 = levelediter.GetLevelData()->cameraInit.Point2;

	camera->SetRotate(cameraRotate);
	camera->SetTranslate(cameraTranslate);

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

	if (!levelediter.GetLevelData()->objects.empty()) {
		for (auto& object : levelediter.GetLevelData()->objects) {

			Vector3 position = object.translation;

			AABB aabb;
			aabb.min = position + object.colliderAABB.min;
			aabb.max = position + object.colliderAABB.max;

			stagesAABB.push_back(aabb);
		}
	}

	//チェックポイント地点
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

	bool isWall = false;
	bool isGround = false;
	player_->IsGround(false);


	for (auto& stage : stagesAABB) {
		AABB playerAABB = player_->GetAABB();

		if (IsCollisionAABB(playerAABB, stage)) {

			Vector3 position = player_->GetTranslate();
			Vector3 overlap = OverAABB(player_->GetAABB(), stage);

			// 重なりが最小の軸で押し戻しを行う	
			if (overlap.x < overlap.y) {
				//真ん中の座標を代入
				float playerCenterX = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
				float obstacleCenterX = (stage.min.x + stage.max.x) * 0.5f;
				//真ん中から 右の場合 - / 左の場合 +
				float push = (playerCenterX < obstacleCenterX) ? -overlap.x : overlap.x;

				position.x += push;
				isWall = true;
			}
			else if (overlap.y < overlap.x) {
				// 真ん中の座標を代入
				float playerCenterY = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
				float obstacleCenterY = (stage.min.y + stage.max.y) * 0.5f;
				//真ん中から 右の場合 - / 左の場合 +
				float push = (playerCenterY < obstacleCenterY) ? -overlap.y : overlap.y;

				//床 or 天井 (0以上は床、0未満は天井)
				if (push >= 0.0f) {
					position.y += push;
					// 着地判定を立てる
					player_->IsGround(true);
					player_->GrabityZero();
				}
				else if (push < 0.0f) {
					position.y += push;
				}
				isGround = true;
			}
			//z軸はいらないかも

			player_->SetTranslate(position);

			//両方ともtrueの時
			if (isWall && isGround) {
				break;
			}
		}
	}


	isWall = false;
	isGround = false;

	for (auto& enemy : enemies) {
		enemy->IsGround(false);
		for (auto& stage : stagesAABB) {

			AABB enemyAABB = enemy->GetAABB();

			if (IsCollisionAABB(enemyAABB, stage)) {

				Vector3 position = enemy->GetTranslate();
				Vector3 overlap = OverAABB(enemy->GetAABB(), stage);

				// 重なりが最小の軸で押し戻しを行う	
				if (overlap.x < overlap.y) {
					//真ん中の座標を代入
					float enemyCenterX = (enemyAABB.min.x + enemyAABB.max.x) * 0.5f;
					float obstacleCenterX = (stage.min.x + stage.max.x) * 0.5f;
					//真ん中から 右の場合 - / 左の場合 +
					float push = (enemyCenterX < obstacleCenterX) ? -overlap.x : overlap.x;

					position.x += push;
					isWall = true;
				}
				else if (overlap.y < overlap.x) {
					// 真ん中の座標を代入
					float enemyCenterY = (enemyAABB.min.y + enemyAABB.max.y) * 0.5f;
					float obstacleCenterY = (stage.min.y + stage.max.y) * 0.5f;
					//真ん中から 右の場合 - / 左の場合 +
					float push = (enemyCenterY < obstacleCenterY) ? -overlap.y : overlap.y;

					//床 or 天井 (0以上は床、0未満は天井)
					if (push >= 0.0f) {
						position.y += push;
						// 着地判定を立てる
						enemy->IsGround(true);
						enemy->GrabityZero();
					}
					else if (push < 0.0f) {
						position.y += push;
					}
					isGround = true;
				}
				//z軸はいらないかも

				enemy->SetTranslate(position);

				//両方ともtrueの時
				if (isWall && isGround) {
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

		for (std::unique_ptr<IEnemy>& enemy : enemies) {
			for (EnemyBullet* bulletE : enemy->GetBullets()) {
				if (IsCollisionAABB(bulletE->GetAABB(), stage)) {
					bulletE->IsHit();
				}
			}
		}

	}

	Vector3 shadowPos = { 0,0,0 };

	shadowPos = ShadowCollision(stagesAABB, player_->GetShadowAABB(), player_->GetTranslate());

	player_->SetShadowPosition(shadowPos);
	player_->ShadowUpdate();

	for (auto& enemy : enemies) {
		shadowPos = ShadowCollision(stagesAABB, enemy->GetShadowAABB(), enemy->GetTranslate());

		enemy->SetShadowPosition(shadowPos);
		enemy->ShadowUpdate();
	}

	
}

Vector3 IScene::ShadowCollision(std::vector<AABB> stageAABB, AABB shadowAABB, Vector3 position) {
	
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