#include "GameScene.h"
#include <sstream>
using namespace MyMath;

void GameScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_0.json");

	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_0.obj");

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, 0.3f, true);

	WarterWarpExit();

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void GameScene::Update() {

	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
		Audio::GetInstance()->StopWave(BGMData_);
	}

	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		NextSceneFadeInStart("Title");
	}

	startWarp->Update();

	if (isNextStage) {
		if (zumuTimer <= 1.0f) {
			zumuTimer += 1.0f / 60.0f;

		}
		camera->Zumu(cameraSegment, zumuTimer);

		if (zumuTimer >= 1.0f) {
			NextSceneFadeInStart("NextStage");
		}
	}

	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//ワープゲート
			if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get()) && Input::GetInstance()->TriggerKey(DIK_E)) {
				WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
				cameraSegment.origin = camera->GetTranslate();//ズーム前のカメラ位置
				cameraSegment.diff = player_->GetTranslate() + Vector3(0, 2, -15.0f);//プレイヤーよりちょっと離れてる
				isNextStage = true;
				nextStage_fileName = warpGate->GetNextStage();
				break;
			}//ゴール
			else if (stageObject.get() == dynamic_cast<Goal*>(stageObject.get())) {
				NextSceneFadeInStart("Clear");
				return;
			}
		}
	}

	skyBox->Update(MakeScaleMatrix({ 1000,1000,1000 }));//大きくするため

	camera->Update();

	player_->Update();
	
	stageobj->Update();

	if (isStartStage) {
		//プレイヤー配置座標 + 地面当たり判定によって上げられる分
		if (startPointY >= playerPoint.y) {
			player_->SetTranslate({ playerPoint.x,startPointY,playerPoint.z });
			isStartStage = false;
			player_->IsAnimationOnlyUpdate(false);//演出モードを終了し操作できるように
			player_->IsJumping();//強制的にジャンプさせて飛び出たようにする
			return;
		}

		startPointY += 0.1f;
		player_->SetTranslate({ playerPoint.x,startPointY,playerPoint.z });
		
		for (auto& enemy : enemies) {
			enemy->GrabityZero();
		}
	}


	if (isNextStage) {
		player_->IsAnimationOnlyUpdate(true);
		player_->SetRotate({ 0,0,0 });
		return;
	}
	
	for (auto& enemy : enemies) {
		enemy->SetPlayer(player_.get());
		enemy->Update();
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}

	ChangeCheckPoint();

	//共有イベントフラグ
	bool isEventCommon = false;

	for (auto& eventTrigger : eventTriggers) {

		if (eventTrigger->GetEventData().isEvent) {
			isEventCommon = true;
			eventTrigger->SetPopEnemies(enemies);
			eventTrigger->Update();

			enemies = std::move(eventTrigger->GetPopEnemy());
		}
	}

	Respawn();

	//演出では使わない
	if (!isStartStage) {
		startWarp->Vanish();//出てきた後消えるようにする
		CollisionCommon();
	}

	//イベント中はカメラが固定
	if (isEventCommon) {
		//カメラ固定
		worldTransformCamera_.rotation_ = cameraRotate;
		worldTransformCamera_.translation_ = cameraTranslate;
	}
	else {
		//カメラの移動範囲
		//次ステージ移動時はズームされるのでここは除外
		if (!isNextStage) {
			CameraControl();
		}
	}

	//落ちた場合
	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}

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
	
	startWarp->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();

}

void GameScene::Finalize() {
	enemies.clear();
	stageObjects.clear();
	stagesAABB.clear();
}

void GameScene::WarterWarpExit() {
	
	//初期化
	startWarp = std::make_unique<WarpGate>();
	startWarp->Initialize();
	isStartStage = true;
	startPointY = 10.0f;

	playerPoint = player_->GetTranslate();
	startPointY = playerPoint.y - startPointY;//プレイヤーが真下からくるように設定する

	//ワープゲート出口の位置決め
	Vector3 warpPosition = player_->GetTranslate();

	//当たり判定
	AABB startWarpAABB;
	startWarpAABB.max = warpPosition + Vector3{ 0,1,0 };
	startWarpAABB.min = warpPosition + Vector3{ 0,-10,0 };

	//プレイヤー初期位置の真下に
	warpPosition = UnderCollision(stagesAABB, startWarpAABB, playerPoint);
	warpPosition.y += 0.02f;//重ならないように影より上にする

	startWarp->SetPosition(warpPosition);//playerの真下に
	startWarp->SetRotation({ 90.0f,0.0f,0.0f });//下向きにして水たまりに

	player_->IsAnimationOnlyUpdate(true);
}

void GameScene::ChangeCheckPoint() {
	//リスポーン変更した時
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject.get() == dynamic_cast<CheckPoint*>(stageObject.get())) {
				CheckPoint* checkPoint = dynamic_cast<CheckPoint*>(stageObject.get());
				player_->SetRespownPosition(checkPoint->GetPosition());
			}
		}
	}
}

void GameScene::CameraControl() {

	//Point1とPoint2から出たとき
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
	//カメラ座標系更新
	worldTransformCamera_.UpdateMatrix();

	//カメラ更新
	camera->SetRotate(worldTransformCamera_.rotation_);
	camera->SetTranslate(worldTransformCamera_.translation_);
}

void GameScene::Respawn() {
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
			//残機が0の場合ゲームオーバー
			NextSceneFadeInStart("GameOver");
		}
		
		for (auto& eventTrigger : eventTriggers){
			eventTrigger->FailureEvent();
			MainCamera();
		}
	}

}