#include "GameScene.h"
#include <sstream>
#include "StageObjectFunction.h"
#include "SceneManager.h"

using namespace MyMath;
using namespace StageObjectFunction;

void GameScene::Initialize() {
	//ゲームデータ引継ぎ(Hp,ステージ面)
	sceneSaveData_ = NextStageSave::GetInstance().GetNextStageSaveData();

	//ゲームオブジェクト配置
	LevelEditorObjectSetting();
	
	//BGM、SEの設定
	BGMData_ = Audio::GetInstance().LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance().LoadWave("resource/sound/bane.wav");

	//BGM再生(リピート)
	Audio::GetInstance().SoundPlayWave(BGMData_, volume_, true);

	//スタート演出
	WaterWarpExit();

	//演出時に重力が発動しないようにする
	for (auto& enemy : enemies_) {
		enemy->IsPerformanceFlag(true);
	}

	//フェードスタート
	FadeScreen::GetInstance().FadeStart(type_fadeOut);
	//ワープやゴールのフラグをオフ
	CollisionManager::GetInstance().ResetFlag();

	backGround = std::make_unique<BackGroundObject>();
	backGround->Initialize();
}

void GameScene::Update() {

	//演出用のワープゲート出口
	startWarp_->Update();

	//カメラコントロール
	cameraControl_->SetPlayerPosition(player_->GetTranslate());
	//プレイヤーが倒されたらシェイク
	(player_->GetIsDead()) ? cameraControl_->ShakeMode(true) : cameraControl_->ResetShakeTime();
	//更新処理
	cameraControl_->Update(&*camera_.get());

	//プレイヤー更新処理
	player_->Update();


	//プレイヤーがゴールした
	if (CollisionManager::GetInstance().IsGoal()) {
		PlayerGoal();
	}
	//次のシーンに移動する
	if (CollisionManager::GetInstance().IsWarp()) {
		WarpNextScene(*player_.get(),cameraControl_.get(), isNextLoadingStageScene);
	}

	//ステージの更新処理
	stageObj_->Update();

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
		}
		else {
			startPointY_ += kPlayerUp_;
			player_->SetTranslate({ playerPoint_.x,startPointY_,playerPoint_.z });
		}
	}
	
	//死んでしまった、復活(リスポーン)する時
	if (player_->GetIsDead() && player_->GetIsRespawn()) {
		Respawn();
	}

	//プレイヤーが死んでしまったら通らない(停止)
	if (!player_->GetIsDead()) {
		PlayerAliveUpdate();
	}

	if (!isStartStage_) {
		startWarp_->Vanish();//出てきた後消えるようにする	
	}

	//落ちた場合
	if (player_->GetTranslate().y < kFallEndY_) {
		player_->IsFall();
	}
	
	backGround->Update();

	//プレイヤーが移動したら変更
	UIManager::GetInstance().SetPlayerTranslate(player_->GetTranslate());
	//スプライト更新処理
	UIManager::GetInstance().Update();

#ifdef  USE_IMGUI

	ImGui::Begin("camera");

	//カメラの端
	ImGui::Text("p1 : %f %f %f", cameraPointLeft_.x, cameraPointLeft_.y, cameraPointLeft_.z);
	ImGui::Text("p2 : %f %f %f", cameraPointRight_.x, cameraPointRight_.y, cameraPointRight_.z);

	ImGui::SliderFloat("volume", &volume_, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI

	Audio::GetInstance().ControlVolume(BGMData_, volume_);
}

void GameScene::PlayerAliveUpdate() {
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

		if (eventTrigger->GetEventData().isEvent) {
			eventTrigger->SetPopEnemies(enemies_);
			eventTrigger->Update();

			enemies_ = std::move(eventTrigger->GetPopEnemy());
		}
	}

	if (boss_) {
		boss_->SetPlayer(player_.get());
		boss_->Update();
	}
	//使用する当たり判定
	CollisionCommon();

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

	UIManager::GetInstance().Draw();
	//説明ガイド
	UIManager::GetInstance().GuideDraw();
}

void GameScene::Finalize() {
	UIManager::GetInstance().Finalize();
}


void GameScene::LevelEditorObjectSetting(const std::string& levelEditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();

	stageFileName_ = sceneSaveData_.nextStageFile;//ステージの全体層(.obj)

	//値が入っている場合
	if (levelEditor_file != "") {
		//代入
		stageFileName_ = levelEditor_file;
		sceneSaveData_.playerHp = player_->GetMaxHp();
	}
	//jsonファイルで設定したゲームオブジェクトの配置処理をまとめた
	SpitOutGameObject();

	//チュートリアル用の操作方法スプライト
	if (stageFileName_ == "stage_0") {
		UIManager::GetInstance().CreateGuide(kGuideMove_);
		UIManager::GetInstance().CreateGuide(kGuideJump_);
		UIManager::GetInstance().CreateGuide(kGuideFire_);
		UIManager::GetInstance().CreateGuide(kGuideShield_);
		UIManager::GetInstance().CreateGuide(kGuideBrink_);
		UIManager::GetInstance().CreateGuide(kGuideGliding_);
		UIManager::GetInstance().CreateGuide(kGuideWarp_);
	}
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
	player_->SetHp(sceneSaveData_.playerHp);
	player_->SetRemain(sceneSaveData_.playerRemain);
	//プレイヤーを配置
	spitOut_.SpitOutPlayer(player_);
	//ステージの当たり判定を設定/配置
	spitOut_.SpitOutStage(stageObj_, stageFileName_, stagesAABB_);
	//ステージオブジェクトの配置
	spitOut_.SpitOutStageObject(stageObjects_);
	//敵の配置
	spitOut_.SpitOutEnemies(enemies_);
	//イベントトリガーの配置
	spitOut_.SpitOutEventTrigger(eventTriggers_);

	//敵がステージ全体当たり判定をもらう(プレイヤーを見つける処理に使う)
	for (auto& enemy : enemies_) {
		enemy->SetStages(stagesAABB_);
	}

	//ボスの配置
	spitOut_.SpitOutBoss(boss_);
	if (boss_) {
		cameraControl_->CameraYFixed(true);
	}
}


void GameScene::PlayerGoal() {
	//プレイヤーにカメラズーム
	//何度も読み取ってワープより早く移動する
	cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos_);
	player_->IsPerformanceFlag(true);//演出モード           
	player_->DirectionTheCamera();//向きをカメラのほうに(-Z方向)

	if (cameraControl_->ZoomEnd()) {
		isNextClearScene = true;
	}
}

void GameScene::CollisionCommon() {
	//ゲーム内で使用する当たり判定
	//プレイヤーと敵
	CollisionManager::GetInstance().PlayerAndEnemy(player_.get(), enemies_);
	//プレイヤーとステージ自体
	CollisionManager::GetInstance().PlayerAndStage(player_.get(), stagesAABB_);
	//プレイヤーとステージオブジェクト
	CollisionManager::GetInstance().PlayerAndStageObject(player_.get(), stageObjects_);
	//プレイヤーとイベントトリガー
	CollisionManager::GetInstance().PlayerAndEventTrigger(player_.get(), eventTriggers_,cameraControl_.get(), levelEditor_);
	//敵とステージ自体
	CollisionManager::GetInstance().EnemyAndStage(enemies_,stagesAABB_);

	if (boss_) {
		//ボスとプレイヤー
		CollisionManager::GetInstance().BossAndPlayer(*player_.get(),*boss_.get());
	}
}

void GameScene::WaterWarpExit() {
	
	//初期化
	startWarp_ = std::make_unique<WarpGate>();
	startWarp_->Initialize();
	isStartStage_ = true;

	playerPoint_ = player_->GetTranslate();
	startPointY_ = playerPoint_.y + startPointY_;//プレイヤーが真下からくるように設定する

	//ワープゲート出口の位置決め
	Vector3 warpPosition = player_->GetTranslate();

	//当たり判定
	AABB startWarpAABB;
	startWarpAABB.max = warpPosition;
	startWarpAABB.min = warpPosition + Vector3{ 0,startPointY_,0 };

	//プレイヤー初期位置の真下に
	warpPosition = CollisionManager::GetInstance().UnderCollision(stagesAABB_, startWarpAABB, playerPoint_);
	warpPosition.y += kWarpGateUpThanShadow_;//重ならないように影より上にする

	startWarp_->SetPosition(warpPosition);//playerの真下に
	startWarp_->SetRotation({ kStartWarpGateRange_ ,0.0f,0.0f });//下向きにして水たまりに

	player_->IsPerformanceFlag(true);
}

void GameScene::Respawn() {
	if (player_->GetRemain() == 0) {
		//残機が0で倒された場合ゲームオーバー
		isNextGameOverScene = true;
		FadeScreen::GetInstance().SetMaskTexture("fade02.png");
		FadeScreen::GetInstance().SetBackGround("black.png");
		return;
	}

	//敵が復活
	for (auto& enemy : enemies_) {
		enemy->RespawnEnemy();
	}
	//プレイヤー復活
	player_->RespawnPlayer();
	//突破できてないならやり直し
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->FailureEvent();
		cameraControl_->CameraInterpolation(levelEditor_.GetLevelData()->cameraInit["MainCamera"], false);
	}

	//リセット
	boss_.reset();
	//ボスの配置
	spitOut_.SpitOutBoss(boss_);
}

void GameScene::SceneUpdate() {
#ifdef USE_IMGUI
	if (Input::GetInstance().TriggerKey(DIK_F2)) {
		SceneManager::GetInstance().ChangeScene("Clear");//クリアシーンに移動
	}
	if (Input::GetInstance().TriggerKey(DIK_F3)) {
		SceneManager::GetInstance().ChangeScene("GameOver");//ゲームオーバーシーンに移動
	}
#endif // USE_IMGUI

	if (Input::GetInstance().TriggerKey(DIK_ESCAPE)) {
		SceneManager::GetInstance().ChangeScene("Select");//ゲームオーバーシーンに移動
	}

	if (isNextClearScene) {
		SceneManager::GetInstance().ChangeScene("Clear");//クリアシーンに移動
	}
	else if (isNextLoadingStageScene) {
		SceneManager::GetInstance().ChangeScene("Game");//次のステージに移動(ゲームシーンであることは変わらない)
	}
	else if (isNextGameOverScene) {
		SceneManager::GetInstance().ChangeScene("GameOver");//ゲームオーバーシーンに移動
	}

	if (boss_) {
		//ボスを倒したら
		if (boss_->IsDead()) {
			SceneManager::GetInstance().ChangeScene("Clear");//クリアシーンに移動
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