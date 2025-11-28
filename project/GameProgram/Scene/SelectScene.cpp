#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting("stage_select");

	//ステージの全体層
	stageobj_ = std::make_unique<Object3d>();
	stageobj_->Initialize();
	stageobj_->SetModelFile("stage_select.obj");

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープ、ゴールフラグのリセット
	CollisionManager::GetInstance()->ResetFlag();
}

void SelectScene::Update() {
	
	input_->JoystickUpdate(state_, preState_);

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	//ワープするflag && カメラがズームし終わった
	if (CollisionManager::GetInstance()->IsWarp() && cameraControl_->ZoomEnd()) {
		NextSceneFadeInStart("Game");
	}

	//ワープするflag && カメラがズームし終わった
	if (CollisionManager::GetInstance()->IsGoal() && cameraControl_->ZoomEnd()) {
		NextSceneFadeInStart("Clear");
	}

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	cameraControl_->Update(&*camera_);
	WarpNextScene();

	player_->Update();

	stageobj_->Update();

	CollisionCommon();

	spriteGuides_[kGuideMove_.name]->SetPosition(kSpriteTranslateMove_);
	spriteGuides_[kGuideWarp_.name]->SetPosition(kSpriteTranslateEkey_);

	//ガイド更新処理
	UpdateGuide();
}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();

	Object3dCommon::GetInstance()->Command();

	stageobj_->Draw();

	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}

	GLTFCommon::GetInstance()->Command();
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	SpriteCommon::GetInstance()->Command();
	//説明ガイド
	DrawGuide();
}

void SelectScene::Finalize() {}