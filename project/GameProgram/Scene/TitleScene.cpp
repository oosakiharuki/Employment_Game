#include "TitleScene.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void TitleScene::Initialize() {

	InitCamera();

	//オブジェクトを読み込む
	ObjectLoading();

	InitSprite();

	//パーティクル初期化
	sceneParticles_[particleBullet_.name] = ParticleManager::GetInstance()->InitParticle(particleBullet_);

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void TitleScene::InitSprite() {
	//タイトル名スプライトの初期化
	spriteMojiTitle_ = std::make_unique<Sprite>();
	spriteMojiTitle_->Initialize("Moji_Title.png");
	spriteMojiTitle_->SetPosition(titlePos_);
}

void TitleScene::InitCamera() {
	camera_ = std::make_unique<Camera>();
	cameraTranslate_ = kCameraTranslate_;
	camera_->SetTranslate(cameraTranslate_);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());
}

void TitleScene::ObjectLoading() {
	//playerワールド座標系
	for (uint32_t i = 0; i < kMaxWt_; i++) {

		//オブジェクト読み込みとワールド行列の初期値
		if (i == 0) {
			//プレイヤー
			MakeObject("player_standby.gltf",kPlayerInitPoint_,kRotatePlayer_,kDefaultScale_);
		}
		else if (i == 1) {
			//傘
			MakeObject("umbrella_Open.gltf",kUmbrellaInitPoint_,umbrellaRange_,kDefaultScale_);
		}
		else if (i == 2) {
			//ゲームスタートの文字
			MakeObject("Select_Start.gltf", kSelectMojiPosition_, kRotateSelectMoji_, kScaleSelectMoji_);
		}
		else if (i == 3) {
			//ゲーム終了の文字
			Vector3 gTranslate = kSelectMojiPosition_ + Vector3{ 0,kSelectEndPositionY_,0 };
			MakeObject("Select_End.gltf", gTranslate, kRotateSelectMoji_, kScaleSelectMoji_);
		}
		else if (i == 4) {
			MakeObject("Title_stage.gltf",{0,0,0},{0,0,0},kDefaultScale_);
		}

	}
	//傘がプレイヤーを親としてついていく
	wts_[1].SetParent(wts_[0]);
	
	//影
	playerShadow_ = std::make_unique<Shadow>();
	playerShadow_->Initialize();
	Vector3 shadowPos = transforms_[0].translate;//影位置
	shadowPos.y = kShadowPositionY_;//影位置Y
	playerShadow_->SetTranslate(shadowPos);
}

void TitleScene::MakeObject(const std::string& objectName, const Vector3& translate, const Vector3& rotate, const Vector3& scale) {
	//ワールド行列作成
	WorldTransform gwt;
	gwt.Initialize();

	//オブジェクト作成
	std::unique_ptr<Object_glTF> gObject;
	gObject = std::make_unique<Object_glTF>();
	gObject->Initialize();

	Transform transform{};
	//Transform更新処理
	transform = gwt.UpdateTransform();

	transform.rotate = rotate;
	transform.translate = translate;
	transform.scale = scale;
	gObject->SetModelFile(objectName);		
	
	wts_.push_back(gwt);
	transforms_.push_back(transform);
	objects_.push_back(std::move(gObject));
}

void TitleScene::Update() {

	//ゲームパット用操作処理設定
	input_->JoystickUpdate(state_, preState_);

	//カメラ更新
	camera_->Update();

	//プレイヤーが降ってくるところ
	if (transforms_[0].translate.y <= kLandingPointY_) {
		//座標を維持
		transforms_[0].translate.y = kLandingPointY_;
		//Maxになるまでタイマーを進ませる
		if (titleFallingTimer_ < kTitleFallingTimeMax_) {
			titleFallingTimer_ += kDeltaTime_;
		}
		else {
			//maxに
			titleFallingTimer_ = kTitleFallingTimeMax_;
		}
		//タイトルが上からくる(最終的にStartY_がEndY_と同じ値になる)
		appearsePointStartY_ = appearsePointEndY_ + EaseOut(appearsePointStartY_, titleFallingTimer_, kTitleFallingTimeMax_);
		titlePos_.y = appearsePointStartY_;

	}
	else {
		//重力でゆっくり落ちる
		transforms_[0].translate.y -= kGravity_;
	}

	//タイトル更新処理
	spriteMojiTitle_->SetPosition(titlePos_);//常時場所設定
	spriteMojiTitle_->Update();

	if (isSelect_) {
		if (transforms_[1].translate.y == transforms_[2].translate.y) {
			transforms_[2].translate.x += kMoveSelectMoji_;
		}
		else if (transforms_[1].translate.y == transforms_[3].translate.y) {
			transforms_[3].translate.x += kMoveSelectMoji_;
		}

		bulletTimer_ += kDeltaTime_;
		if (bulletTimer_ >= kBulletTimeMax_) {
			if (transforms_[1].translate.y == transforms_[2].translate.y) {
				//セレクトシーンに移動
				isNextSelectScene = true;
			}
			else if (transforms_[1].translate.y == transforms_[3].translate.y) {
				//ゲーム終了
				isNextGameEnd = true;
			}
			nextSceneNo_ = "Select";//セレクトシーンに移動
		}
	}

	//タイトルが出てくるまで選択部分は通さない
	if (titleFallingTimer_ < kTitleFallingTimeMax_) {
		UpdateBehind();
		return;
	}

	//-選択-
	//選択できるようになったら傘を親子関係をなくす
	if (wts_[1].GetParent()) {
		wts_[1].ResetParent();
		transforms_[1].rotate.z = kUmbrellaRange_;
		transforms_[1].translate = { kUmbrellaArrowModePositionX_,transforms_[2].translate.y,0 };
		objects_[1]->ChangeAnimation("umbrella_Close.gltf");
	}
	
	//文字が見えるまで回転する
	if (transforms_[2].rotate.y <= 0.0f && transforms_[3].rotate.y <= 0.0f) {
		//カメラの方向に文字が見える
		transforms_[2].rotate.y = 0.0f;
		transforms_[3].rotate.y = 0.0f;
	}
	else {
		//回転
		transforms_[2].rotate.y -= kRotating_;
		transforms_[3].rotate.y -= kRotating_;
	}

	//キーボード操作

	if (Input::GetInstance()->TriggerKey(DIK_W) && !isSelect_) {
		transforms_[1].translate.y = transforms_[2].translate.y;//ゲームスタート
	}
	if (Input::GetInstance()->TriggerKey(DIK_S) && !isSelect_) {
		transforms_[1].translate.y = transforms_[3].translate.y;//ゲーム終了
	}

	//ゲームパット操作

	if (Input::GetInstance()->GetJoystickState(0, state_) && !isSelect_) {
		//スティックの傾き度
		float padY = static_cast<float>(state_.Gamepad.sThumbLY) / 32768.0f;

		if (padY > kStickPower_) {
			transforms_[1].translate.y = transforms_[2].translate.y;//ゲームスタート
		}
		else if (padY < -kStickPower_) {
			transforms_[1].translate.y = transforms_[3].translate.y;//ゲーム終了
		}
	}

	//spaceもしくはAボタンを押したら実行
	if ((Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_A)) && !isSelect_) {
		isSelect_ = true;
		sceneParticles_[particleBullet_.name]->SetParticleBorn(ParticleBorn::MomentMode);
		sceneParticles_[particleBullet_.name]->SetTranslate(transforms_[1].translate);
		sceneParticles_[particleBullet_.name]->SetRotate({ 0.0f,0.0f,kUmbrellaRange_ });
	}

	UpdateBehind();
}

void TitleScene::UpdateBehind() {

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
	for (uint32_t i = 0; i < kMaxWt_; i++) {
		wts_[i].UpdateMatrix(transforms_[i]);
		objects_[i]->Update(wts_[i]);
	}

	playerShadow_->Update();

	//パーティクル更新
	for (auto& particle : sceneParticles_) {
		particle.second->Update();
	}
}

void TitleScene::Draw() {

	Object3dCommon::GetInstance()->Command();

	playerShadow_->Draw();

	GLTFCommon::GetInstance()->Command();

	for (auto& object : objects_) {
		object->Draw();
	}

	SpriteCommon::GetInstance()->Command();

	spriteMojiTitle_->Draw();

	ParticleCommon::GetInstance()->Command();

	for (auto& particle : sceneParticles_) {
		particle.second->Draw();
	}
}

void TitleScene::Finalize() {}

void TitleScene::SceneUpdate() {
	if (isNextSelectScene) {
		nextSceneNo_ = "Select";
	}

	if (isNextGameEnd) {
		nextSceneNo_ = "End";
	}

	ChangeSceneNo();

}