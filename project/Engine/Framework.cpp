#include "Framework.h"

void Framework::Initialize() {

	winApp_ = new WinApp();
	winApp_->Initialize();


	input_ = Input::GetInstance();
	//GetHInstance()GetHwnd()を入れず直接winAppのクラスのものを使える
	input_->Initialize(winApp_);


	dxCommon = DirectXCommon::GetInstance();
	dxCommon->SetWinApp(winApp_);
	dxCommon->Initialize();

	srvManager = SrvManager::GetInstance();
	srvManager->Initialize(dxCommon);

	ImGuiManager::GetInstance()->Initialize(winApp_, dxCommon, srvManager);


	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon);
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);
	

	object3dCommon = Object3dCommon::GetInstance();
	object3dCommon->Initialize(dxCommon);
	
	glTFCommon = GLTFCommon::GetInstance();
	glTFCommon->Initialize(dxCommon);

	modelCommon = new ModelCommon();
	modelCommon->Initialize(dxCommon);
	ModelManager::GetInstance()->Initialize(dxCommon);


	particleCommon = ParticleCommon::GetInstance();
	particleCommon->Initialize(dxCommon);
	ParticleManager::GetInstance()->Initialize(dxCommon, srvManager);

	debugWireframes = DebugWireframes::GetInstance();
	debugWireframes->Initialize(dxCommon);

	cubemap = Cubemap::GetInstance();
	cubemap->Initialize(dxCommon);

	postEffectM = PostEffectManager::GetInstance();
	postEffectM->Initialize(dxCommon);

	audio_ = Audio::GetInstance();
	audio_->Initialize();
}

void Framework::Update() {
	if (winApp_->ProcessMessage()) {
		isRequst = true;
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

	dxCommon->Finalize();

	srvManager->Finalize();

	spriteCommon->Finalize();	
	object3dCommon->Finalize();
	glTFCommon->Finalize();
	delete modelCommon;
	
	particleCommon->Finalize();

	debugWireframes->Finalize();
	cubemap->Finalize();

	postEffectM->Finalize();

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

