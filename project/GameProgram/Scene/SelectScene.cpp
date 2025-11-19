#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting("stage_select_test");

	skyBox_ = std::make_unique<BoxModel>();
	skyBox_->Initialize("resource/rostock_laage_airport_4k.dds");

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
	
	input_->JoystickUpdate(state_, preState_);

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	//ワープするflag && カメラがズームし終わった
	if (CollisionManager::GetInstance()->IsWarp() && cameraControl_->MaxZoom()) {
		NextSceneFadeInStart("Game");
	}

	//ワープするflag && カメラがズームし終わった
	if (CollisionManager::GetInstance()->IsGoal() && cameraControl_->MaxZoom()) {
		NextSceneFadeInStart("Clear");
	}

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	cameraControl_->Update(&*camera_);
	WarpNextScene();

	player_->Update();

	skyBox_->Update(MakeScaleMatrix({ 1000,1000,1000 }));
	stageobj_->Update();

	CollisionCommon();

	//ゴールしたとき
	if (CollisionManager::GetInstance()->IsGoal() || player_->GetPerformanceMode()) {
		cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos_);
		player_->IsPerformanceFlag(true);
		player_->SetRotate({ 0,180.0f,0 });//向きを前に
	}

	setumei_[0]->SetPosition({ 300,20 });
	setumei_[6]->SetPosition({ 600,20 });

	for (auto& a : setumei_) {
		a->Update();
	}

}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();
	//skyBox->Draw();

	Object3dCommon::GetInstance()->Command();

	stageobj_->Draw();

	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}

	GLTFCommon::GetInstance()->Command();
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	SpriteCommon::GetInstance()->Command();
	
	DrawCommon();
}

void SelectScene::Finalize() {}