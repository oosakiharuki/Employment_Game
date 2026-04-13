#include "GameOverScene.h"
#include "SceneManager.h"
#include "SelectScene.h"
#include "FadeScreen.h"
#include "NextStageSave.h"

void GameOverScene::Initialize() {

	levelEditor_.LoadLevelEditor("resource/LevelEditor/gameOver_setting.json");
	spitOut_.SetLevelEditor(&levelEditor_);

	//スプライト初期化処理
	InitSprite();
	//カメラ初期化処理
	InitCamera();
	//オブジェクト初期化処理
	InitObject();

	//フェードスタート
	FadeScreen::GetInstance().FadeStart(type_fadeOut);
}

void GameOverScene::InitSprite() {
	//ゲームオーバーロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameOver.png");
	sprite_->SetPosition(kSpritePositionGameOver_);

	//戻る
	spriteSpace_ = std::make_unique<Sprite>();
	spriteSpace_->Initialize("Moji_button.png");
	spriteSpace_->SetPosition(kSpritePositionButton_);
	spriteSpace_->SetSize(kSpriteSizeButton_);
}

void GameOverScene::InitCamera() {
	//カメラ設定
	camera_ = std::make_unique<Camera>();
	spitOut_.SpitOutCamera(cameraControl_);

	Object3dCommon::GetInstance().SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance().SetDefaultCamera(camera_.get());
	//ゴール、ワープフラグをリセット
	CollisionUtility::GetInstance().ResetFrag();
}

void GameOverScene::InitObject() {
	visualActors = std::move(spitOut_.SpitOutVisualActor());

	for (auto& visualActor : visualActors) {
		transforms_[visualActor->GetObjectName()] = visualActor->GetTransform();
		visualActor->LightOn();//ライト処理オン
	}
}


void GameOverScene::Update() {
	cameraControl_->Update(camera_.get());

	sprite_->Update();
	spriteSpace_->Update();

	//オブジェクト更新処理
	for (auto& visualActor : visualActors) {
		transforms_[visualActor->GetObjectName()].rotate.y += kRotate_;
		visualActor->SetTransform(transforms_[visualActor->GetObjectName()]);//座標更新
		visualActor->Update();
	}
}

void GameOverScene::Draw() {

	GLTFCommon::GetInstance().Command();

	//オブジェクト描画
	for (auto& visualActor : visualActors) {
		visualActor->Draw();
	}

	SpriteCommon::GetInstance().Command();

	sprite_->Draw();
	spriteSpace_->Draw();
}

void GameOverScene::Finalize() {}

void GameOverScene::SceneUpdate() {

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if ((Input::GetInstance().TriggerKey(DIK_SPACE) ||
		Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) && !FadeScreen::GetInstance().GetIsFading()) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<SelectScene>());
		FadeScreen::GetInstance().SetMaskTexture("fade01.png");
		FadeScreen::GetInstance().SetBackGround("fadeTexture.png");
	}

	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance().FadeStart(type_fadeIn);
	}
}