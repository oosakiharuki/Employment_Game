/// -------------------------------
///
/// フレームワークの元
/// 
/// -------------------------------
#include "Framework.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Framework::Initialize() {
		//WinApp処理
		winApp_ = std::make_unique<WinApp>();
		winApp_->Initialize();

		//入力処理
		Input::GetInstance().Initialize(winApp_.get());

		//DirectX処理
		DirectXCommon::GetInstance().SetWinApp(winApp_.get());
		DirectXCommon::GetInstance().Initialize();

		//Srv処理
		SrvManager::GetInstance().Initialize();

		//スプライト(2d)の共通処理のまとめ
		SpriteCommons();

		//オブジェクト(3d)の共通処理のまとめ
		ObjectCommons();

		//ポストエフェクト共有処理
		PostEffectManager::GetInstance().Initialize();

		//音声共有処理
		Audio::GetInstance().Initialize();
	}

	void Framework::SpriteCommons() {
		//imGui処理
#ifdef USE_IMGUI
		ImGuiManager::GetInstance().Initialize(winApp_.get());
#endif // USE_IMGUI

		//スプライト共通処理
		SpriteCommon::GetInstance().Initialize();
		//テクスチャマネージャ初期化
		TextureManager::GetInstance().Initialize();
	}

	void Framework::ObjectCommons() {
		//オブジェクト(.obj)共有処理
		Object3dCommon::GetInstance().Initialize();

		//オブジェクト(.gltf)共有処理
		GLTFCommon::GetInstance().Initialize();

		//スキニング共有処理
		SkinningCommon::GetInstance().Initialize();

		//モデルマネージャ初期化
		ModelManager::GetInstance();

		//パーティクル共有処理
		ParticleCommon::GetInstance().Initialize();
		//パーティクルマネージャ初期化
		ParticleManager::GetInstance().InitializeParameter();

		//ワイヤーフレーム処理
		DebugWireframes::GetInstance().Initialize();

		//キューブマップ処理
		CubeMap::GetInstance().Initialize();
	}

	void Framework::Update() {
		//ウィンドウを閉じるまで終わらない
		if (winApp_->ProcessMessage()) {
			isRequest_ = true;
		}
		else {
			//ゲームの処理
			Input::GetInstance().Update();
		}
	}

	void Framework::Finalize() {
		//旧WinApp
		D3DResourceLeakChecker leakCheck;

		//delete input_;
		Input::GetInstance().Finalize();

		winApp_->Finalize();
		winApp_.reset();

		TextureManager::GetInstance().Finalize();
		ModelManager::GetInstance().Finalize();
		ParticleManager::GetInstance().Finalize();

		DirectXCommon::GetInstance().Finalize();
		SrvManager::GetInstance().Finalize();
		RtvManager::GetInstance().Finalize();
		SpriteCommon::GetInstance().Finalize();
		Object3dCommon::GetInstance().Finalize();
		GLTFCommon::GetInstance().Finalize();
		SkinningCommon::GetInstance().Finalize();
		ParticleCommon::GetInstance().Finalize();
		DebugWireframes::GetInstance().Finalize();
		CubeMap::GetInstance().Finalize();
		PostEffectManager::GetInstance().Finalize();
		SceneManager::GetInstance().Finalize();
		Audio::GetInstance().Finalize();
	}


	void Framework::Run() {

		//ゲーム初期化
		Initialize();

		//ウィンドウの×ボタンが押されるまでループ
		while (true) {
			//毎フレーム更新
			Update();

			//終了リクエスト
			if (IsEndRequest()) {
				break;
			}
			//描画処理
			Draw();
		}

		//ゲーム処理
		Finalize();
	}

}