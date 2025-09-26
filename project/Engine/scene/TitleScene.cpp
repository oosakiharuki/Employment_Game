#include "TitleScene.h"
#include "LoadingModels.h"

using namespace MyMath;

void TitleScene::Initialize() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();

	TextureManager::GetInstance()->LoadTexture("resource/rostock_laage_airport_4k.dds");


	wt.Initialize();

	camera = new Camera();
	camera->SetTranslate({ 0,0,-10.0f });
	camera->SetRotate(wt.rotation_);


	Object3dCommon::GetInstance()->SetDefaultCamera(camera);
	GLTFCommon::GetInstance()->SetDefaultCamera(camera);

	Moji_Title = new Object3d();
	Moji_Title->Initialize();
	Moji_Title->SetModelFile("Title.obj");
	Moji_Title->SetTranslate({ -3.0f,1.0f,0.0f });

	brainStem = new Object_glTF();
	brainStem->Initialize();
	brainStem->SetModelFile("BrainStem.gltf");
	brainStem->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	sprite_Moji_Start = new Sprite();
	sprite_Moji_Start->Initialize("Moji_Title_Start.png");
	sprite_Moji_Start->SetPosition({ 800,500 });

	sprite_Moji_End = new Sprite();
	sprite_Moji_End->Initialize("Moji_Title_End.png");
	sprite_Moji_End->SetPosition({ 700,600 });

	sprite_Select_Allow = new Sprite();
	sprite_Select_Allow->Initialize("Select_Allow.png");
	sprite_Select_Allow->SetPosition({ 750,550 });
}

void TitleScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	camera->Update();

	if (timer <= TimeMax) {
		timer += 1.0f / 60.0f;
	}
	else {
		timer = TimeMax;
	}

	center = EaseIn(center, timer, TimeMax);

	move += 0.01f;
	wt.translation_.y = 1.0f + center.y + std::sin(move) / 4.0f;
	wt.rotation_.x += 0.1f;

	Moji_Title->Update(wt);
	brainStem->Update(wt);

	sprite_Moji_Start->Update();
	sprite_Moji_End->Update();
	sprite_Select_Allow->Update();


	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();

		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			if (sprite_Select_Allow->GetPosition().y == 550) {
				sceneNo = Select;
				isfadeStart = false;
			}
			else if (sprite_Select_Allow->GetPosition().y == 650) {
				isGameEnd = true;
			}
		}
	}
	else {
		FadeScreen::GetInstance()->FedeOut();
	}

	//フェードが終わったら押せる
	if (!FadeScreen::GetInstance()->GetIsFadeing()) {

		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			sprite_Select_Allow->SetPosition({ 750,550 });
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			sprite_Select_Allow->SetPosition({ 650,650 });
		}

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isfadeStart = true;
		}

		//ゲームパット操作
		
		if (Input::GetInstance()->GetJoyStickState(0, state)) {
			float padY = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

			if (padY > 0.5f) {
				sprite_Select_Allow->SetPosition({ 750,550 });
			}
			else if (padY < -0.5f) {
				sprite_Select_Allow->SetPosition({ 650,650 });
			}
		}

		if (Input::GetInstance()->TriggerBotton(state,preState, XINPUT_GAMEPAD_A)) {
			isfadeStart = true;
		}
	}

	wt.UpdateMatrix();
}

void TitleScene::Draw() {

	Object3dCommon::GetInstance()->Command();

	Moji_Title->Draw();

	GLTFCommon::GetInstance()->Command();
	brainStem->Draw();

	SkinningCommon::GetInstance()->Command();	

	SpriteCommon::GetInstance()->Command();

	sprite_Moji_Start->Draw();
	sprite_Moji_End->Draw();
	sprite_Select_Allow->Draw();
}

void TitleScene::Finalize() {
	delete Moji_Title;
	delete brainStem;
	delete sprite_Moji_Start;
	delete sprite_Moji_End;
	delete sprite_Select_Allow;

}