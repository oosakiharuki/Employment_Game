#include "ClearScene.h"

using namespace MyMath;

void ClearScene::Initialize() {
	//クリアロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameClear.png");
	sprite_->SetPosition({ 100,100 });

	//戻る
	spriteSpace_ = std::make_unique<Sprite>();
	spriteSpace_->Initialize("Moji_botton.png");
	spriteSpace_->SetPosition(kSpritePosition_);
	spriteSpace_->SetSize(kSpriteSize_);

	camera_ = std::make_unique<Camera>();

	cameraTranslate_ = kCameraTranslate_;
	cameraRotate_ = { 0,0,0 };

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

	for (int32_t i = 0; i < 3; i++) {
		std::unique_ptr<Particle > gParticle;
		gParticle = std::make_unique<Particle>();
		gParticle->Initialize("clear_fanfare", "resource/Sprite/white.png", PrimitiveType::plane);
		gParticle->SetParticleBorn(ParticleBorn::MomentMode);
		gParticle->SetParticleMosion(ParticleMosion::Fanfare);
		gParticle->SetFrequency(1.5f);
		gParticle->SetParticleCount(10);
		particle_fanfares_.push_back(std::move(gParticle));
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
	for (auto& gParticle : particle_fanfares_) {
		gParticle->SetParticleBorn(ParticleBorn::TimerMode);
		gParticle->SetTranslate(fanfareTranslate_);
		gParticle->Update();
		//左->中央->右の順に配置
		fanfareTranslate_.x += kFanfareX_;
	}

	//周りのステージ
	stageGltf_->Update();

	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
	}

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
		Input::GetInstance()->TriggerBotton(state_,preState_,XINPUT_GAMEPAD_A) && !FadeScreen::GetInstance()->GetIsFadeing()) {
		NextSceneFadeInStart("Select");
	}

}

void ClearScene::Draw() {

	GLTFCommon::GetInstance()->Command();
	playerGltf_->Draw();
	stageGltf_->Draw();

	ParticleCommon::GetInstance()->Command();
	for (auto& gParticle : particle_fanfares_) {
		gParticle->Draw();
	}

	SpriteCommon::GetInstance()->Command();
	//クリアロゴｗ描画
	sprite_->Draw();
	spriteSpace_->Draw();
}

void ClearScene::Finalize() {
	ParticleManager::GetInstance()->ResetParticle("clear_fanfare");
}