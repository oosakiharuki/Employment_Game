#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_select.json");

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_select.obj");

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void SelectScene::Update() {
	
	InputGamePad();

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	if (isZumuIn) {
		if (zumuTimer <= 1.0f) {
			zumuTimer += 1.0f / 60.0f;

		}
		camera->Zumu(cameraSegment, zumuTimer);

		if (zumuTimer >= 1.0f) {
			NextSceneFadeInStart("Game");
		}
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}
	camera->Update();
	

	player_->Update();

	if (isZumuIn) {
		player_->IsAnimationOnlyUpdate(true);
		player_->SetRotate({ 0,0,0 });//向きを前に
		return;
	}

	skyBox->Update(MakeScaleMatrix({ 1000,1000,1000 }));
	stageobj->Update();

	CollisionCommon();


	for (auto& stageObject : stageObjects) {
		//stageObjectsの中でワープゲートである場合
		if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get())) {
			WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
			//プレイヤーとワープゲートの当たり判定 + Eキーを押した時
			if (IsCollisionAABB(player_->GetAABB(), warpGate->GetAABB()) && Input::GetInstance()->TriggerKey(DIK_E)) {
				isZumuIn = true;
				cameraSegment.origin = camera->GetTranslate();//ズーム前のカメラ位置
				cameraSegment.diff = player_->GetTranslate() + Vector3(0, 2, -15.0f);//プレイヤーよりちょっと離れてる
				break;
			}
		}
	}

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
	stageObjects.clear();
}