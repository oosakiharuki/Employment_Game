#include "TitleScene.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void TitleScene::Initialize() {

	camera_ = std::make_unique<Camera>();
	cameraTranslate_ = { 0,0.0f,-30.0f };
	camera_->SetTranslate(cameraTranslate_);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());

	//オブジェクトを読み込む
	ObjectLoading();

	//影
	playerShadow_ = std::make_unique<Shadow>();
	playerShadow_->Initialize();
	Vector3 shadowPos = wts_[0].translation_;//影位置
	shadowPos.y = kShadowPositionY_;//影位置Y
	playerShadow_->SetTranslate(shadowPos);

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

void TitleScene::ObjectLoading() {
	//playerワールド座標系
	for (uint32_t i = 0; i < kMaxWt_; i++) {
		//ワールド行列作成
		WorldTransform gwt;
		gwt.Initialize();

		//オブジェクト作成
		std::unique_ptr<Object_glTF> gObject;
		gObject = std::make_unique<Object_glTF>();
		gObject->Initialize();

		//オブジェクト読み込みとワールド行列の初期値
		if (i == 0) {
			//プレイヤー
			gwt.rotation_.y = kRotatePlayer_;
			gwt.translation_ = kPlayerInitPoint_;
			gObject->SetModelFile("player_standby.gltf");
		}
		else if (i == 1) {
			//傘
			gwt.translation_ = kUmbrellaInitPoint_;
			gwt.rotation_.x = kUmbrellaRange_;
			gObject->SetModelFile("umbrella_Open.gltf");
		}
		else if (i == 2) {
			//ゲームスタートの文字
			gwt.rotation_.y = kRotateSelectMoji_;
			gwt.translation_ = kSelectMojiPosition_;
			gwt.scale_ = kScaleSelectMoji_;
			gObject->SetModelFile("Select_Start.gltf");
		}
		else if (i == 3) {
			//ゲーム終了の文字
			gwt.rotation_.y = kRotateSelectMoji_;
			gwt.translation_ = kSelectMojiPosition_;
			gwt.translation_.y = kSelectEndPositionY_;//Y座標のみ変更
			gwt.scale_ = kScaleSelectMoji_;
			gObject->SetModelFile("Select_End.gltf");
		}
		else if (i == 4) {
			gObject->SetModelFile("Title_stage.gltf");
		}
		wts_.push_back(gwt);
		objects_.push_back(std::move(gObject));
	}
	//傘がプレイヤーを親としてついていく
	wts_[1].parent_ = &wts_[0];

}

void TitleScene::Update() {
	//次のシーンに移動
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		if (wts_[1].translation_.y == wts_[2].translation_.y) {
			ChangeScene();
		}
		else if (wts_[1].translation_.y == wts_[3].translation_.y) {
			isGameEnd_ = true;
		}
	}

	//ゲームパット用操作処理設定
	input_->JoystickUpdate(state_, preState_);

	//カメラ更新
	camera_->Update();

	//プレイヤーが降ってくるところ
	if (wts_[0].translation_.y <= kLandingPointY_) {
		//座標を維持
		wts_[0].translation_.y = kLandingPointY_;
		//Maxになるまでタイマーを進ませる
		if (titleFallingTimer_ < kTitleFallingTimeMax_) {
			titleFallingTimer_ += kDeltaTime;
		}
		else {
			titleFallingTimer_ = kTitleFallingTimeMax_;
		}
		//タイトルが上からくる(最終的にStartY_がEndY_と同じ値になる)
		appearsePointStartY_ = appearsePointEndY_ + EaseOut(appearsePointStartY_, titleFallingTimer_, kTitleFallingTimeMax_);
		titlePos_.y = appearsePointStartY_;

	}
	else {
		//重力でゆっくり落ちる
		wts_[0].translation_.y -= kGravity_;
	}

	//タイトル更新処理
	spriteMojiTitle_->SetPosition(titlePos_);//常時場所設定
	spriteMojiTitle_->Update();

	if (isSelect_) {
		if (wts_[1].translation_.y == wts_[2].translation_.y) {
			wts_[2].translation_.x += kMoveSelectMoji_;
		}
		else if (wts_[1].translation_.y == wts_[3].translation_.y) {
			wts_[3].translation_.x += kMoveSelectMoji_;
		}

		bulletTimer_ += kDeltaTime;
		if (bulletTimer_ >= kBulletTimeMax_) {
			NextSceneFadeInStart("Select");
		}
	}

	//フェードが終わったら押せる
	if (titleFallingTimer_ >= kTitleFallingTimeMax_ && !isSelect_) {

		//選択できるようになったら傘を親子関係をなくす
		if (wts_[1].parent_) {
			wts_[1].parent_ = nullptr;
			wts_[1].rotation_.z = kUmbrellaRange_;
			wts_[1].translation_ = { kUmbrellaArrowModePositionX_,wts_[2].translation_.y,0 };
			objects_[1]->ChangeAnimation("umbrella_Close.gltf");
		}

		if (wts_[2].rotation_.y <= 0.0f && wts_[3].rotation_.y <= 0.0f) {
			wts_[2].rotation_.y = 0.0f;
			wts_[3].rotation_.y = 0.0f;
		}
		else {
			wts_[2].rotation_.y -= kRotating_;
			wts_[3].rotation_.y -= kRotating_;
		}

		//キーボード操作

		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			wts_[1].translation_.y = wts_[2].translation_.y;//ゲームスタート
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			wts_[1].translation_.y = wts_[3].translation_.y;//ゲーム終了
		}

		//ゲームパット操作
		
		if (Input::GetInstance()->GetJoystickState(0, state_)) {
			//スティックの傾き度
			float padY = static_cast<float>(state_.Gamepad.sThumbLY) / 32768.0f;

			if (padY > 0.5f) {
				wts_[1].translation_.y = wts_[2].translation_.y;//ゲームスタート
			}
			else if (padY < -0.5f) {
				wts_[1].translation_.y = wts_[3].translation_.y;//ゲーム終了
			}
		}

		//spaceもしくはAボタンを押したら実行
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
			Input::GetInstance()->TriggerBotton(state_,preState_, XINPUT_GAMEPAD_A)) {
			isSelect_ = true;
			particleBullet_->SetParticleBorn(ParticleBorn::MomentMode);
			particleBullet_->SetTranslate(wts_[1].translation_);
			particleBullet_->SetRotate({0.0f,0.0f,kUmbrellaRange_ });
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
	for (uint32_t i = 0; i < kMaxWt_; i++) {
		wts_[i].UpdateMatrix();
		objects_[i]->Update(wts_[i]);
	}

	playerShadow_->Update();
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

	particleBullet_->Draw();
}

void TitleScene::Finalize() {}