#include "GameScene.h"

using namespace MyMath;

void GameScene::Initialize() {

	modelManager = ModelManager::GetInstance();
	modelManager->LoadModel("cannon", ".obj");
	modelManager->LoadModel("playerHead", ".obj");
	modelManager->LoadModel("enemy", ".obj");
	modelManager->LoadModel("stage_proto", ".obj");
	modelManager->LoadModel("sneakWalk", ".gltf");
	modelManager->LoadModel("PlayerBullet", ".obj");
	modelManager->LoadModel("umbrella_Open", ".obj");
	modelManager->LoadModel("umbrella_Close", ".obj");

	camera = new Camera();

	//levelediter = new Levelediter();
	levelediter.LoadLevelediter("resource/Levelediter/myEngine_stageProto.json");

	cameraRotate = levelediter.GetLevelData()->cameraInit.rotation;
	cameraTranslate = levelediter.GetLevelData()->cameraInit.translation;

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
	}


	if (!levelediter.GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelediter.GetLevelData()->spawnEnemies) {

			IEnemy* enemy;
			if (enemyData.fileName == "Turret") {
				enemy = new Enemy_Turret();
			}
			else {
				enemy = new Enemy();
			}
			
			
			enemy->Initialize();
			enemy->SetTranslate(enemyData.translation);
			enemy->SetRotate(enemyData.rotation);
			enemy->SetAABB(enemyData.colliderAABB);
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

	worldTransformCamera_.Initialize();
	camera->SetParent(&worldTransformCamera_);

	stageobj = new Object3d();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_proto.obj");

	wt.Initialize();

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, 0.3f, true);

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	gltfOBJ = new Object_glTF();
	gltfOBJ->Initialize();
	gltfOBJ->SetModelFile("sneakWalk.gltf");
	gltfOBJ->SetEnvironment("resource/rostock_laage_airport_4k.dds");
}

void GameScene::Update() {
	
	
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		Audio::GetInstance()->SoundPlayWave(soundData_, 0.3f,false);
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

	stageobj->Update();

	for (auto& enemy : enemies) {
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
				player_->KnockBackPlayer(Vector3(0.0f,0.0f,0.3f),0.0f);
			}

			if (IsCollisionAABB(bulletE->GetAABB(), player_->GetAABB())) {
				bulletE->IsHit();
				player_->IsDamage();
			}
		}
	}

	for (auto& stage : stagesAABB) {
		AABB playerAABB = player_->GetAABB();

		if (IsCollisionAABB(playerAABB, stage)) {

			Vector3 position = player_->GetTranslate();
			Vector3 overlap = OverAABB(player_->GetAABB(), stage);

			// 重なりが最小の軸で押し戻しを行う
			if (overlap.x <= overlap.y && overlap.x < overlap.z) {
				//真ん中の座標を代入
				float playerCenterX = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
				float obstacleCenterX = (stage.min.x + stage.max.x) * 0.5f;
				//真ん中から 右の場合 - / 左の場合 +
				float push = (playerCenterX < obstacleCenterX) ? -overlap.x : overlap.x;

				position.x += push;
			}
			else if (overlap.y < overlap.x && overlap.y < overlap.z) {
				// 真ん中の座標を代入
				float playerCenterY = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
				float obstacleCenterY = (stage.min.y + stage.max.y) * 0.5f;
				//真ん中から 右の場合 - / 左の場合 +
				float push = (playerCenterY < obstacleCenterY) ? -overlap.y : overlap.y;

				position.y += push + 0.00001f;//少し浮かせるため
				// 上向きの押し戻しなら着地判定を立てる
				player_->IsGround(true);
			}
			else if (overlap.z < overlap.x && overlap.z < overlap.y) {
				//真ん中の座標を代入
				float playerCenterZ = (playerAABB.min.z + playerAABB.max.z) * 0.5f;
				float obstacleCenterZ = (stage.min.z + stage.max.z) * 0.5f;
				//真ん中から 右の場合 - / 左の場合 +
				float push = (playerCenterZ < obstacleCenterZ) ? -overlap.z : overlap.z;

				position.z -= overlap.z;
			}

			player_->SetTranslate(position);

			break;
		}
		else {
			player_->IsGround(false);
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




	

	
	gltfOBJ->Update(wt);


	worldTransformCamera_.translation_ = player_->GetTranslate();

	wt.UpdateMatrix();
	worldTransformCamera_.UpdateMatrix();


#ifdef  USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &cameraTranslate.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &cameraTranslate.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &cameraRotate.x);
	ImGui::SliderFloat("cameraRotateX", &cameraRotate.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &cameraRotate.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &cameraRotate.z, -360.0f, 360.0f);
	camera->SetRotate(cameraRotate);
	camera->SetTranslate(cameraTranslate);

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
	
	gltfOBJ->Draw();

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();
	
	stageobj->Draw(wt);

	player_->Draw();
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();


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
	delete gltfOBJ;
}
