#include "ClearScene.h"

void ClearScene::Initialize() {
	//クリアロゴ作成
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Moji_GameClear.png");
	sprite->SetPosition({ 100,100 });

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void ClearScene::Update() {

	input_->JoystickUpdate(state, preState);

	//クリアロゴ更新
	sprite->Update();

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
	SpriteCommon::GetInstance()->Command();
	//クリアロゴｗ描画
	sprite->Draw();
}

void ClearScene::Finalize() {}