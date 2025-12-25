#include "ClearScene.h"

using namespace MyMath;
using namespace UseEveryOne;

void ClearScene::Initialize() {
	//クリアロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameClear.png");
	sprite_->SetPosition(kSpritePositionGameClear_);

	//戻る
	spriteSpace_ = std::make_unique<Sprite>();
	spriteSpace_->Initialize("Moji_botton.png");
	spriteSpace_->SetPosition(kSpritePositionBotton_);
	spriteSpace_->SetSize(kSpriteSizeBotton_);

	camera_ = std::make_unique<Camera>();
	
	//カメラを設定
	cameraTranslate_ = kCameraTranslate_;

	camera_->SetTranslate(cameraTranslate_);
	camera_->SetRotate(cameraRotate_);

	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());

	playerGltf_ = std::make_unique<Object_glTF>();
	playerGltf_->Initialize();
	playerGltf_->SetModelFile("player_clear.gltf");

	wt_.Initialize();
	wt_.rotation_.y = kPlayerFrontRange_;

	//地面
	stageGltf_ = std::make_unique<Object_glTF>();
	stageGltf_->Initialize();
	stageGltf_->SetModelFile("gameover_stage.gltf");

	for (uint32_t i = 0; i < kParticleMaxNum_; i++) {
		//ファンファーレのパーティクル作成
		sceneParticles_[particleFanfare_.name] = ParticleManager::GetInstance()->InitParticle(particleFanfare_);
		//vectorに導入
		particleFanfares_.push_back(std::move(sceneParticles_[particleFanfare_.name]));
	}
	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void ClearScene::Update() {

	input_->JoystickUpdate(state_, preState_);

	camera_->Update();
	//クリアロゴ更新
	sprite_->Update();	
	spriteSpace_->Update();

	wt_.UpdateMatrix();
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
		Input::GetInstance()->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_A)) && !FadeScreen::GetInstance()->GetIsFadeing()) {
		nextSceneNo_ = "Select";
	}

	ChangeSceneNo();
}