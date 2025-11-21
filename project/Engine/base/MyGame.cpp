#include "MyGame.h"

void MyGame::Initialize() {
	//フレームワーク処理
	Framework::Initialize();

	//ゲームシーン初期化
	gameScene_ = new SceneManager();
	gameScene_->Initialize();

	fadeScreen_ = FadeScreen::GetInstance();
	fadeScreen_->Initialize();
}

void MyGame::Update() {

	Framework::Update();

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance()->Begin();
#endif //  USE_IMGUI

	//ゲームシーン更新
	gameScene_->Update();

	//タイトル画面で終了を選択した時するとき
	if (gameScene_->SetGameEnd()) {
		Framework::SetIsEndRequst(gameScene_->SetGameEnd());
	}

	//フェード更新
	fadeScreen_->Update();
	
	//ポストエフェクト更新/変更
	PostEffectManager::GetInstance()->Update();

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance()->End();
#endif //  USE_IMGUI

}

void MyGame::Draw() {
	//描画開始
	DirectXCommon::GetInstance()->RenderTexturePreDraw();// 対 renderTexture
	
	//ゲームシーン描画
	gameScene_->Draw();

	DirectXCommon::GetInstance()->RenderTexturePostDraw();

	//描画開始
	DirectXCommon::GetInstance()->PreDraw();// 対 swapchain
	
	//フェード
	fadeScreen_->Draw();

	DirectXCommon::GetInstance()->FadePreDraw();

#ifdef  USE_IMGUI
	//ImGui描画処理
	ImGuiManager::GetInstance()->Draw();
#endif //  USE_IMGUI

	//描画終了
	DirectXCommon::GetInstance()->PostDraw();

}


void MyGame::Finalize() {
	delete gameScene_;
	fadeScreen_->Finalize();

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance()->Finalize();
#endif //  USE_IMGUI

	Framework::Finalize();
}
