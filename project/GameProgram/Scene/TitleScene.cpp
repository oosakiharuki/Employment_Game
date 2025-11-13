#include "TitleScene.h"

using namespace MyMath;

void TitleScene::Initialize() {

	camera = std::make_unique<Camera>();
	cameraTranslate = { 0,0.0f,-30.0f };
	camera->SetTranslate(cameraTranslate);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera.get());

	//プレイヤー初期化処理
	object_autoPlayer = std::make_unique<Object_glTF>();
	object_autoPlayer->Initialize();
	object_autoPlayer->SetModelFile("NewPlayer.gltf");
	object_autoPlayer->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//playerワールド座標系
	wts[0].Initialize();
	wts[0].rotation_ = { 0,180,0 };
	wts[0].translation_ = { -4.5f,10.0f,0 };

	//影
	player_shadow = std::make_unique<Shadow>();
	player_shadow->Initialize();

	//手動
	Vector3 shadowPos = wts[0].translation_;
	shadowPos.y = -3.8f;
	shadowPos.y += 0.01f;//少し上に上げてにしない

	player_shadow->SetTranslate(shadowPos);

	//傘の初期化
	umbrella = std::make_unique<Object_glTF>();
	umbrella->Initialize();
	umbrella->SetModelFile("umbrella_Open.gltf");
	umbrella->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//umbrellaワールド座標系
	wts[1].Initialize();
	wts[1].parent_ = &wts[0];//プレイヤーを親としてついていく
	wts[1].translation_ = { 0,2,0 };
	wts[1].rotation_ = { -90.0f,0,0 };

	//ゲームスタート文字の初期化
	object_Moji_Start = std::make_unique<Object_glTF>();
	object_Moji_Start->Initialize();
	object_Moji_Start->SetModelFile("Select_Start.gltf");
	object_Moji_Start->SetEnvironment("resource/rostock_laage_airport_4k.dds");


	//ゲームスタート文字のワールド座標系
	wts[2].Initialize();
	wts[2].rotation_.y = rotate_select_moji;
	wts[2].translation_ = { 3,-0.5f,0 };
	wts[2].scale_ = { 1.5f,1.5f,1.5f };

	//ゲーム終了文字の初期化
	object_Moji_End = std::make_unique<Object_glTF>();
	object_Moji_End->Initialize();
	object_Moji_End->SetModelFile("Select_End.gltf");
	object_Moji_End->SetEnvironment("resource/rostock_laage_airport_4k.dds");
	
	//ゲーム終了文字のワールド座標系
	wts[3].Initialize();
	wts[3].rotation_.y = rotate_select_moji;
	wts[3].translation_ = { 3,-2.5,0 };
	wts[3].scale_ = { 1.5f,1.5f,1.5f };

	//ステージ全体のオブジェクト初期化
	object_stage = std::make_unique<Object_glTF>();
	object_stage->Initialize();
	object_stage->SetModelFile("Title_stage.gltf");
	object_stage->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	//タイトル名スプライトの初期化
	sprite_Moji_Title = std::make_unique<Sprite>();
	sprite_Moji_Title->Initialize("Moji_Title.png");
	sprite_Moji_Title->SetPosition(Title_pos);

	//パーティクル初期化
	bullet_particle = std::make_unique<Particle>();
	bullet_particle->Initialize("title_bullet", "resource/Sprite/cone.png", PrimitiveType::cone);
	bullet_particle->SetParticleCount(1);
	bullet_particle->SetParticleMosion(ParticleMosion::Fixed);
	bullet_particle->SetFrequency(0.5f);

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void TitleScene::Update() {
	//次のシーンに移動
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		if (wts[1].translation_.y == wts[2].translation_.y) {
			ChangeScene();
		}
		else if (wts[1].translation_.y == wts[3].translation_.y) {
			isGameEnd = true;
		}
	}

	//ゲームパット用操作処理設定
	input_->JoystickUpdate(state, preState);

	//カメラ更新
	camera->Update();

	//プレイヤーが降ってくるところ
	if (wts[0].translation_.y <= -2.0f) {
		//座標を維持
		wts[0].translation_.y = -2.0f;
		//Maxになるまでタイマーを進ませる
		if (timer < kTimeMax) {
			timer += 1.0f / 60.0f;
		}
		else {
			timer = kTimeMax;
			move += 0.01f;
		}
		//タイトルが上からくる
		start.y = end.y + EaseOut(start.y, timer, kTimeMax);
		Title_pos.y = start.y + 10.0f * std::sin(move);

	}
	else {
		//重力でゆっくり落ちる
		wts[0].translation_.y -= gravity;
	}

	//タイトル更新処理
	sprite_Moji_Title->SetPosition(Title_pos);//常時場所設定
	sprite_Moji_Title->Update();

	if (isbullet) {
		if (wts[1].translation_.y == wts[2].translation_.y) {
			wts[2].translation_.x += 0.5f;
		}
		else if (wts[1].translation_.y == wts[3].translation_.y) {
			wts[3].translation_.x += 0.5f;
		}

		bulletTimer += 1.0f / 60.0f;
		if (bulletTimer >= kBulletTimeMax) {
			NextSceneFadeInStart("Select");
		}
	}

	//フェードが終わったら押せる
	if (timer >= kTimeMax && !isbullet) {

		//選択できるようになったら傘を親子関係をなくす
		if (wts[1].parent_) {
			wts[1].parent_ = nullptr;
			wts[1].rotation_ = { 0,90,0 };
			wts[1].translation_ = { -1.0f,wts[2].translation_.y,0 };
			umbrella->ChangeAnimation("umbrella_Close.gltf");
			umbrella->SetEnvironment("resource/rostock_laage_airport_4k.dds");
		}

		wts[1].translation_.x = -1.0f + std::sin(move) / 4.0f;


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
		
		if (Input::GetInstance()->GetJoystickState(0, state)) {
			//スティックの傾き度
			float padY = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

			if (padY > 0.5f) {
				wts[1].translation_.y = wts[2].translation_.y;//ゲームスタート
			}
			else if (padY < -0.5f) {
				wts[1].translation_.y = wts[3].translation_.y;//ゲーム終了
			}
		}

		//spaceもしくはAボタンを押したら実行
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
			Input::GetInstance()->TriggerBotton(state,preState, XINPUT_GAMEPAD_A)) {
			isbullet = true;
			bullet_particle->ChangeMode(BornParticle::MomentMode);
			bullet_particle->SetTranslate(wts[1].translation_);
			bullet_particle->SetRotate({0.0f,0.0f,-90.0f});
		}
	}
	//パーティクル更新
	bullet_particle->Update();

#ifdef USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &cameraTranslate.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &cameraTranslate.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &cameraRotate.x);
	ImGui::SliderFloat("cameraRotateX", &cameraRotate.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &cameraRotate.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &cameraRotate.z, -360.0f, 360.0f);
	
	camera->SetTranslate(cameraTranslate);
	camera->SetRotate(cameraRotate);

	ImGui::End();

#endif // USE_IMGUI

	//ワールド座標系更新
	for (uint32_t i = 0; i < 4; i++) {
		wts[i].UpdateMatrix();
	}

	object_autoPlayer->Update(wts[0]);
	umbrella->Update(wts[1]);

	object_Moji_Start->Update(wts[2]);
	object_Moji_End->Update(wts[3]);

	object_stage->Update();

	player_shadow->Update();
}

void TitleScene::Draw() {

	Object3dCommon::GetInstance()->Command();

	player_shadow->Draw();

	GLTFCommon::GetInstance()->Command();

	umbrella->Draw();
	object_autoPlayer->Draw();	
	object_Moji_Start->Draw();
	object_Moji_End->Draw();
	object_stage->Draw();

	SkinningCommon::GetInstance()->Command();

	SpriteCommon::GetInstance()->Command();

	sprite_Moji_Title->Draw();

	ParticleCommon::GetInstance()->Command();

	bullet_particle->Draw();
}

void TitleScene::Finalize() {}