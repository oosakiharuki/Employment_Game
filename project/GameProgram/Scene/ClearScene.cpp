#include "ClearScene.h"

using namespace MyMath;

void ClearScene::Initialize() {
	//クリアロゴ作成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Moji_GameClear.png");
	sprite_->SetPosition({ 100,100 });


	camera = std::make_unique<Camera>();
	
	cameraTranslate = {-2,3,-10.0f};
	cameraRotate = {15,0.0f,0};

	camera->SetTranslate(cameraTranslate);
	camera->SetRotate(cameraRotate);

	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());

	player_gltf_ = std::make_unique<Object_glTF>();
	player_gltf_->Initialize();
	player_gltf_->SetModelFile("player_clear.gltf");

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void ClearScene::Update() {

	input_->JoystickUpdate(state, preState);

	camera->Update();
	//クリアロゴ更新
	sprite_->Update();	

	WorldTransform gWt;
	gWt.Initialize();
	gWt.rotation_.y = 180.0f;
	gWt.UpdateMatrix();

	player_gltf_->Update(gWt);

	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
	}

	//セレクトシーンに戻る(フェードの最中にボタンを押せなくする)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
		Input::GetInstance()->TriggerBotton(state,preState,XINPUT_GAMEPAD_A) && !FadeScreen::GetInstance()->GetIsFadeing()) {
		NextSceneFadeInStart("Select");
	}

}

void ClearScene::Draw() {

	GLTFCommon::GetInstance()->Command();
	player_gltf_->Draw();

	SpriteCommon::GetInstance()->Command();
	//クリアロゴｗ描画
	sprite_->Draw();
}

void ClearScene::Finalize() {}