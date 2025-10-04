#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_select.json");

	skyBox = new BoxModel();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");
	wt.Initialize();

	stageobj = new Object3d();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_select.obj");

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

	if (isZumuIn) {
		if (zumuTimer <= 1.0f) {
			zumuTimer += 1.0f / 60.0f;

		}
		camera->Zumu(cameraSegment, zumuTimer);

		if (zumuTimer >= 1.0f) {
			isfadeStart = true;
		}
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}
	camera->Update();
	

	player_->Update();

	if (isZumuIn) {
		player_->IsAnimationOnlyUpdate();
		return;
	}

	skyBox->Update(wt.matWorld_ * MakeScaleMatrix({ 1000,1000,1000 }));
	stageobj->Update(wt);

	CollisionCommon();


	for (auto& stageObject : stageObjects) {
		if (stageObject == dynamic_cast<WarpGate*>(stageObject)) {
			WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject);
			if (IsCollisionAABB(player_->GetAABB(), warpGate->GetAABB()) && Input::GetInstance()->TriggerKey(DIK_E)) {
				isZumuIn = true;
				cameraSegment.origin = camera->GetTranslate();//ズーム前のカメラ位置
				cameraSegment.diff = player_->GetTranslate() + Vector3(0, 2, -15.0f);//プレイヤーよりちょっと離れてる
				break;
			}
		}
	}

	wt.UpdateMatrix();
}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();
	skyBox->Draw();

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

void SelectScene::Finalize() {
	delete player_;
	for (auto& stageObject : stageObjects) {
		delete stageObject;
	}	
	delete camera;
	delete skyBox;
	delete stageobj;
}