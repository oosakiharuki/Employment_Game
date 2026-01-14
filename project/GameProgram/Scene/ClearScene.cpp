#include "ClearScene.h"

using namespace MyMath;
using namespace UseEveryOne;

void ClearScene::Initialize() {
	//スプライト初期化処理
	InitSprite();
	//カメラ初期化処理
	InitCamera();
	//オブジェクト初期化処理
	InitObject();
	//パーティクル生成
	for (uint32_t i = 0; i < kParticleMaxNum_; i++) {
		//ファンファーレのパーティクル作成
		sceneParticles_[particleFanfare_.name] = ParticleManager::GetInstance()->InitParticle(particleFanfare_);
		//vectorに導入
		particleFanfares_.push_back(std::move(sceneParticles_[particleFanfare_.name]));
	}
	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void ClearScene::InitSprite() {
	//クリアロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameClear.png");
	sprite_->SetPosition(kSpritePositionGameClear_);

	//戻る
	spriteSpace_ = std::make_unique<Sprite>();
	spriteSpace_->Initialize("Moji_button.png");
	spriteSpace_->SetPosition(kSpritePositionButton_);
	spriteSpace_->SetSize(kSpriteSizeButton_);
}

void ClearScene::InitCamera() {
	camera_ = std::make_unique<Camera>();
	//カメラを設定
	cameraTranslate_ = kCameraTranslate_;
	camera_->SetTranslate(cameraTranslate_);
	camera_->SetRotate(cameraRotate_);

	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());

}

void ClearScene::InitObject() {
	playerGltf_ = std::make_unique<Object_glTF>();
	playerGltf_->Initialize();
	playerGltf_->SetModelFile("player_clear.gltf");

	wt_.Initialize();	
	//Transform更新処理
	transform_ = wt_.UpdateTransform();
	transform_.rotate.y = kPlayerFrontRange_;

	//地面
	stageGltf_ = std::make_unique<Object_glTF>();
	stageGltf_->Initialize();
	stageGltf_->SetModelFile("gameover_stage.gltf");
}

void ClearScene::Update() {

	input_->JoystickUpdate(state_, preState_);

	camera_->Update();
	//クリアロゴ更新
	sprite_->Update();	
	spriteSpace_->Update();

	wt_.UpdateMatrix(transform_);
	playerGltf_->Update(wt_);
	
	//初期値に戻す
	fanfareTranslate_ = kFanfareInitTranslate_;
	for (auto& gParticle : particleFanfares_) {
		gParticle->SetParticleBorn(ParticleBorn::TimerMode);
		gParticle->SetTranslate(fanfareTranslate_);
		gParticle->Update();
		//左->中央->右の順に配置
		fanfareTranslate_.x += kFanfareX_;
	}

	//ライトのスイッチオフ
	stageGltf_->LightSwitch(false);
	playerGltf_->LightSwitch(false);

	//周りのステージ
	stageGltf_->Update();

}

void ClearScene::Draw() {

	GLTFCommon::GetInstance()->Command();
	playerGltf_->Draw();
	stageGltf_->Draw();

	ParticleCommon::GetInstance()->Command();
	for (auto& particle : particleFanfares_) {
		particle->Draw();
	}

	SpriteCommon::GetInstance()->Command();
	//クリアロゴｗ描画
	sprite_->Draw();
	spriteSpace_->Draw();
}

void ClearScene::Finalize() {
	ParticleManager::GetInstance()->ResetParticle("clear_fanfare");
}

void ClearScene::SceneUpdate() {

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if ((Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerButton(state_, preState_, XINPUT_GAMEPAD_A)) && !FadeScreen::GetInstance()->GetIsFading()) {
		nextSceneNo_ = "Select";
	}

	ChangeSceneNo();
}