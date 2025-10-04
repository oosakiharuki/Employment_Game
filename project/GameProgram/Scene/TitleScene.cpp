#include "TitleScene.h"
#include "LoadingModels.h"

using namespace MyMath;

void TitleScene::Initialize() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();

	TextureManager::GetInstance()->LoadTexture("resource/rostock_laage_airport_4k.dds");

	//player
	wts[0].Initialize();
	wts[0].rotation_ = { 0,180,0 };
	wts[0].translation_ = { -4.5f,10.0f,0 };

	//umbrella
	wts[1].Initialize();
	wts[1].parent_ = &wts[0];
	wts[1].translation_ = { 0,2,0 };
	wts[1].rotation_ = { -90.0f,0,0 };

	//start
	wts[2].Initialize();
	wts[2].rotation_.y = rotate_select_moji;
	wts[2].translation_ = { 3,-0.5f,0 };
	wts[2].scale_ = { 1.5f,1.5f,1.5f };

	//end
	wts[3].Initialize();
	wts[3].rotation_.y = rotate_select_moji;
	wts[3].translation_ = { 3,-2.5,0 };
	wts[3].scale_ = { 1.5f,1.5f,1.5f };

	worldTransformCamera_.Initialize();

	camera = new Camera();
	worldTransformCamera_.translation_ = { 0,0.0f,-30.0f };
	camera->SetTranslate(worldTransformCamera_.translation_);
	camera->SetRotate(worldTransformCamera_.rotation_);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera);
	GLTFCommon::GetInstance()->SetDefaultCamera(camera);
	ParticleCommon::GetInstance()->SetDefaultCamera(camera);

	object_autoPlayer = new Object_glTF();
	object_autoPlayer->Initialize();
	object_autoPlayer->SetModelFile("NewPlayer.gltf");
	object_autoPlayer->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	player_shadow = std::make_unique<Shadow>();
	player_shadow->Initialize();

	Vector3 shadowPos = wts[0].translation_;
	shadowPos.y = -3.8f;
	shadowPos.y += 0.01f;//少し上に上げてにしない

	player_shadow->SetTranslate(shadowPos);


	object_Moji_Start = std::make_unique<Object_glTF>();
	object_Moji_Start->Initialize();
	object_Moji_Start->SetModelFile("Select_Start.gltf");
	object_Moji_Start->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	object_Moji_End = std::make_unique<Object_glTF>();
	object_Moji_End->Initialize();
	object_Moji_End->SetModelFile("Select_End.gltf");
	object_Moji_End->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	umbrella = std::make_unique<Object_glTF>();
	umbrella->Initialize();
	umbrella->SetModelFile("umbrella_Open.gltf");
	umbrella->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	object_stage = std::make_unique<Object_glTF>();
	object_stage->Initialize();
	object_stage->SetModelFile("Title_stage.gltf");
	object_stage->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	sprite_Moji_Title = new Sprite();
	sprite_Moji_Title->Initialize("Moji_Title.png");
	sprite_Moji_Title->SetPosition(Title_pos);

	bullet_particle = std::make_unique<Particle>();
	bullet_particle->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	bullet_particle->SetParticleCount(1);
	bullet_particle->ChangeMode(BornParticle::Stop);
	bullet_particle->SetParticleMosion(ParticleMosion::Fixed);
	bullet_particle->SetFrequency(0.5f);
}

void TitleScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	camera->Update();

	if (wts[0].translation_.y <= -2.0f) {
		wts[0].translation_.y = -2.0f;
		if (timer < TimeMax) {
			timer += 1.0f / 60.0f;
		}
		else {
			timer = TimeMax;
			move += 0.01f;
		}
		start.y = end.y + EaseIn(start.y, timer, TimeMax);
		Title_pos.y = start.y + 10.0f * std::sin(move);

	}
	else {
		wts[0].translation_.y -= gravity;
	}

	sprite_Moji_Title->SetPosition(Title_pos);
	sprite_Moji_Title->Update();

	if (isbullet) {
		if (wts[1].translation_.y == wts[2].translation_.y) {
			wts[2].translation_.x += 0.5f;
		}
		else if (wts[1].translation_.y == wts[3].translation_.y) {
			wts[3].translation_.x += 0.5f;
		}

		bulletTimer += 1.0f / 60.0f;
		if (bulletTimer >= bulletTimeMax) {
			isfadeStart = true;
		}
	}

	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();

		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			if (wts[1].translation_.y == wts[2].translation_.y) {
				sceneNo = Select;
				isfadeStart = false;
			}
			else if (wts[1].translation_.y == wts[3].translation_.y) {
				isGameEnd = true;
			}
		}
	}
	else {
		FadeScreen::GetInstance()->FedeOut();
	}

	//フェードが終わったら押せる
	if (timer >= TimeMax && !isbullet) {

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


		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			wts[1].translation_.y = wts[2].translation_.y;
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			wts[1].translation_.y = wts[3].translation_.y;
		}

		//ゲームパット操作
		
		if (Input::GetInstance()->GetJoyStickState(0, state)) {
			float padY = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

			if (padY > 0.5f) {
				wts[1].translation_.y = wts[2].translation_.y;
			}
			else if (padY < -0.5f) {
				wts[1].translation_.y = wts[3].translation_.y;
			}
		}

		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
			Input::GetInstance()->TriggerBotton(state,preState, XINPUT_GAMEPAD_A)) {
			isbullet = true;
			bullet_particle->ChangeMode(BornParticle::MomentMode);
			bullet_particle->SetTranslate(wts[1].translation_);
			bullet_particle->SetRotate({0.0f,0.0f,-90.0f});
		}
	}

	bullet_particle->Update();

#ifdef _DEBUG

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &worldTransformCamera_.translation_.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &worldTransformCamera_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &worldTransformCamera_.rotation_.x);
	ImGui::SliderFloat("cameraRotateX", &worldTransformCamera_.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &worldTransformCamera_.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &worldTransformCamera_.rotation_.z, -360.0f, 360.0f);
	
	camera->SetTranslate(worldTransformCamera_.translation_);
	camera->SetRotate(worldTransformCamera_.rotation_);

	ImGui::End();

#endif // _DEBUG

	for (uint32_t i = 0; i < 4; i++) {
		wts[i].UpdateMatrix();
	}

	object_autoPlayer->Update(wts[0]);
	umbrella->Update(wts[1]);

	object_Moji_Start->Update(wts[2]);
	object_Moji_End->Update(wts[3]);

	WorldTransform wt;
	wt.Initialize();
	wt.translation_.y = -4.0f;
	wt.UpdateMatrix();
	object_stage->Update(wt);

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

void TitleScene::Finalize() {
	delete object_autoPlayer;
	delete sprite_Moji_Title;
}