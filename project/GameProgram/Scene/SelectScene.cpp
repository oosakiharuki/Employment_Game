#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	LevelEditorObjectSetting("stage_select");

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_select.obj");

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void SelectScene::Update() {
	
	input_->JoystickUpdate(state, preState);

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	if (isWarp) {
		if (cameraControl_->MaxZoom()) {
			NextSceneFadeInStart("Game");
		}
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}

	cameraControl_->Update(&*camera);
	

	player_->Update();

	if (isWarp) {
		player_->isPerformanceFlag(true);
		player_->SetRotate({ 0,0,0 });//向きを前に
		return;
	}

	skyBox->Update(MakeScaleMatrix({ 1000,1000,1000 }));
	stageobj->Update();

	CollisionCommon();

	WarpNextScene();

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

}

void SelectScene::Finalize() {}