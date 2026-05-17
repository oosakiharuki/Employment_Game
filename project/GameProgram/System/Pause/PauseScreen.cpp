#include "PauseScreen.h"
#include "UseEveryOne.h"
#include <algorithm>

using namespace MyMath;
using namespace UseEveryOne;

std::unique_ptr<PauseScreen> PauseScreen::sInstance_ = nullptr;

PauseScreen& PauseScreen::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<PauseScreen>();
	}
	return *sInstance_;
}

void PauseScreen::Initialize() {
	backScreen_ = std::make_unique<EngineLayer::Sprite>();
	backScreen_->Initialize("pauseScreen.png");

	selectPoint_ = std::make_unique<EngineLayer::Sprite>();
	selectPoint_->Initialize("pauseSelect.png");

	spriteGuide_ = std::make_unique<EngineLayer::Sprite>();
	spriteGuide_->Initialize("guideList.png");

	spriteSelectGuide_ = std::make_unique<EngineLayer::Sprite>();
	spriteSelectGuide_->Initialize("pauseGuide.png");

	spriteSelectReturn_ = std::make_unique<EngineLayer::Sprite>();
	spriteSelectReturn_->Initialize("pauseReturnGame.png");

	spriteSelectSceneChange_ = std::make_unique<EngineLayer::Sprite>();

	ResetPauseSprite();

	pauseState_ = std::make_unique<PauseMove>();
}

void PauseScreen::ResetPauseSprite() {
	backScreen_->SetPosition({ 0,0 });
	spriteGuide_->SetPosition({ 64,32 });

	spriteSelectGuide_->SetPosition(kSelectGuideStartPosition_);
	spriteSelectReturn_->SetPosition(kSelectReturnStartPosition_);
	spriteSelectSceneChange_->SetPosition(kSelectSceneChangeStartPosition_);

	interpolation_ = 0.0f;

}

void PauseScreen::BeforeChangeScene(const std::string& textureName, std::unique_ptr<EngineLayer::BaseScene> changeScene) {
	if (spriteSelectSceneChange_ != nullptr) {
		spriteSelectSceneChange_.reset();
	}
	
	spriteSelectSceneChange_ = std::make_unique<EngineLayer::Sprite>();
	spriteSelectSceneChange_->Initialize(textureName);

	ResetPauseSprite();

	nextScene_ = std::move(changeScene);
}


void PauseScreen::Update() {
	isPause_ = true;

	//スプライトがスライド移動
	pauseState_->Update(*this);

	selectPoint_->SetPosition(select_);

	if (isSelect_) {
		SelectResult();
		ResetPauseSprite();

		isSelect_ = false;

		//座標を更新するため
		spriteSelectGuide_->Update();
		spriteSelectReturn_->Update();
		spriteSelectSceneChange_->Update();
	}

	backScreen_->Update();
	selectPoint_->Update();
}

void PauseScreen::MoveSprite() {
	interpolation_ += kDeltaTime_ * 2.0f;
	interpolation_ = std::clamp(interpolation_, 0.0f, 1.0f);

	moveSelectReturn_ = spriteSelectReturn_->GetPosition();
	moveSelectGuide_ = spriteSelectGuide_->GetPosition();
	moveSelectSceneChange_ = spriteSelectSceneChange_->GetPosition();

	moveSelectReturn_ = EaseOut(kSelectReturnEndPosition_, kSelectReturnStartPosition_, interpolation_);
	moveSelectGuide_ = EaseOut(kSelectGuideEndPosition_, kSelectGuideStartPosition_, interpolation_);
	moveSelectSceneChange_ = EaseOut(kSelectSceneChangeEndPosition_, kSelectSceneChangeStartPosition_, interpolation_);

	spriteSelectReturn_->SetPosition(moveSelectReturn_);
	spriteSelectGuide_->SetPosition(moveSelectGuide_);
	spriteSelectSceneChange_->SetPosition(moveSelectSceneChange_);
	
	spriteSelectGuide_->Update();
	spriteSelectReturn_->Update();
	spriteSelectSceneChange_->Update();
}


void PauseScreen::UpdateGuide() {
	//コントローラーが接続されているなら
	if (EngineLayer::Input::GetInstance().GetJoystickState()) {
		spriteGuide_->SetTextureFile("guideList2.png");
	}
	else {
		spriteGuide_->SetTextureFile("guideList.png");
	}

	spriteGuide_->Update();
}

void PauseScreen::SelectMode() {
	//選択方法
	if (EngineLayer::Input::GetInstance().GetActiveGamePad()) {
		SelectGamePad();//ゲームパッド
	}
	else {
		SelectKeyBoard();//キーボード
	}

	//選択している項目に緑枠を入れる
	if (selectNumber == 0) {
		select_ = kSelectReturnEndPosition_;
	}
	else if (selectNumber == 1) {
		select_ = kSelectGuideEndPosition_;
	}
	else if (selectNumber == 2) {
		select_ = kSelectSceneChangeEndPosition_;
	}

	if ((EngineLayer::Input::GetInstance().TriggerKey(DIK_SPACE) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) && interpolation_ >= 1.0f) {
		isSelect_ = true;
	}

	if ((EngineLayer::Input::GetInstance().TriggerKey(DIK_ESCAPE) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_START)) && interpolation_ >= 1.0f) {
		select_ = kSelectReturnEndPosition_;
		isSelect_ = true;
	}
}

void PauseScreen::SelectGamePad() {
	if (EngineLayer::Input::GetInstance().LeftStickY() >= -0.5f && EngineLayer::Input::GetInstance().LeftStickY() <= 0.5f) {
		isMoveStick_ = false;//傾きを直した
	}
	//一度傾けを戻さないと進まない
	if (isMoveStick_) return;

	//上に傾けた場合
	if (EngineLayer::Input::GetInstance().LeftStickY() > 0.5f && selectNumber > 0) {
		selectNumber--;
		isMoveStick_ = true;
	}//下に傾けた場合
	else if (EngineLayer::Input::GetInstance().LeftStickY() < -0.5f && selectNumber < 2) {
		selectNumber++;
		isMoveStick_ = true;
	}
}

void PauseScreen::SelectKeyBoard() {
	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_W) && selectNumber > 0) {
		selectNumber--;
	}
	else if (EngineLayer::Input::GetInstance().TriggerKey(DIK_S) && selectNumber < 2) {
		selectNumber++;
	}
}

void PauseScreen::SelectResult() {
	if (select_.y == kSelectReturnEndPosition_.y) {
		isPause_ = false;
		ResetPauseSprite();
		selectNumber = 0;
	}
	else if (select_.y == kSelectGuideEndPosition_.y) {
		ChangePauseState(std::make_unique<PauseSelectGuide>());
		ResetPauseSprite();
	}
	else if (select_.y == kSelectSceneChangeEndPosition_.y) {
		ChangePauseState(std::make_unique<PauseSelectSceneChange>());
		selectNumber = 0;
	}
}

void PauseScreen::Draw() {
	backScreen_->Draw();
	selectPoint_->Draw();

	pauseState_->Draw(*this);
}

void PauseScreen::Finalize() {
	sInstance_.reset();
}

void PauseScreen::DrawGuide() {
	spriteGuide_->Draw();
}

void PauseScreen::DrawSelectMode() {
	spriteSelectReturn_->Draw();
	spriteSelectGuide_->Draw();
	spriteSelectSceneChange_->Draw();
}

void PauseScreen::ChangePauseState(std::unique_ptr<BasePauseState> nextState) {
	pauseState_.reset();//一度リセット
	pauseState_ = std::move(nextState); //変更する
}