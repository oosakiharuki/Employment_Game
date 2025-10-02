#include "GameScene.h"

using namespace MyMath;

void GameScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_0.json");

	stageobj = new Object3d();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_0.obj");

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	wt.Initialize();

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, 0.3f, true);


}

void GameScene::Update() {

	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();
		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			if (isGameClear) {
				//ゲームクリアシーンに移動
				sceneNo = Clear;
				Audio::GetInstance()->StopWave(BGMData_);
			}
			else if (isGameOver) {
				//ゲームオーバーシーンに移動
				sceneNo = GameOver;
				Audio::GetInstance()->StopWave(BGMData_);
			}
			else if (isNextStage) {
				StageMovement("resource/Levelediter/" + nextStage_fileName + ".json", nextStage_fileName + ".obj");
				player_->SpriteUpdate();
				isNextStage = false;
				zumuTimer = 0.0f;
			}
			isfadeStart = false;
		}
	}
	else {
		FadeScreen::GetInstance()->FedeOut();
	}

	if (isNextStage) {
		if (zumuTimer <= 1.0f) {
			zumuTimer += 1.0f / 60.0f;

		}
		camera->Zumu(cameraSegment, zumuTimer);

		if (zumuTimer >= 1.0f) {
			isfadeStart = true;
		}
	}

	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//ワープゲート
			if (stageObject == dynamic_cast<WarpGate*>(stageObject) && Input::GetInstance()->TriggerKey(DIK_E)) {
				WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject);
				cameraSegment.origin = camera->GetTranslate();//ズーム前のカメラ位置
				cameraSegment.diff = player_->GetTranslate() + Vector3(0, 2, -15.0f);//プレイヤーよりちょっと離れてる
				isNextStage = true;
				nextStage_fileName = warpGate->GetNextStage();
				break;
			}//ゴール
			else if (stageObject == dynamic_cast<Goal*>(stageObject)) {
				isGameClear = true;
				isfadeStart = true;
				return;
			}
		}
	}

	skyBox->Update(wt.matWorld_ * MakeScaleMatrix({ 1000,1000,1000 }));//大きくするため

	camera->Update();

	player_->Update();

	if (isNextStage) {
		player_->IsAnimationOnlyUpdate();
		return;
	}
	
	for (auto& enemy : enemies) {
		enemy->SetPlayer(player_);
		enemy->Update();
	}

	CollisionCommon();

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}

	bool isChangeRespown = false;
	//リスポーン変更した時
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject == dynamic_cast<CheckPoint*>(stageObject)) {
				CheckPoint* checkPoint = dynamic_cast<CheckPoint*>(stageObject);
				player_->SetRespownPosition(checkPoint->GetPosition());
				isChangeRespown = true;
			}
		}
	}

	//プレイヤーが死んで、リスポーン地点が変更していないとき敵は復活する
	if (player_->GetIsPlayerDown() && player_->GetIsRespown()) {

		RemainingLife--;
		if (RemainingLife != 0) {
			for (auto& enemy : enemies) {
				enemy->RespownEnemy();
			}
			player_->AllRespownEnd();
		}
		else {
			isGameOver = true;
			isfadeStart = true;
		}
	}
	
	//
	if (Input::GetInstance()->TriggerKey(DIK_F2)) {
		sceneNo = Clear;
		Audio::GetInstance()->StopWave(BGMData_);
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

	//次ステージ移動時はズームされるのでここは除外
	if (!isNextStage) {
		if (cameraTranslate.x + cameraPoint1.x < player_->GetTranslate().x && cameraTranslate.x + cameraPoint2.x > player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = player_->GetTranslate().x;
		}
		else if (cameraTranslate.x + cameraPoint1.x >= player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint1.x;
		}
		else if (cameraTranslate.x + cameraPoint2.x <= player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint2.x;
		}

		if (cameraTranslate.y < player_->GetTranslate().y + 6.0f) {
			worldTransformCamera_.translation_.y = player_->GetTranslate().y + 6.0f;
		}
		else {
			worldTransformCamera_.translation_.y = cameraTranslate.y;
		}

		camera->SetTranslate(worldTransformCamera_.translation_);
	}



	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}

	stageobj->Update(wt);

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
	
	stageobj->Draw();

	player_->Draw();
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Draw();
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
	enemies.clear();
	delete stageobj;
	delete skyBox;

	for (auto& stageObject : stageObjects) {
		delete stageObject;
	}
	stageObjects.clear();

	stagesAABB.clear();
}

void GameScene::StageMovement(const std::string leveleditor_file, const std::string stageObj) {
	playerHp = player_->GetHp();
	Finalize();
	levelediter.ResetData();
	//パーティクルナンバー初期化
	ParticleNum::number = 0;

	LevelEditorObjectSetting(leveleditor_file);	
	
	player_->SetHp(playerHp);

	stageobj = new Object3d();
	stageobj->Initialize();
	stageobj->SetModelFile(stageObj);

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");
}

