#include "ClearScene.h"
#include "SceneManager.h"
#include "SelectScene.h"
#include "FadeScreen.h"

using namespace MyMath;
using namespace UseEveryOne;

void ClearScene::Initialize() {

	levelEditor_.LoadLevelEditor("resource/LevelEditor/clear_setting.json");
	spitOut_.SetLevelEditor(&levelEditor_);

	//スプライト初期化処理
	InitSprite();
	//カメラ初期化処理
	InitCamera();
	//オブジェクト初期化処理
	InitObject();
	//パーティクル初期化処理
	InitParticle();
	//フェードスタート
	FadeScreen::GetInstance().FadeStart(type_fadeOut);
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
	spitOut_.SpitOutCamera(cameraControl_);

	Object3dCommon::GetInstance().SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance().SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance().SetDefaultCamera(camera_.get());
	//ゴール、ワープフラグをリセット
	CollisionUtility::GetInstance().ResetFrag();
}

void ClearScene::InitObject() {
	visualActors = std::move(spitOut_.SpitOutVisualActor());

	for (auto& visualActor : visualActors) {
		transforms_[visualActor->GetObjectName()] = visualActor->GetTransform();
		transforms_[visualActor->GetObjectName()].rotate.y = kPlayerFrontRange_;
	}
}

void ClearScene::InitParticle() {
	//パーティクル生成
	for (uint32_t i = 0; i < kParticleMaxNum_; i++) {
		//ファンファーレのパーティクル作成
		sceneParticles_[particleFanfare_] = ParticleManager::GetInstance().InitParticle(particleFanfare_);
		//vectorに導入
		particleFanfares_.push_back(std::move(sceneParticles_[particleFanfare_]));
	}

	//場所の設定
	for (auto& gParticle : particleFanfares_) {
		gParticle->SetTranslate(fanfareTranslate_);
		//左->中央->右の順に配置
		fanfareTranslate_.x += kFanfareX_;
	}
}

void ClearScene::Update() {
	cameraControl_->Update(camera_.get());
	//クリアロゴ更新
	sprite_->Update();	
	spriteSpace_->Update();

	for (auto& visualActor : visualActors) {
		visualActor->SetTransform(transforms_[visualActor->GetObjectName()]); 
		visualActor->Update();
	}
	
	for (auto& gParticle : particleFanfares_) {
		gParticle->SetParticleBorn(ParticleBorn::TimerMode);
		gParticle->Update();
	}
}

void ClearScene::Draw() {

	GLTFCommon::GetInstance().Command();
	for (auto& visualActor : visualActors) {
		visualActor->Draw();
	}

	ParticleCommon::GetInstance().Command();
	for (auto& particle : particleFanfares_) {
		particle->Draw();
	}

	SpriteCommon::GetInstance().Command();
	//クリアロゴｗ描画
	sprite_->Draw();
	spriteSpace_->Draw();
}

void ClearScene::Finalize() {
	ParticleManager::GetInstance().ResetParticle("clear_fanfare");
}

void ClearScene::SceneUpdate() {

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if ((Input::GetInstance().TriggerKey(DIK_SPACE) ||
		Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) && !FadeScreen::GetInstance().GetIsFading()) {
		SceneManager::GetInstance().ChangeScene(std::make_unique<SelectScene>());
	}

	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance().FadeStart(type_fadeIn);
	}
}