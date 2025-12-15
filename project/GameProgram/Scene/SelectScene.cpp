#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	//移行データ初期化
	sceneSaveData_.nextStageFile = "stage_select";

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting(sceneSaveData_.nextStageFile);

	//ステージの全体層
	stageobj_ = std::make_unique<Object3d>();
	stageobj_->Initialize();
	stageobj_->SetModelFile("stage_select.obj");

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープ、ゴールフラグのリセット
	CollisionManager::GetInstance()->ResetFlag();
}

void SelectScene::Update() {
	
	//ゲームパットの更新
	input_->JoystickUpdate(state_, preState_);

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	//タイトルに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		NextSceneFadeInStart("Title");
	}

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	//カメラコントロールの更新
	cameraControl_->Update(&*camera_);

	//ワープしてゲームシーンに移動
	WarpNextScene("Game");

	//プレイヤー更新
	player_->Update();
	
	//ステージ更新
	stageobj_->Update();

	//当たり判定
	CollisionCommon();

	//ガイド更新処理
	UIManager::GetInstance()->Update();
}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();

	Object3dCommon::GetInstance()->Command();

	//ステージ描画
	stageobj_->Draw();

	//ステージオブジェクト描画
	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}

	GLTFCommon::GetInstance()->Command();
	//プレイヤー描画
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	//プレイヤーパーティクル描画
	player_->DrawParticle();

	SpriteCommon::GetInstance()->Command();
	//説明ガイド
	UIManager::GetInstance()->Draw();
	UIManager::GetInstance()->GuideDraw();
}

void SelectScene::Finalize() {
	UIManager::GetInstance()->Finalize();
}


void SelectScene::LevelEditorObjectSetting(const std::string& leveleditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();

	stageFileName_ = sceneSaveData_.nextStageFile;//ステージの全体層(.obj)

	//値が入っている場合
	if (leveleditor_file != "") {
		//代入
		stageFileName_ = leveleditor_file;
		sceneSaveData_.playerHp = player_->GetMaxHp();
	}

	//ステージのjsonを読み取る
	levelediter_.LoadLevelediter("resource/Levelediter/" + stageFileName_ + ".json");
	spitOut_.SetLevelEditor(&levelediter_);

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	//カメラコントロール設定
	cameraControl_ = std::make_unique<CameraControl>();
	cameraControl_->Initialize();//初期化
	//メインのカメラ
	cameraControl_->CameraSetting(levelediter_.GetLevelData()->cameraInit["MainCamera"], false);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera_.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera_.get());

	//プレイヤーの体力を上書き
	player_->SetHp(sceneSaveData_.playerHp);
	player_->SetZanki(sceneSaveData_.playerZanki);
	
	spitOut_.SpitOutPlayer(player_);
	spitOut_.SpitOutStage(stageobj_, stageFileName_, stagesAABB_);
	spitOut_.SpitOutStageObject(stageObjects_);

	//操作方法スプライト
	UIManager::GetInstance()->CreateGuide(kGuideMove_);
	UIManager::GetInstance()->CreateGuide(kGuideWarp_);
}

void SelectScene::WarpNextScene(const std::string& nextScene) {
	//ワープするときじゃないなら
	if (!CollisionManager::GetInstance()->IsWarp()) {
		return;
	}

	//プレイヤーが演出判定でない
	//「!player_->GetPerformanceMode()」は何度もplayer_のGetTranslateを読み取ることで予定の速度より速くならないようにするため
	if (!player_->GetPerformanceMode()) {
		//プレイヤーにカメラズーム
		CameraZoomPlayer();
		player_->BackDirection();//向きを前に(Z方向)
	}
	//カメラがズームし終わった
	if (cameraControl_->ZoomEnd()) {
		//次のステージに進む時Hpなどパラメータがリセットされないようにする
		sceneSaveData_.playerHp = player_->GetHp(); //現在のプレイヤー体力を保存
		sceneSaveData_.playerZanki = player_->GetZanki(); //現在のプレイヤー残機を保存
		NextStageSave::GetInstance()->SetPlayerParameta(sceneSaveData_); //移行データを代入する
		//フェードインした後、次のシーンに
		NextSceneFadeInStart(nextScene);
	}
}

void SelectScene::CameraZoomPlayer() {
	//ズーム開始(カメラ現在地点 -> プレイヤー座標 + 少し離れた場所)
	cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos_);
	player_->IsPerformanceFlag(true);//演出モード
}

void SelectScene::CollisionCommon() {
	//ゲーム内で使用する当たり判定
	//プレイヤーとステージ自体
	CollisionManager::GetInstance()->PlayerAndStage(player_.get(), stagesAABB_);
	//プレイヤーとステージオブジェクト
	CollisionManager::GetInstance()->PlayerAndStageObject(player_.get(), stageObjects_);
}

