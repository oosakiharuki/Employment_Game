#include "SelectScene.h"
#include "StageObjectFunction.h"
#include "SceneManager.h"

using namespace MyMath;
using namespace StageObjectFunction;

void SelectScene::Initialize() {

	//移行データ初期化
	sceneSaveData_.nextStageFile = "stage_select";

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting(sceneSaveData_.nextStageFile);

	//ステージの全体層
	stageObj_ = std::make_unique<Object3d>();
	stageObj_->Initialize();
	stageObj_->SetModelFile("stage_select.obj");

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープ、ゴールフラグのリセット
	CollisionManager::GetInstance()->ResetFlag();
}

void SelectScene::Update() {
	
	//ゲームパットの更新
	input_->JoystickUpdate(state_, preState_);

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	//ワープしてゲームシーンに移動
	if (CollisionManager::GetInstance()->IsWarp()) {
		WarpNextScene(*player_.get(),cameraControl_.get(), isNextGameScene);
	}
	
	//カメラコントロールの更新
	cameraControl_->Update(&*camera_);

	//プレイヤー更新
	player_->Update();

	//ステージ更新
	stageObj_->Update();

	//当たり判定
	CollisionCommon();

	//ガイド更新処理
	UIManager::GetInstance()->Update();
}

void SelectScene::Draw() {

	CubeMap::GetInstance()->Command();

	Object3dCommon::GetInstance()->Command();

	//ステージ描画
	stageObj_->Draw();

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


void SelectScene::LevelEditorObjectSetting(const std::string& levelEditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();

	stageFileName_ = sceneSaveData_.nextStageFile;//ステージの全体層(.obj)

	//値が入っている場合
	if (levelEditor_file != "") {
		//代入
		stageFileName_ = levelEditor_file;
		sceneSaveData_.playerHp = player_->GetMaxHp();
	}

	SpitOutGameObject();

	//操作方法スプライト
	UIManager::GetInstance()->CreateGuide(kGuideMove_);
	UIManager::GetInstance()->CreateGuide(kGuideWarp_);
}

void SelectScene::SpitOutGameObject() {
	//ステージのjsonを読み取る
	levelEditor_.LoadLevelEditor("resource/LevelEditor/" + stageFileName_ + ".json");
	spitOut_.SetLevelEditor(&levelEditor_);

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	spitOut_.SpitOutCamera(cameraControl_);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera_.get());
	CubeMap::GetInstance()->SetDefaultCamera(camera_.get());

	//プレイヤーの体力を上書き
	player_->Initialize();//初期設定
	player_->SetHp(sceneSaveData_.playerHp);
	player_->SetRemain(sceneSaveData_.playerRemain);
	//プレイヤーを配置
	spitOut_.SpitOutPlayer(player_);
	//ステージの当たり判定を設定/配置
	spitOut_.SpitOutStage(stageObj_, stageFileName_, stagesAABB_);
	//ステージオブジェクトを配置
	spitOut_.SpitOutStageObject(stageObjects_);
}

void SelectScene::CollisionCommon() {
	//ゲーム内で使用する当たり判定
	//プレイヤーとステージ自体
	CollisionManager::GetInstance()->PlayerAndStage(player_.get(), stagesAABB_);
	//プレイヤーとステージオブジェクト
	CollisionManager::GetInstance()->PlayerAndStageObject(player_.get(), stageObjects_);
}

void SelectScene::SceneUpdate() {

	//タイトルに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		SceneManager::GetInstance().ChangeScene("Title");
	}

	if (isNextGameScene) {
		SceneManager::GetInstance().ChangeScene("Game");
	}

	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance()->FadeStart(type_fadeIn);
	}
}