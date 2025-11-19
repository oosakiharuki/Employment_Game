#include "GameScene.h"
#include <sstream>
using namespace MyMath;

void GameScene::Initialize() {
	//ゲームデータ引継ぎ(Hp,ステージ面)
	PreviousSceneData();

	//ゲームオブジェクト配置
	LevelEditorObjectSetting("stage_2");

	//BGM、SEの設定
	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	//BGM再生(リピート)
	Audio::GetInstance()->SoundPlayWave(BGMData_, volume_, true);

	//スタート演出
	WarterWarpExit();

	//演出時に重力が発動しないようにする
	for (auto& enemy : enemies_) {
		enemy->IsPerformanceFlag(true);
	}

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープやゴールのフラグをオフ
	CollisionManager::GetInstance()->ResetFlag();
}

void GameScene::Update() {
	
	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
		Audio::GetInstance()->StopWave(BGMData_);//BGM停止
	}
	//演出用のワープゲート出口
	startWarp_->Update();

	if (CollisionManager::GetInstance()->IsWarp() && cameraControl_->MaxZoom()) {
		NextSceneFadeInStart("NextStage");
	}

	if (CollisionManager::GetInstance()->IsGoal() && cameraControl_->MaxZoom()) {
		NextSceneFadeInStart("Clear");
	}

	//カメラコントロール
	cameraControl_->SetPlayerPosition(player_->GetTranslate());
	//プレイヤーが倒されたらシェイク
	(player_->GetIsDead()) ? cameraControl_->ShakeMode(true) : cameraControl_->ResetShakeTime();
	//更新処理
	cameraControl_->Update(&*camera_.get());

	//ゴールしたとき
	if (CollisionManager::GetInstance()->IsGoal() || player_->GetPerformanceMode()) {
		cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos_);
		player_->IsPerformanceFlag(true);
		player_->SetRotate({ 0,180.0f,0 });//向きを前に
	}

	player_->Update();
	
	//次のシーンに移動する演出
	WarpNextScene();

	stageobj_->Update();

	if (isStartStage_) {
		//プレイヤー配置座標 + 地面当たり判定によって上げられる分
		if (startPointY_ >= playerPoint_.y) {
			player_->SetTranslate({ playerPoint_.x,startPointY_,playerPoint_.z });
			isStartStage_ = false;
			player_->IsPerformanceFlag(false);//演出モードを終了し操作できるように
			player_->IsJumping();//強制的にジャンプさせて飛び出たようにする
			//演出フラグ解除
			for (auto& enemy : enemies_) {
				enemy->IsPerformanceFlag(false);
			}
			return;
		}

		startPointY_ += 0.1f;
		player_->SetTranslate({ playerPoint_.x,startPointY_,playerPoint_.z });
	}

	Respawn();

	//敵やオブジェクトを止める(時間停止)
	if (player_->GetIsDead()) {
		return;
	}
	
	for (auto& enemy : enemies_) {
		enemy->SetPlayer(player_.get());
		enemy->Update();
	}

	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	for (auto& eventTrigger : eventTriggers_) {

		if (eventTrigger->GetEventData().isEvent) {
			eventTrigger->SetPopEnemies(enemies_);
			eventTrigger->Update();

			enemies_ = std::move(eventTrigger->GetPopEnemy());
		}
	}

	if (!isStartStage_) {
		startWarp_->Vanish();//出てきた後消えるようにする	
	}

	CollisionCommon();

	//落ちた場合
	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}

	for (auto& a : setumei_) {
		a->Update();
	}

#ifdef  USE_IMGUI

	ImGui::Begin("camera");

	//カメラの端
	ImGui::Text("p1 : %f %f %f", cameraPointLeft_.x, cameraPointLeft_.y, cameraPointLeft_.z);
	ImGui::Text("p2 : %f %f %f", cameraPointRight_.x, cameraPointRight_.y, cameraPointRight_.z);

	ImGui::SliderFloat("volume", &volume_, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI

	Audio::GetInstance()->ControlVolume(BGMData_, volume_);
}

void GameScene::Draw() {
	
	Cubemap::GetInstance()->Command();
	//skyBox->Draw();//見ずらいため一度コメントアウト
	

	//モデル描画処理
	GLTFCommon::GetInstance()->Command();
	
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->Draw();
	}

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();

	stageobj_->Draw();


	for (auto& enemy : enemies_) {
		enemy->Draw();
		enemy->DrawCommon();
	}

	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}
	
	startWarp_->Draw();
	
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();
	
	DrawCommon();
}

void GameScene::Finalize() {}

void GameScene::WarterWarpExit() {
	
	//初期化
	startWarp_ = std::make_unique<WarpGate>();
	startWarp_->Initialize();
	isStartStage_ = true;
	startPointY_ = 10.0f;

	playerPoint_ = player_->GetTranslate();
	startPointY_ = playerPoint_.y - startPointY_;//プレイヤーが真下からくるように設定する

	//ワープゲート出口の位置決め
	Vector3 warpPosition = player_->GetTranslate();

	//当たり判定
	AABB startWarpAABB;
	startWarpAABB.max = warpPosition + Vector3{ 0,1,0 };
	startWarpAABB.min = warpPosition + Vector3{ 0,-10,0 };

	//プレイヤー初期位置の真下に
	warpPosition = CollisionManager::GetInstance()->UnderCollision(stagesAABB_, startWarpAABB, playerPoint_);
	warpPosition.y += 0.02f;//重ならないように影より上にする

	startWarp_->SetPosition(warpPosition);//playerの真下に
	startWarp_->SetRotation({ 90.0f,0.0f,0.0f });//下向きにして水たまりに

	player_->IsPerformanceFlag(true);
}

void GameScene::Respawn() {
	//プレイヤーが死んで、リスポーン地点が変更していないとき敵は復活する
	if (player_->GetIsDead() && player_->GetIsRespawn()) {

		if (RemainingLife_ == 0) {
			//残機が0で倒された場合ゲームオーバー
			NextSceneFadeInStart("GameOver");
			FadeScreen::GetInstance()->SetMaskTexture("fade02.png");
			FadeScreen::GetInstance()->SetBackGround("black.png");
			return;
		}
		//残機が一つ減る
		RemainingLife_--;

		//敵が復活
		for (auto& enemy : enemies_) {
			enemy->RespawnEnemy();
		}
		//プレイヤー復活
		player_->RespawnPlayer();
		//突破できてないならやり直し
		for (auto& eventTrigger : eventTriggers_) {
			eventTrigger->FailureEvent();
			cameraControl_->CameraSetting(levelediter_.GetLevelData()->cameraInit["MainCamera"], false);
		}
	}
}