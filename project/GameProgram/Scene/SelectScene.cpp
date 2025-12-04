#include "SelectScene.h"
using namespace MyMath;

void SelectScene::Initialize() {

	//移行データ初期化
	sceneSaveData_.nextStageFile = "stage_select";

	//ステージシーンのゲームオブジェクト配置
	LevelEditorObjectSetting(sceneSaveData_.nextStageFile);

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
	
	//ゲームパットの更新
	input_->JoystickUpdate(state_, preState_);

	//フェーズインが完了した時
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//ゲームシーンに移動
		ChangeScene();
	}

	//タイトルに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		NextSceneFadeInStart("Title");
	}

	//ステージオブジェクト更新
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	//カメラコントロールの更新
	cameraControl_->Update(&*camera_);

	//ワープしてゲームシーンに移動
	WarpNextScene("Game");

	//プレイヤー更新
	player_->Update();
	
	//ステージ更新
	stageobj_->Update();

	//当たり判定
	CollisionCommon();

	//表示する操作説明
	spriteGuides_[kGuideMove_.name]->SetPosition(kSpriteTranslateMove_);
	spriteGuides_[kGuideWarp_.name]->SetPosition(kSpriteTranslateEkey_);

	//ガイド更新処理
	UpdateGuide();
}

void SelectScene::Draw() {

	Cubemap::GetInstance()->Command();

	Object3dCommon::GetInstance()->Command();

	//ステージ描画
	stageobj_->Draw();

	//ステージオブジェクト描画
	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}

	GLTFCommon::GetInstance()->Command();
	//プレイヤー描画
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	//プレイヤーパーティクル描画
	player_->DrawP();

	SpriteCommon::GetInstance()->Command();
	//説明ガイド
	DrawGuide();
}

void SelectScene::Finalize() {}