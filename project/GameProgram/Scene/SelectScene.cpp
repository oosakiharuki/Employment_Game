#include "SelectScene.h"
#include "StageObjectFunction.h"

using namespace MyMath;
using namespace StageObjectFunction;

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

	SpitOutGameObject();

	//操作方法スプライト
	UIManager::GetInstance()->CreateGuide(kGuideMove_);
	UIManager::GetInstance()->CreateGuide(kGuideWarp_);
}

void SelectScene::SpitOutGameObject() {
	//ステージのjsonを読み取る
	levelediter_.LoadLevelediter("resource/Levelediter/" + stageFileName_ + ".json");
	spitOut_.SetLevelEditor(&levelediter_);

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	spitOut_.SpitOutCamera(cameraControl_);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera_.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera_.get());

	//プレイヤーの体力を上書き
	player_->Initialize();//初期設定
	player_->SetHp(sceneSaveData_.playerHp);
	player_->SetZanki(sceneSaveData_.playerZanki);
	//プレイヤーを配置
	spitOut_.SpitOutPlayer(player_);
	//ステージの当たり判定を設定/配置
	spitOut_.SpitOutStage(stageobj_, stageFileName_, stagesAABB_);
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
		nextSceneNo_ = "Title";
	}

	if (isNextGameScene) {
		nextSceneNo_ = "Game";
	}

	//ゲームシーンに移動
	ChangeSceneNo();
}