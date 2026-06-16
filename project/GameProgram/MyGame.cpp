#include "MyGame.h"
#include "LoadingModels.h"
#include "UIManager.h"
#include "TimeScale.h"
#include <ShadowManager.h>

void MyGame::Initialize() {
	//フレームワークの初期化
	Framework::Initialize();

	//objectをローディング
	LoadingModels::GetInstance().LoadObjects();
	LoadingModels::GetInstance().Finalize();

	//フェード処理の初期化
	FadeScreen::GetInstance().Initialize();
	//ポーズ処理の初期化
	PauseScreen::GetInstance().Initialize();
	//最初のシーンの設定
	EngineLayer::SceneManager::GetInstance().ChangeScene(std::make_unique<TitleScene>());
	//シーンの更新処理(変更処理)
	EngineLayer::SceneManager::GetInstance().SceneUpdate();

	EngineLayer::ShadowManager::GetInstance().Initialize();
}

void MyGame::Update() {

#ifdef  USE_IMGUI
	EngineLayer::ImGuiManager::GetInstance().Begin();
#endif //  USE_IMGUI

	//フレームワークの更新
	Framework::Update();

	//フェード中は変更しない
	if (!FadeScreen::GetInstance().GetIsFading()) {
		//シーンの更新処理(変更処理)
		EngineLayer::SceneManager::GetInstance().SceneUpdate();
	}

	EngineLayer::SceneManager::GetInstance().Update();

	//タイトル画面で終了を選択した時するとき
	if (EngineLayer::SceneManager::GetInstance().SetGameEnd()) {
		Framework::SetIsEndRequest(EngineLayer::SceneManager::GetInstance().SetGameEnd());
	}

	//時間尺度の更新処理
	TimeScale::GetInstance().Update();

	EngineLayer::ShadowManager::GetInstance().Update();

	//フェード更新
	FadeScreen::GetInstance().Update();
	//スプライト更新処理
	UIManager::GetInstance().Update();
	//ポストエフェクト更新/変更
	EngineLayer::PostEffectManager::GetInstance().Update();

#ifdef  USE_IMGUI
	EngineLayer::ParticleManager::GetInstance().ParameterImGui();

	EngineLayer::ImGuiManager::GetInstance().End();
#endif //  USE_IMGUI

}

void MyGame::Draw() {
	//描画開始
	EngineLayer::DirectXCommon::GetInstance().RenderTexturePreDraw();// 対 renderTexture
	
	//ゲームシーン描画
	EngineLayer::SceneManager::GetInstance().Draw();
	
	EngineLayer::DirectXCommon::GetInstance().RenderTexturePostDraw();

	//描画開始
	EngineLayer::DirectXCommon::GetInstance().PreDraw();// 対 swapChain

	EngineLayer::SpriteCommon::GetInstance().Command();
	//説明ガイド
	UIManager::GetInstance().Draw();

	if (PauseScreen::GetInstance().IsPause()) {
		PauseScreen::GetInstance().Draw();
	}

	//フェード
	FadeScreen::GetInstance().Draw();

#ifdef  USE_IMGUI
	//ImGui描画処理
	EngineLayer::ImGuiManager::GetInstance().Draw();
#endif //  USE_IMGUI

	//描画終了
	EngineLayer::DirectXCommon::GetInstance().PostDraw();

	EngineLayer::ShadowManager::GetInstance().Reset();

}


void MyGame::Finalize() {
	FadeScreen::GetInstance().Finalize();
	EngineLayer::Audio::GetInstance().Finalize();
	EngineLayer::SceneManager::GetInstance().Finalize();
	NextStageSave::GetInstance().Finalize();
	CollisionManager::GetInstance().Finalize();
	UIManager::GetInstance().Finalize();
	EngineLayer::ParticleEmitter::GetInstance().Finalize();
	PauseScreen::GetInstance().Finalize();

	EngineLayer::ShadowManager::GetInstance().Finalize();

#ifdef  USE_IMGUI
	EngineLayer::ImGuiManager::GetInstance().Finalize();
#endif //  USE_IMGUI

	Framework::Finalize();
}
