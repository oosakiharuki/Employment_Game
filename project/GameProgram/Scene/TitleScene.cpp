#include "TitleScene.h"

using namespace MyMath;

void TitleScene::Initialize() {

	camera_ = std::make_unique<Camera>();
	cameraTranslate_ = { 0,0.0f,-30.0f };
	camera_->SetTranslate(cameraTranslate_);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());

	//プレイヤー初期化処理
	objectAutoPlayer_ = std::make_unique<Object_glTF>();
	objectAutoPlayer_->Initialize();
	objectAutoPlayer_->SetModelFile("NewPlayer.gltf");
	objectAutoPlayer_->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//playerワールド座標系
	wts[0].Initialize();
	wts[0].rotation_ = { 0,180,0 };
	wts[0].translation_ = { -4.5f,10.0f,0 };

	//影
	playerShadow_ = std::make_unique<Shadow>();
	playerShadow_->Initialize();

	//手動
	Vector3 shadowPos = wts[0].translation_;
	shadowPos.y = -3.8f;
	shadowPos.y += 0.01f;//少し上に上げてにしない

	playerShadow_->SetTranslate(shadowPos);

	//傘の初期化
	objectUmbrella_ = std::make_unique<Object_glTF>();
	objectUmbrella_->Initialize();
	objectUmbrella_->SetModelFile("umbrella_Open.gltf");
	objectUmbrella_->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//umbrellaワールド座標系
	wts[1].Initialize();
	wts[1].parent_ = &wts[0];//プレイヤーを親としてついていく
	wts[1].translation_ = { 0,2,0 };
	wts[1].rotation_ = { -90.0f,0,0 };

	//ゲームスタート文字の初期化
	objectMojiStart_ = std::make_unique<Object_glTF>();
	objectMojiStart_->Initialize();
	objectMojiStart_->SetModelFile("Select_Start.gltf");
	objectMojiStart_->SetEnvironment("resource/rostock_laage_airport_4k.dds");


	//ゲームスタート文字のワールド座標系
	wts[2].Initialize();
	wts[2].rotation_.y = rotateSelectMoji_;
	wts[2].translation_ = { 3,-0.5f,0 };
	wts[2].scale_ = { 1.5f,1.5f,1.5f };

	//ゲーム終了文字の初期化
	objectMojiEnd_ = std::make_unique<Object_glTF>();
	objectMojiEnd_->Initialize();
	objectMojiEnd_->SetModelFile("Select_End.gltf");
	objectMojiEnd_->SetEnvironment("resource/rostock_laage_airport_4k.dds");
	
	//ゲーム終了文字のワールド座標系
	wts[3].Initialize();
	wts[3].rotation_.y = rotateSelectMoji_;
	wts[3].translation_ = { 3,-2.5,0 };
	wts[3].scale_ = { 1.5f,1.5f,1.5f };

	//ステージ全体のオブジェクト初期化
	objectStage_ = std::make_unique<Object_glTF>();
	objectStage_->Initialize();
	objectStage_->SetModelFile("Title_stage.gltf");
	objectStage_->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//タイトル名スプライトの初期化
	spriteMojiTitle_ = std::make_unique<Sprite>();
	spriteMojiTitle_->Initialize("Moji_Title.png");
	spriteMojiTitle_->SetPosition(titlePos_);

	//パーティクル初期化
	particleBullet_ = std::make_unique<Particle>();
	particleBullet_->Initialize("title_bullet", "resource/Sprite/cone.png", PrimitiveType::cone);
	particleBullet_->SetParticleCount(1);
	particleBullet_->SetParticleMosion(ParticleMosion::Fixed);
	particleBullet_->SetFrequency(0.5f);

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void TitleScene::Update() {
	//次のシーンに移動
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		if (wts[1].translation_.y == wts[2].translation_.y) {
			ChangeScene();
		}
		else if (wts[1].translation_.y == wts[3].translation_.y) {
			isGameEnd_ = true;
		}
	}

	//ゲームパット用操作処理設定
	input_->JoystickUpdate(state_, preState_);

	//カメラ更新
	camera_->Update();

	//プレイヤーが降ってくるところ
	if (wts[0].translation_.y <= -2.0f) {
		//座標を維持
		wts[0].translation_.y = -2.0f;
		//Maxになるまでタイマーを進ませる
		if (timer_ < kTimeMax_) {
			timer_ += 1.0f / 60.0f;
		}
		else {
			timer_ = kTimeMax_;
			move_ += 0.01f;
		}
		//タイトルが上からくる
		start_.y = end_.y + EaseOut(start_.y, timer_, kTimeMax_);
		titlePos_.y = start_.y + 10.0f * std::sin(move_);

	}
	else {
		//重力でゆっくり落ちる
		wts[0].translation_.y -= gravity_;
	}

	//タイトル更新処理
	spriteMojiTitle_->SetPosition(titlePos_);//常時場所設定
	spriteMojiTitle_->Update();

	if (isbullet) {
		if (wts[1].translation_.y == wts[2].translation_.y) {
			wts[2].translation_.x += 0.5f;
		}
		else if (wts[1].translation_.y == wts[3].translation_.y) {
			wts[3].translation_.x += 0.5f;
		}

		bulletTimer_ += 1.0f / 60.0f;
		if (bulletTimer_ >= kBulletTimeMax_) {
			NextSceneFadeInStart("Select");
		}
	}

	//フェードが終わったら押せる
	if (timer_ >= kTimeMax_ && !isbullet) {

		//選択できるようになったら傘を親子関係をなくす
		if (wts[1].parent_) {
			wts[1].parent_ = nullptr;
			wts[1].rotation_ = { 0,90,0 };
			wts[1].translation_ = { -1.0f,wts[2].translation_.y,0 };
			objectUmbrella_->ChangeAnimation("umbrella_Close.gltf");
			objectUmbrella_->SetEnvironment("resource/rostock_laage_airport_4k.dds");
		}

		wts[1].translation_.x = -1.0f + std::sin(move_) / 4.0f;


		if (wts[2].rotation_.y <= 0.0f && wts[3].rotation_.y <= 0.0f) {
			wts[2].rotation_.y = 0.0f;
			wts[3].rotation_.y = 0.0f;
		}
		else {
			wts[2].rotation_.y -= 30.0f;
			wts[3].rotation_.y -= 30.0f;
		}

		//キーボード操作

		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			wts[1].translation_.y = wts[2].translation_.y;//ゲームスタート
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			wts[1].translation_.y = wts[3].translation_.y;//ゲーム終了
		}

		//ゲームパット操作
		
		if (Input::GetInstance()->GetJoystickState(0, state_)) {
			//スティックの傾き度
			float padY = static_cast<float>(state_.Gamepad.sThumbLY) / 32768.0f;

			if (padY > 0.5f) {
				wts[1].translation_.y = wts[2].translation_.y;//ゲームスタート
			}
			else if (padY < -0.5f) {
				wts[1].translation_.y = wts[3].translation_.y;//ゲーム終了
			}
		}

		//spaceもしくはAボタンを押したら実行
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
			Input::GetInstance()->TriggerBotton(state_,preState_, XINPUT_GAMEPAD_A)) {
			isbullet = true;
			particleBullet_->ChangeMode(BornParticle::MomentMode);
			particleBullet_->SetTranslate(wts[1].translation_);
			particleBullet_->SetRotate({0.0f,0.0f,-90.0f});
		}
	}
	//パーティクル更新
	particleBullet_->Update();

#ifdef USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &cameraTranslate_.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &cameraTranslate_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &cameraRotate_.x);
	ImGui::SliderFloat("cameraRotateX", &cameraRotate_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &cameraRotate_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &cameraRotate_.z, -360.0f, 360.0f);
	
	camera_->SetTranslate(cameraTranslate_);
	camera_->SetRotate(cameraRotate_);

	ImGui::End();

#endif // USE_IMGUI

	//ワールド座標系更新
	for (uint32_t i = 0; i < 4; i++) {
		wts[i].UpdateMatrix();
	}

	objectAutoPlayer_->Update(wts[0]);
	objectUmbrella_->Update(wts[1]);

	objectMojiStart_->Update(wts[2]);
	objectMojiEnd_->Update(wts[3]);

	objectStage_->Update();

	playerShadow_->Update();
}

void TitleScene::Draw() {

	Object3dCommon::GetInstance()->Command();

	playerShadow_->Draw();

	GLTFCommon::GetInstance()->Command();

	objectUmbrella_->Draw();
	objectAutoPlayer_->Draw();	
	objectMojiStart_->Draw();
	objectMojiEnd_->Draw();
	objectStage_->Draw();

	SkinningCommon::GetInstance()->Command();

	SpriteCommon::GetInstance()->Command();

	spriteMojiTitle_->Draw();

	ParticleCommon::GetInstance()->Command();

	particleBullet_->Draw();
}

void TitleScene::Finalize() {}