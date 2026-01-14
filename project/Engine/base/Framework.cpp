#include "Framework.h"

void Framework::Initialize() {
	//WinApp処理
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	//入力処理
	input_ = Input::GetInstance().get();
	input_->Initialize(winApp_.get());

	//DirectX処理
	dxCommon_ = DirectXCommon::GetInstance().get();
	dxCommon_->SetWinApp(winApp_.get());
	dxCommon_->Initialize();

	//Srv処理
	srvManager_ = SrvManager::GetInstance().get();
	srvManager_->Initialize(dxCommon_);

	//スプライト(2d)の共通処理のまとめ
	SpriteCommons();

	//オブジェクト(3d)の共通処理のまとめ
	ObjectCommons();

	//ポストエフェクト共有処理
	postEffectM_ = PostEffectManager::GetInstance().get();
	postEffectM_->Initialize(dxCommon_);

	//音声共有処理
	audio_ = Audio::GetInstance().get();
	audio_->Initialize();
}

void Framework::SpriteCommons() {
	//imGui処理
	ImGuiManager::GetInstance()->Initialize(winApp_.get(), dxCommon_, srvManager_);

	//スプライト共通処理
	spriteCommon_ = SpriteCommon::GetInstance().get();
	spriteCommon_->Initialize(dxCommon_);
	//テクスチャマネージャ初期化
	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);
}

void Framework::ObjectCommons() {
	//オブジェクト(.obj)共有処理
	object3dCommon_ = Object3dCommon::GetInstance().get();
	object3dCommon_->Initialize(dxCommon_);

	//オブジェクト(.gltf)共有処理
	glTFCommon_ = GLTFCommon::GetInstance().get();
	glTFCommon_->Initialize(dxCommon_);

	//スキニング共有処理
	skinningCommon_ = SkinningCommon::GetInstance().get();
	skinningCommon_->Initialize(dxCommon_);

	//モデル共有処理
	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(dxCommon_);
	//モデルマネージャ初期化
	ModelManager::GetInstance()->Initialize(dxCommon_);

	//パーティクル共有処理
	particleCommon_ = ParticleCommon::GetInstance().get();
	particleCommon_->Initialize(dxCommon_);
	//パーティクルマネージャ初期化
	ParticleManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	//ワイヤーフレーム処理
	debugWireframes_ = DebugWireframes::GetInstance().get();
	debugWireframes_->Initialize(dxCommon_);

	//キューブマップ処理
	cubeMap_ = CubeMap::GetInstance().get();
	cubeMap_->Initialize(dxCommon_);
}

void Framework::Update() {
	//ウィンドウを閉じるまで終わらない
	if (winApp_->ProcessMessage()) {
		isRequest_ = true;
	}
	else {
		//ゲームの処理
		input_->Update();
	}
}

void Framework::Finalize() {
	//旧WinApp
	D3DResourceLeakChecker leakCheck;

	//delete input_;
	input_->Finalize();

	winApp_->Finalize();
	winApp_.reset();
	winApp_ = nullptr;

	TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	ParticleManager::GetInstance()->Finalize();

	dxCommon_->Finalize();
	srvManager_->Finalize();
	spriteCommon_->Finalize();
	object3dCommon_->Finalize();
	glTFCommon_->Finalize();
	skinningCommon_->Finalize();
	modelCommon_.reset();
	particleCommon_->Finalize();
	debugWireframes_->Finalize();
	cubeMap_->Finalize();
	postEffectM_->Finalize();
	audio_->Finalize();
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

