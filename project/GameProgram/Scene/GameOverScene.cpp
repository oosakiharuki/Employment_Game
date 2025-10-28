#include "GameOverScene.h"

void GameOverScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Moji_GameOver.png");
	sprite->SetPosition({ 100,100 });

	camera = std::make_unique<Camera>();
	camera->SetTranslate({0,5,-15});
	camera->SetRotate({15.0f,0,0});

	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());

	player_gltf = std::make_unique<Object_glTF>();
	player_gltf->Initialize();
	player_gltf->SetModelFile("player_GameOver.gltf");

	stage_gltf = std::make_unique<Object_glTF>();
	stage_gltf->Initialize();
	stage_gltf->SetModelFile("gameover_stage.gltf");

	wt.Initialize();
	worldTransformCamera_.Initialize();
	
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void GameOverScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);
	
	sprite->Update();

	if (!FadeScreen::GetInstance()->GetIsFadeing()) {
		ChangeScene();
	}

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerBotton(state, preState, XINPUT_GAMEPAD_A)) {
		NextSceneFadeInStart("Game");
	}

	wt.rotation_.y += 0.5f;
	wt.UpdateMatrix();

	//ライトのスイッチ
	stage_gltf->LightSwitch(true);
	player_gltf->LightSwitch(true);

	player_gltf->Update(wt);
	stage_gltf->Update(wt);

	camera->Update();
}

void GameOverScene::Draw() {

	GLTFCommon::GetInstance()->Command();

	player_gltf->Draw();
	
	stage_gltf->Draw();

	SpriteCommon::GetInstance()->Command();

	sprite->Draw();

}

void GameOverScene::Finalize() {}