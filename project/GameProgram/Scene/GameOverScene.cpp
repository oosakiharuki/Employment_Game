#include "GameOverScene.h"

void GameOverScene::Initialize() {
	//ゲームオーバーロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameOver.png");
	sprite_->SetPosition(kSpritePositionGameOver_);

	//戻る
	spriteSpace_ = std::make_unique<Sprite>();
	spriteSpace_->Initialize("Moji_botton.png");
	spriteSpace_->SetPosition(kSpritePositionBotton_);
	spriteSpace_->SetSize(kSpriteSizeBotton_);

	cameraTranslate_ = kCameraTranslate_;
	cameraRotate_ = kCameraRotate_;

	//カメラ設定
	camera_ = std::make_unique<Camera>();
	camera_->SetTranslate(cameraTranslate_);
	camera_->SetRotate(cameraRotate_);

	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());

	//残念そうなプレイヤーオブジェクト
	playerGltf_ = std::make_unique<Object_glTF>();
	playerGltf_->Initialize();
	playerGltf_->SetModelFile("player_GameOver.gltf");
	//地面
	stageGltf_ = std::make_unique<Object_glTF>();
	stageGltf_->Initialize();
	stageGltf_->SetModelFile("gameover_stage.gltf");

	wt_.Initialize();
	
	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void GameOverScene::Update() {

	input_->JoystickUpdate(state_, preState_);

	sprite_->Update();
	spriteSpace_->Update();

	wt_.rotation_.y += kRotate_;
	wt_.UpdateMatrix();

	//ライトのスイッチ
	stageGltf_->LightSwitch(true);
	playerGltf_->LightSwitch(true);

	//オブジェクト更新
	playerGltf_->Update(wt_);
	stageGltf_->Update(wt_);

	camera_->Update();
}

void GameOverScene::Draw() {

	GLTFCommon::GetInstance()->Command();

	//オブジェクト描画
	playerGltf_->Draw();
	stageGltf_->Draw();

	SpriteCommon::GetInstance()->Command();

	sprite_->Draw();
	spriteSpace_->Draw();
}

void GameOverScene::Finalize() {}

void GameOverScene::SceneUpdate() {
	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if ((Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_A)) && !FadeScreen::GetInstance()->GetIsFadeing()) {
		nextSceneNo_ = "Select";
		FadeScreen::GetInstance()->SetMaskTexture("fade01.png");
		FadeScreen::GetInstance()->SetBackGround("fadeTexture.png");
	}

	ChangeSceneNo();
}