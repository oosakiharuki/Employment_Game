#include "GameScene.h"
#include <sstream>
#include "SceneManager.h"

#include "SelectScene.h"
#include "GameOverScene.h"
#include "ClearScene.h"

#include "FadeScreen.h"

using namespace MyMath;

void GameScene::Initialize() {
	//ゲームオブジェクト配置
	LevelEditorObjectSetting();
	
	//BGM、SEの設定
	BGMData_ = Audio::GetInstance().LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance().LoadWave("resource/sound/bane.wav");

	//BGM再生(リピート)
	Audio::GetInstance().SoundPlayWave(BGMData_, volume_, true);

	//スタート演出
	WaterWarpExit();

	//フェードスタート
	FadeScreen::GetInstance().FadeStart(type_fadeOut);

	backGround = std::make_unique<BackGroundObject>();
	backGround->Initialize();

	//ポーズ画面
	PauseScreen::GetInstance().BeforeChangeScene("pauseReturnSelect.png",std::make_unique<SelectScene>());

	CollisionManager::GetInstance().ResetFrag();
}

void GameScene::Update() {
	if (PauseScreen::GetInstance().IsPause()) {
		PauseScreen::GetInstance().Update(); 
		return;
	}
	if (Input::GetInstance().TriggerKey(DIK_ESCAPE)) {
		PauseScreen::GetInstance().PauseFlag(true);
	}

	//演出用のワープゲート出口
	startWarp_->Update();

	//カメラコントロール
	cameraControl_->SetPlayerPosition(player_->GetTranslate());
	//プレイヤーが倒されたらシェイク
	(player_->GetHp() == 0) ? cameraControl_->ShakeMode(true) : cameraControl_->ResetShakeTime();
	//更新処理
	cameraControl_->Update(&*camera_.get());

	//プレイヤー更新処理
	player_->Update();

	//ステージの更新処理
	stageObj_->Update();
	
	//死んでしまった、復活(リスポーン)する時
	Respawn();

	//プレイヤーが死んでしまったら通らない(停止)
	PlayerAliveUpdate();

	if (!player_->GetPerformanceMode()) {
		startWarp_->Vanish();//出てきた後消えるようにする	
	}

	backGround->Update();

	for (auto& guide : guides_) {
		guide->Update();
	}

	//プレイヤーが移動したら変更
	UIManager::GetInstance().SetPlayerTranslate(player_->GetTranslate());
	//スプライト更新処理
	UIManager::GetInstance().Update();

#ifdef  USE_IMGUI

	ImGui::Begin("camera");
	ImGui::SliderFloat("volume", &volume_, 0.0f, 1.0f);
	ImGui::End();

#endif //  USE_IMGUI

	Audio::GetInstance().ControlVolume(BGMData_, volume_);
}

void GameScene::PlayerAliveUpdate() {
	if (player_->GetHp() == 0) return;

	//敵の更新
	for (auto& enemy : enemies_) {
		enemy->SetPlayer(player_.get());
		enemy->Update();
	}

	//ステージオブジェクトの更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	//イベントトリガーの更新
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->Update(*cameraControl_, levelEditor_, std::move(enemies_));
		enemies_ = eventTrigger->GetPopEnemy();//moveで渡したのを返してもらう(eventTriggerで増える)
	}

	//イベントで全て倒したら削除
	eventTriggers_.remove_if([](auto& event) {
		if (event->EventEnd()) {
			return true;
		}
		return false;
	});

	if (boss_) {
		boss_->SetPlayer(player_.get());
		boss_->Update();
	}
	//使用する当たり判定
	CollisionManager::GetInstance().CollisionUpdate();	

}

void GameScene::Draw() {
	//モデル描画処理
	GLTFCommon::GetInstance().Command();
	backGround->Draw();

	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->Draw();
	}

	if (boss_) {
		boss_->Draw();
	}

	//モデル描画処理
	Object3dCommon::GetInstance().Command();

	stageObj_->Draw();

	for (auto& guide : guides_) {
		guide->Draw();
	}

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
	ParticleCommon::GetInstance().Command();

	//敵のパーティクル描画
	for (auto& enemy : enemies_) {
		enemy->DrawParticle();
	}
	//プレイヤーのパーティクル描画
	player_->DrawParticle();
	//イベントトリガーのパーティクル描画
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->DrawParticle();
	}

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance().Command();

}

void GameScene::Finalize() {
	UIManager::GetInstance().Finalize();
}


void GameScene::LevelEditorObjectSetting(const std::string& levelEditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();
	stageFileName_ = NextStageSave::GetInstance().GetNextStageSaveData().nextStageFile;//ステージの全体層(.obj)

	//jsonファイルで設定したゲームオブジェクトの配置処理をまとめた
	SpitOutGameObject();
}

void GameScene::SpitOutGameObject() {
	//ステージのjsonを読み取る
	levelEditor_.LoadLevelEditor("resource/LevelEditor/" + stageFileName_ + ".json");
	spitOut_.SetLevelEditor(&levelEditor_);

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	//カメラコントロール設定
	spitOut_.SpitOutCamera(cameraControl_);
	//プレイヤーから使用するカメラに移動
	spitOut_.CameraStartPointPlayer(cameraControl_);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance().SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance().SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance().SetDefaultCamera(camera_.get());
	DebugWireframes::GetInstance().SetDefaultCamera(camera_.get());
	CubeMap::GetInstance().SetDefaultCamera(camera_.get());

	//プレイヤーの体力を上書き
	player_->Initialize();//初期設定
	player_->SetHp(NextStageSave::GetInstance().GetNextStageSaveData().playerHp);
	player_->SetRemain(NextStageSave::GetInstance().GetNextStageSaveData().playerRemain);
	//プレイヤーを配置
	spitOut_.SpitOutPlayer(player_);
	//ステージの当たり判定を設定/配置
	spitOut_.SpitOutStage(stageObj_, stageFileName_);
	//ステージオブジェクトの配置
	stageObjects_ = std::move(spitOut_.SpitOutStageObject());
	//敵の配置
	enemies_ = std::move(spitOut_.SpitOutEnemies());
	//イベントトリガーの配置
	eventTriggers_ = std::move(spitOut_.SpitOutEventTrigger());
	
	//チュートリアル用の操作方法スプライト
	if (stageFileName_ == "stage_0") {
		guides_ = std::move(spitOut_.SpitOutGuide());
	}

	//ボスの配置
	spitOut_.SpitOutBoss(boss_);
	if (boss_) {
		cameraControl_->CameraYFixed(true);
	}
}

void GameScene::WaterWarpExit() {
	//初期化
	startWarp_ = std::make_unique<WarpGate>();
	startWarp_->Initialize();

	startWarp_->SetRotation({ kStartWarpGateRange_ ,0.0f,0.0f });//下向きにして水たまりに
	startWarp_->WarpExit(player_->GetTranslate());

	player_->IsPerformanceFlag(true);
}

void GameScene::Respawn() {
	//死んでしまったとき || 残機が0の時
	if (!player_->IsRespawn() || player_->GetRemain() == 0) {
		return;
	}

	//敵が復活
	enemies_.clear();//一度リセット
	enemies_ = std::move(spitOut_.SpitOutEnemies());//敵の配置

	//突破できてないならやり直し
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->FailureEvent();
	}

	//リセット
	boss_.reset();
	//ボスの配置
	spitOut_.SpitOutBoss(boss_);

	player_->RespawnEnd();

	cameraControl_->CameraSettingCheckPoint(levelEditor_.GetLevelData()->cameraInit["MainCamera"]);
}

void GameScene::SceneUpdate() {
#ifdef USE_IMGUI
	if (Input::GetInstance().TriggerKey(DIK_F2)) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<ClearScene>());//クリアシーンに移動
	}
	if (Input::GetInstance().TriggerKey(DIK_F3)) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<GameOverScene>());//ゲームオーバーシーンに移動
	}
#endif // USE_IMGUI
	
	//ゴールした+カメラズームが完了
	if (CollisionManager::GetInstance().IsGoal() && cameraControl_->ZoomEnd()) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<ClearScene>());//クリアシーンに移動
	}
	//ワープする+カメラズームが完了
	else if (CollisionManager::GetInstance().IsWarp() && cameraControl_->ZoomEnd()) {
		//次のステージに進む時Hpなどパラメータがリセットされないようにする
		NextStageSave::GetInstance().SetPlayerHp(player_->GetHp()); //現在のプレイヤー体力を保存
		NextStageSave::GetInstance().SetPlayerRemain(player_->GetRemain()); //現在のプレイヤー残機を保存
		SceneManager::GetInstance().ChangeScene(std::make_unique<GameScene>());//次のステージに移動(ゲームシーンであることは変わらない)
	}
	else if (player_->GetRemain() == 0) {
		//残機が0の場合ゲームオーバー
		FadeScreen::GetInstance().SetMaskTexture("fade02.png");//フェードのマスク変更
		FadeScreen::GetInstance().SetBackGround("black.png");  //フェードのテクスチャ変更

		SceneManager::GetInstance().ChangeScene(std::make_unique<GameOverScene>());//ゲームオーバーシーンに移動
	}

	if (boss_) {
		//ボスを倒したら
		if (boss_->IsDeadMotionFinish()) {
			SceneManager::GetInstance().ChangeScene(std::make_unique<ClearScene>());//クリアシーンに移動
		}
	}
	
	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//BGM停止
		Audio::GetInstance().StopWave(BGMData_);
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance().FadeStart(type_fadeIn);
	}
}