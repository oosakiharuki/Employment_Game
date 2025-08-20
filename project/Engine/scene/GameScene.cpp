#include "GameScene.h"

using namespace MyMath;

void GameScene::Initialize() {

	modelManager = ModelManager::GetInstance();
	modelManager->LoadModel("cannon", ".obj");
	modelManager->LoadModel("playerHead", ".obj");
	modelManager->LoadModel("enemy", ".obj");
	modelManager->LoadModel("stage_proto", ".obj");
	modelManager->LoadModel("PlayerBullet", ".obj");
	modelManager->LoadModel("umbrella_Open", ".obj");
	modelManager->LoadModel("umbrella_Close", ".obj");
	modelManager->LoadModel("checkpoint", ".obj");
	modelManager->LoadModel("sphere", ".obj");
	modelManager->LoadModel("stage_0", ".obj");
	
	//glft
	modelManager->LoadModel("walk", ".gltf");
	modelManager->LoadModel("sneakWalk", ".gltf");
	modelManager->LoadModel("stop", ".gltf");



	camera = new Camera();

	//levelediter = new Levelediter();
	levelediter.LoadLevelediter("resource/Levelediter/stage_0.json");

	cameraRotate = levelediter.GetLevelData()->cameraInit.rotation;
	cameraTranslate = levelediter.GetLevelData()->cameraInit.translation;

	cameraPoint1 = levelediter.GetLevelData()->cameraInit.Point1;
	cameraPoint2 = levelediter.GetLevelData()->cameraInit.Point2;

	camera->SetRotate(cameraRotate);
	camera->SetTranslate(cameraTranslate);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera);
	GLTFCommon::GetInstance()->SetDefaultCamera(camera);
	ParticleCommon::GetInstance()->SetDefaultCamera(camera);
	DebugWireframes::GetInstance()->SetDefaultCamera(camera);
	Cubemap::GetInstance()->SetDefaultCamera(camera);

	player_ = new Player();
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

			IEnemy* enemy;
			if (enemyData.fileName == "Turret") {
				enemy = new Enemy_Turret();
			}else if (enemyData.fileName == "Bomb") {
				enemy = new Enemy_Bomb();
			}
			else {
				enemy = new Enemy_Soldier();
			}
			
			
			enemy->Initialize();
			
			enemy->SetTranslate(enemyData.translation);
			enemy->SetRotate(enemyData.rotation);
			enemy->SetInit_Position(enemyData.translation, enemyData.rotation);

			enemy->SetAABB(enemyData.colliderAABB);
			enemy->SetRoutePoint1(enemyData.Point1);
			enemy->SetRoutePoint2(enemyData.Point2);

			enemy->DirectionDegree();

			enemies.push_back(enemy);
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
	if (!levelediter.GetLevelData()->objects.empty()) {
		for (auto& checkPointData : levelediter.GetLevelData()->checkpoints) {
			CheckPoint* checkPoint = new CheckPoint();
			checkPoint->Initialize();
			checkPoint->SetPosition(checkPointData.translation);
			checkPoint->SetAABB(checkPointData.colliderAABB);
			checkPoints.push_back(checkPoint);
		}
	}


	worldTransformCamera_.Initialize();
	worldTransformCamera_.translation_ = cameraTranslate;
	worldTransformCamera_.rotation_ = cameraRotate;

	//camera->SetParent(&worldTransformCamera_);

	stageobj = new Object3d();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_0.obj");

	wt.Initialize();

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, 0.3f, true);

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	for (auto& enemy : enemies) {
		enemy->SetStages(stagesAABB);
	}

}

void GameScene::Update() {


	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		Audio::GetInstance()->SoundPlayWave(soundData_, 0.3f, false);
	}

	if (Input::GetInstance()->PushKey(DIK_0)) {
		OutputDebugStringA("Hit 0\n");
	}

	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		OutputDebugStringA("Hit 1\n");
	}

	//今回はしない
	//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	//	sceneNo = Title;
	//}
	skyBox->Update(wt.matWorld_ * MakeScaleMatrix({ 1000,1000,1000 }));//大きくするため

	camera->Update();

	player_->Update();

	for (auto& enemy : enemies) {
		enemy->SetPlayer(player_);
		enemy->Update();

		for (PlayerBullet* bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), enemy->GetAABB()) && !enemy->IsDead()) {
				enemy->IsDamage();
				bullet->IsHit();
			}
		}

		for (EnemyBullet* bulletE : enemy->GetBullets()) {

			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetUmbrella()->GetAABB()) && player_->GetIsShield()) {
				bulletE->IsHit();
				player_->KnockBackUmbrella(Vector3(0.0f, 0.0f, 0.3f), 0.0f);
			}

			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetAABB()) && !player_->GetIsPlayerDown()) {
				bulletE->IsHit();
				player_->IsDamage();
			}
		}

		//ダウンキャスト
		//親から子(基盤クラスから派生クラス)に変換し派生クラスの関数を使えることができる
		//if(enemyが<派生クラス>と同じ) = true
		if (enemy == dynamic_cast<Enemy_Bomb*>(enemy)) {
			Enemy_Bomb* enemy_Bomb = dynamic_cast<Enemy_Bomb*>(enemy);
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
		for (PlayerBullet* bullet : player_->GetBullets()) {
			if (IsCollisionAABB(bullet->GetAABB(), stage)) {
				bullet->IsHit();
			}
		}

		for (IEnemy* enemy : enemies) {
			for (EnemyBullet* bulletE : enemy->GetBullets()) {
				if (IsCollisionAABB(bulletE->GetAABB(), stage)) {
					bulletE->IsHit();
				}
			}
		}

	}


	bool isChangeRespown = false;
	//リスポーン変更した時
	for (auto& checkPoint : checkPoints) {
		checkPoint->Update();

		if (IsCollisionAABB(player_->GetAABB(), checkPoint->GetAABB())) {
			player_->SetRespownPosition(checkPoint->GetPosition());
			isChangeRespown = true;
		}
	}

	//プレイヤーが死んで、リスポーン地点が変更していないとき敵は復活する
	if (player_->GetIsPlayerDown() && player_->GetIsRespown()) {
		for (auto& enemy : enemies) {
			enemy->RespownEnemy();
		}
		player_->AllRespownEnd();
	}
	
	//リスポーン地点を変更前に倒した敵は復活しない
	if (isChangeRespown) {
		//敵を倒したら削除
		enemies.remove_if([](IEnemy* enemy) {
			if (enemy->GetDeleteEnemy()) {
				delete enemy;
				return true;
			}
			return false;
			});
		isChangeRespown = false;
	}

	//カメラの移動範囲
	if (cameraTranslate.x + cameraPoint1.x < player_->GetTranslate().x && cameraTranslate.x + cameraPoint2.x > player_->GetTranslate().x) {
		worldTransformCamera_.translation_.x = player_->GetTranslate().x;
		camera->SetTranslate(worldTransformCamera_.translation_);
	}
	else if(cameraTranslate.x + cameraPoint1.x >= player_->GetTranslate().x){
		worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint1.x;
		camera->SetTranslate(worldTransformCamera_.translation_);
	}
	else if (cameraTranslate.x + cameraPoint2.x <= player_->GetTranslate().x) {
		worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint2.x;
		camera->SetTranslate(worldTransformCamera_.translation_);
	}

	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}


	wt.UpdateMatrix();
	worldTransformCamera_.UpdateMatrix();


#ifdef  USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &worldTransformCamera_.translation_.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &worldTransformCamera_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &worldTransformCamera_.rotation_.x);
	ImGui::SliderFloat("cameraRotateX", &worldTransformCamera_.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &worldTransformCamera_.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &worldTransformCamera_.rotation_.z, -360.0f, 360.0f);

	ImGui::Text("p1 : %f %f %f", cameraPoint1.x, cameraPoint1.y, cameraPoint1.z);
	ImGui::Text("p2 : %f %f %f", cameraPoint2.x, cameraPoint2.y, cameraPoint2.z);

	camera->SetRotate(worldTransformCamera_.rotation_);
	camera->SetTranslate(worldTransformCamera_.translation_);

	ImGui::SliderFloat("volume", &volume, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI


	Audio::GetInstance()->ControlVolume(BGMData_, volume);
}

void GameScene::Draw() {
	
	Cubemap::GetInstance()->Command();
	skyBox->Draw();


	//モデル描画処理
	GLTFCommon::GetInstance()->Command();

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();
	
	stageobj->Draw(wt);

	player_->Draw();
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	for (auto& checkPoint : checkPoints) {
		checkPoint->Draw();
	}
	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();

}

void GameScene::Finalize() {
	delete camera;
	delete player_;
	for (auto& enemy : enemies) {
		delete enemy;
	}
	delete stageobj;
	delete skyBox;

	for (auto& checkPoint : checkPoints) {
		delete checkPoint;
	}
}
