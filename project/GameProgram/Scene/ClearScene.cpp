#include "ClearScene.h"

using namespace MyMath;

void ClearScene::Initialize() {
	//クリアロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameClear.png");
	sprite_->SetPosition({ 100,100 });

	//戻る
	sprite_space = std::make_unique<Sprite>();
	sprite_space->Initialize("Moji_botton.png");
	sprite_space->SetPosition({ 800,10 });
	sprite_space->SetSize({ 256,64 });

	camera = std::make_unique<Camera>();

	cameraTranslate = { 0,2.0f,-18.0f };
	cameraRotate = { 0,0.0f,0 };

	camera->SetTranslate(cameraTranslate);
	camera->SetRotate(cameraRotate);

	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera.get());

	player_gltf_ = std::make_unique<Object_glTF>();
	player_gltf_->Initialize();
	player_gltf_->SetModelFile("player_clear.gltf");

	wt_.Initialize();
	wt_.rotation_.y = 180.0f;

	//地面
	stage_gltf_ = std::make_unique<Object_glTF>();
	stage_gltf_->Initialize();
	stage_gltf_->SetModelFile("gameover_stage.gltf");

	for (int32_t i = 0; i < 3; i++) {
		std::unique_ptr<Particle > gParticle;
		gParticle = std::make_unique<Particle>();
		gParticle->Initialize("clear_fanfare", "resource/Sprite/white.png", PrimitiveType::plane);
		gParticle->ChangeMode(BornParticle::MomentMode);
		gParticle->SetParticleMosion(ParticleMosion::Fanfare);
		gParticle->SetFrequency(1.5f);
		gParticle->SetParticleCount(10);
		particle_fanfares_.push_back(std::move(gParticle));
	}
	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void ClearScene::Update() {

	input_->JoystickUpdate(state, preState);

	camera->Update();
	//クリアロゴ更新
	sprite_->Update();	
	sprite_space->Update();

	wt_.UpdateMatrix();
	player_gltf_->Update(wt_);

	Vector3 gTranslate = { -3,2,0 };
	for (auto& gParticle : particle_fanfares_) {
		if (setFrequencyTime_) {
			gParticle->ChangeMode(BornParticle::TimerMode);
		}
		gParticle->SetTranslate(gTranslate);
		gParticle->Update();

		gTranslate.x += 3.0f;
	}

	stage_gltf_->Update();


	setFrequencyTime_ = true;

	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
	}

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
		Input::GetInstance()->TriggerBotton(state,preState,XINPUT_GAMEPAD_A) && !FadeScreen::GetInstance()->GetIsFadeing()) {
		NextSceneFadeInStart("Select");
	}

}

void ClearScene::Draw() {

	GLTFCommon::GetInstance()->Command();
	player_gltf_->Draw();
	stage_gltf_->Draw();

	ParticleCommon::GetInstance()->Command();
	for (auto& gParticle : particle_fanfares_) {
		gParticle->Draw();
	}

	SpriteCommon::GetInstance()->Command();
	//クリアロゴｗ描画
	sprite_->Draw();
	sprite_space->Draw();
}

void ClearScene::Finalize() {
	ParticleManager::GetInstance()->ResetParticle("clear_fanfare");
}