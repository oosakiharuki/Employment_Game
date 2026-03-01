#include "SelectScene.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "FadeScreen.h"
#include "CubeMap.h"

using namespace MyMath;

void SelectScene::Initialize() {
	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting("stage_select");

	//ステージの全体層
	stageObj_ = std::make_unique<Object3d>();
	stageObj_->Initialize();
	stageObj_->SetModelFile("stage_select.obj");

	//フェードスタート
	FadeScreen::GetInstance().FadeStart(type_fadeOut);

	//背景の初期化処理
	backGround = std::make_unique<BackGroundObject>();
	backGround->Initialize();

	//ポーズ画面
	PauseScreen::GetInstance().BeforeChangeScene("pauseReturnTitle.png", std::make_unique<TitleScene>());

	CollisionManager::GetInstance().ResetFrag();
}

void SelectScene::Update() {
	if (PauseScreen::GetInstance().IsPause()) {
		PauseScreen::GetInstance().Update();
		return;
	}
	if (Input::GetInstance().TriggerKey(DIK_ESCAPE)) {
		PauseScreen::GetInstance().PauseFlag(true);
	}

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}
	
	//カメラコントロールの更新
	cameraControl_->Update(&*camera_);

	//プレイヤー更新
	player_->Update();

	//ステージ更新
	stageObj_->Update();

	//当たり判定
	CollisionManager::GetInstance().CollisionUpdate();

	//ガイド更新処理
	UIManager::GetInstance().Update();

	//背景更新
	backGround->Update();
}

void SelectScene::Draw() {

	CubeMap::GetInstance().Command();

	Object3dCommon::GetInstance().Command();

	//ステージ描画
	stageObj_->Draw();

	//ステージオブジェクト描画
	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}

	GLTFCommon::GetInstance().Command();
	//背景描画
	backGround->Draw();
	//プレイヤー描画
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance().Command();
	//プレイヤーパーティクル描画
	player_->DrawParticle();

	SpriteCommon::GetInstance().Command();
}

void SelectScene::Finalize() {
	UIManager::GetInstance().Finalize();
}


void SelectScene::LevelEditorObjectSetting(const std::string& levelEditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();

	stageFileName_ = levelEditor_file;//ステージの全体層(.obj)

	NextStageSave::GetInstance().SetPlayerHp(3); //現在のプレイヤー体力を保存
	NextStageSave::GetInstance().SetPlayerRemain(3); //現在のプレイヤー残機を保存

	SpitOutGameObject();

	//操作方法スプライト
	UIManager::GetInstance().CreateGuide(kGuideMove_);
	UIManager::GetInstance().CreateGuide(kGuideWarp_);
}

void SelectScene::SpitOutGameObject() {
	//ステージのjsonを読み取る
	levelEditor_.LoadLevelEditor("resource/LevelEditor/" + stageFileName_ + ".json");
	spitOut_.SetLevelEditor(&levelEditor_);

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	spitOut_.SpitOutCamera(cameraControl_);

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
	//ステージオブジェクトを配置
	stageObjects_ = std::move(spitOut_.SpitOutStageObject());
}

void SelectScene::SceneUpdate() {
	//ワープする+カメラズームが完了
	if (CollisionManager::GetInstance().IsWarp() && cameraControl_->ZoomEnd()) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<GameScene>());
	}

	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance().FadeStart(type_fadeIn);
	}
}