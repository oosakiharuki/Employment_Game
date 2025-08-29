#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_select.json");

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");
	wt.Initialize();
}

void SelectScene::Update() {

	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();
		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			sceneNo = Game;
			isfadeStart = false;
		}
	}
	else{
		FadeScreen::GetInstance()->FedeOut();
	}

	for (auto& warpgate : warpGates) {
		warpgate->Update();
	}
	camera->Update();
	player_->Update();

	skyBox->Update(wt.matWorld_ * MakeScaleMatrix({ 1000,1000,1000 }));

	CollisionCommon();

	for (auto& warpGate : warpGates) {
		if (IsCollisionAABB(player_->GetAABB(), warpGate->GetAABB())) {
			isfadeStart = true;
			break;
		}
	}

	wt.UpdateMatrix();
}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();
	skyBox->Draw();

	Object3dCommon::GetInstance()->Command();

	player_->Draw();

	for (auto& warpgate : warpGates) {
		warpgate->Draw();
	}

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

}

void SelectScene::Finalize() {
	delete player_;
	for (auto& warpgate : warpGates) {
		delete warpgate;
	}	
	delete camera;
	delete skyBox;
}