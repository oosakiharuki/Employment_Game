#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting("stage_select_test");

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	//ステージの全体層
	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_select.obj");

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープ、ゴールフラグのリセット
	CollisionManager::GetInstance()->ResetFlag();
}

void SelectScene::Update() {
	
	input_->JoystickUpdate(state, preState);

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
	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}

	cameraControl_->Update(&*camera);
	WarpNextScene();

	player_->Update();

	skyBox->Update(MakeScaleMatrix({ 1000,1000,1000 }));
	stageobj->Update();

	CollisionCommon();

	//ゴールしたとき
	if (CollisionManager::GetInstance()->IsGoal() || player_->GetPerformanceMode()) {
		cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos);
		player_->IsPerformanceFlag(true);
		player_->SetRotate({ 0,180.0f,0 });//向きを前に
	}

	setumei[0]->SetPosition({ 300,20 });
	setumei[6]->SetPosition({ 600,20 });

	for (auto& a : setumei) {
		a->Update();
	}

}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();
	//skyBox->Draw();

	Object3dCommon::GetInstance()->Command();

	stageobj->Draw();

	for (auto& stageObject : stageObjects) {
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