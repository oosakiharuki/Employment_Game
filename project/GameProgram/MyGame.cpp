#include "MyGame.h"
#include "LoadingModels.h"

void MyGame::Initialize() {
	//フレームワーク処理
	Framework::Initialize();

	//ゲームシーン初期化

	//objectをローディング
	LoadingModels::GetInstance().LoadObjects();
	LoadingModels::GetInstance().Finalize();

	FadeScreen::GetInstance().Initialize();

	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance().SetSceneFactory(sceneFactory_);

	SceneManager::GetInstance().ChangeScene("Title");
	//シーンの更新処理(変更処理)
	SceneManager::GetInstance().SceneUpdate();
}

void MyGame::Update() {

	Framework::Update();

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance().Begin();
#endif //  USE_IMGUI

	//ゲームシーン更新
	
	//フェード中は変更しない
	if (!FadeScreen::GetInstance().GetIsFading()) {
		//シーンの更新処理(変更処理)
		SceneManager::GetInstance().SceneUpdate();
	}

	SceneManager::GetInstance().Update();

	//タイトル画面で終了を選択した時するとき
	if (SceneManager::GetInstance().SetGameEnd()) {
		Framework::SetIsEndRequest(SceneManager::GetInstance().SetGameEnd());
	}

	//フェード更新
	FadeScreen::GetInstance().Update();
	
	//ポストエフェクト更新/変更
	PostEffectManager::GetInstance().Update();

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance().End();
#endif //  USE_IMGUI

}

void MyGame::Draw() {
	//描画開始
	DirectXCommon::GetInstance().RenderTexturePreDraw();// 対 renderTexture
	
	//ゲームシーン描画
	SceneManager::GetInstance().Draw();

	DirectXCommon::GetInstance().RenderTexturePostDraw();

	//描画開始
	DirectXCommon::GetInstance().PreDraw();// 対 swapChain
	
	//フェード
	FadeScreen::GetInstance().Draw();

	DirectXCommon::GetInstance().FadePreDraw();

#ifdef  USE_IMGUI
	//ImGui描画処理
	ImGuiManager::GetInstance().Draw();
#endif //  USE_IMGUI

	//描画終了
	DirectXCommon::GetInstance().PostDraw();

}


void MyGame::Finalize() {
	FadeScreen::GetInstance().Finalize();
	SceneManager::GetInstance().Finalize();
	delete sceneFactory_;

#ifdef  USE_IMGUI
	ImGuiManager::GetInstance().Finalize();
#endif //  USE_IMGUI

	Framework::Finalize();
}
