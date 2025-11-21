#include "Framework.h"

void Framework::Initialize() {
	//WinApp処理
	winApp_ = new WinApp();
	winApp_->Initialize();

	//入力処理
	input_ = Input::GetInstance();
	//GetHInstance()GetHwnd()を入れず直接winAppのクラスのものを使える
	input_->Initialize(winApp_);

	//DirectX処理
	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->SetWinApp(winApp_);
	dxCommon_->Initialize();

	//Srv処理
	srvManager_ = SrvManager::GetInstance();
	srvManager_->Initialize(dxCommon_);

	//imugi処理
	ImGuiManager::GetInstance()->Initialize(winApp_, dxCommon_, srvManager_);

	//スプライト共通処理
	spriteCommon_ = SpriteCommon::GetInstance();
	spriteCommon_->Initialize(dxCommon_);
	//テクスチャマネージャ初期化
	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);
	
	//オブジェクト(.obj)共有処理
	object3dCommon_ = Object3dCommon::GetInstance();
	object3dCommon_->Initialize(dxCommon_);
	
	//オブジェクト(.gltf)共有処理
	glTFCommon_ = GLTFCommon::GetInstance();
	glTFCommon_->Initialize(dxCommon_);
	
	//スキニング共有処理
	skinningCommon_ = SkinningCommon::GetInstance();
	skinningCommon_->Initialize(dxCommon_);

	//モデル共有処理
	modelCommon_ = new ModelCommon();
	modelCommon_->Initialize(dxCommon_);
	//モデルマネージャ初期化
	ModelManager::GetInstance()->Initialize(dxCommon_);

	//パーティクル共有処理
	particleCommon_ = ParticleCommon::GetInstance();
	particleCommon_->Initialize(dxCommon_);
	//パーティクルマネージャ初期化
	ParticleManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	//ワイヤーフレーム処理
	debugWireframes_ = DebugWireframes::GetInstance();
	debugWireframes_->Initialize(dxCommon_);

	//キューブマップ処理
	cubemap_ = Cubemap::GetInstance();
	cubemap_->Initialize(dxCommon_);

	//ポストエフェクト共有処理
	postEffectM_ = PostEffectManager::GetInstance();
	postEffectM_->Initialize(dxCommon_);

	//音声共有処理
	audio_ = Audio::GetInstance();
	audio_->Initialize();
}

void Framework::Update() {
	//ウィンドウを閉じるまで終わらない
	if (winApp_->ProcessMessage()) {
		isRequst_ = true;
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
	delete winApp_;
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
	delete modelCommon_;
	particleCommon_->Finalize();
	debugWireframes_->Finalize();
	cubemap_->Finalize();
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
		if (IsEndRequst()) {
			break;
		}
		//描画処理
		Draw();
	}

	//ゲーム処理
	Finalize();
}

