#include "GameScene.h"
#include <sstream>
using namespace MyMath;

void GameScene::Initialize() {

	PreviousSceneData();

	LevelEditorObjectSetting();


	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, volume, true);

	WarterWarpExit();

	//演出時に重力が発動しないようにする
	for (auto& enemy : enemies) {
		enemy->isPerformanceFlag(true);
	}

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void GameScene::Update() {

	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
		Audio::GetInstance()->StopWave(BGMData_);
	}

	startWarp->Update();

	if (isWarp && cameraControl_->MaxZoom()) {
		NextSceneFadeInStart("NextStage");
	}

	WarpNextScene();

	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {	
			//ゴール
			if (stageObject.get() == dynamic_cast<Goal*>(stageObject.get())) {
				NextSceneFadeInStart("Clear");
				return;
			}
		}
	}

	skyBox->Update(MakeScaleMatrix({ 1000,1000,1000 }));//大きくするため

	//
	cameraControl_->SetPlayerPosition(player_->GetTranslate());

	//プレイヤーが倒されたらシェイク
	(player_->GetIsDead()) ? cameraControl_->ShakeMode(true) : cameraControl_->ResetShakeTime();

	cameraControl_->Update(&*camera.get());

	player_->Update();
	
	stageobj->Update();

	if (isStartStage) {
		//プレイヤー配置座標 + 地面当たり判定によって上げられる分
		if (startPointY >= playerPoint.y) {
			player_->SetTranslate({ playerPoint.x,startPointY,playerPoint.z });
			isStartStage = false;
			player_->isPerformanceFlag(false);//演出モードを終了し操作できるように
			player_->IsJumping();//強制的にジャンプさせて飛び出たようにする
			//カイジョ
			for (auto& enemy : enemies) {
				enemy->isPerformanceFlag(false);
			}
			return;
		}

		startPointY += 0.1f;
		player_->SetTranslate({ playerPoint.x,startPointY,playerPoint.z });

	}


	if (isWarp) {
		player_->isPerformanceFlag(true);
		player_->SetRotate({ 0,0,0 });
		return;
	}


	Respawn();

	//敵やオブジェクトを止める(時間停止)
	if (player_->GetIsDead()) {
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

	if (!isStartStage) {
		startWarp->Vanish();//出てきた後消えるようにする	
	}

	CollisionCommon();

	//落ちた場合
	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}

#ifdef  USE_IMGUI

	ImGui::Begin("camera");

	//カメラの端
	ImGui::Text("p1 : %f %f %f", cameraPoint1.x, cameraPoint1.y, cameraPoint1.z);
	ImGui::Text("p2 : %f %f %f", cameraPoint2.x, cameraPoint2.y, cameraPoint2.z);

	ImGui::SliderFloat("volume", &volume, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI

	Audio::GetInstance()->ControlVolume(BGMData_, volume);
}

void GameScene::Draw() {
	
	Cubemap::GetInstance()->Command();
	//skyBox->Draw();//見ずらいため一度コメントアウト
	

	//モデル描画処理
	GLTFCommon::GetInstance()->Command();

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();
	
	stageobj->Draw();


	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Draw();
	}
	
	startWarp->Draw();
	
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();

}

void GameScene::Finalize() {}

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
	warpPosition = CollisionManager::GetInstance()->UnderCollision(stagesAABB, startWarpAABB, playerPoint);
	warpPosition.y += 0.02f;//重ならないように影より上にする

	startWarp->SetPosition(warpPosition);//playerの真下に
	startWarp->SetRotation({ 90.0f,0.0f,0.0f });//下向きにして水たまりに

	player_->isPerformanceFlag(true);
}

void GameScene::ChangeCheckPoint() {
	//リスポーン変更した時
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject.get() == dynamic_cast<CheckPoint*>(stageObject.get())) {
				CheckPoint* checkPoint = dynamic_cast<CheckPoint*>(stageObject.get());
				player_->SetInit_Position(checkPoint->GetPosition(),player_->GetRotate());
			}
		}
	}
}

void GameScene::Respawn() {
	//プレイヤーが死んで、リスポーン地点が変更していないとき敵は復活する
	if (player_->GetIsDead() && player_->GetIsRespawn()) {

		if (RemainingLife == 0) {
			//残機が0で倒された場合ゲームオーバー
			NextSceneFadeInStart("GameOver");
			FadeScreen::GetInstance()->SetMaskTexture("fade02.png");
			FadeScreen::GetInstance()->SetBackGround("black.png");
			return;
		}
		RemainingLife--;

		for (auto& enemy : enemies) {
			enemy->RespawnEnemy();
		}
		player_->RespawnPlayer();

		for (auto& eventTrigger : eventTriggers) {
			eventTrigger->FailureEvent();
			cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit["MainCamera"], false);
		}
	}
}