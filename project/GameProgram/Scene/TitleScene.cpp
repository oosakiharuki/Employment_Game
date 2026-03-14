#include "TitleScene.h"
#include "SceneManager.h"
#include "SelectScene.h"
#include "UseEveryOne.h"
#include "FadeScreen.h"

using namespace MyMath;
using namespace UseEveryOne;

void TitleScene::Initialize() {

	levelEditor_.LoadLevelEditor("resource/LevelEditor/title_setting.json");
	spitOut_.SetLevelEditor(&levelEditor_);

	InitCamera();

	//オブジェクトを読み込む
	ObjectLoading();

	InitSprite();

	//パーティクル初期化
	sceneParticles_[particleBullet_.name] = ParticleManager::GetInstance().InitParticle(particleBullet_);

	FadeScreen::GetInstance().FadeStart(type_fadeOut);
	PauseScreen::GetInstance().OffPause();//ポーズを強制解除
}

void TitleScene::InitSprite() {
	//タイトル名スプライトの初期化
	spriteMojiTitle_ = std::make_unique<Sprite>();
	spriteMojiTitle_->Initialize("Moji_Title.png");
	spriteMojiTitle_->SetPosition(titlePos_);
}

void TitleScene::InitCamera() {
	camera_ = std::make_unique<Camera>();
	spitOut_.SpitOutCamera(cameraControl_);

	Object3dCommon::GetInstance().SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance().SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance().SetDefaultCamera(camera_.get());
}

void TitleScene::ObjectLoading() {

	visualActors = std::move(spitOut_.SpitOutVisualActor());


	for (auto& visualActor : visualActors) {	
		transforms_[visualActor->GetObjectName()] = visualActor->GetTransform();
	}

	//影
	playerShadow_ = std::make_unique<Shadow>();
	playerShadow_->Initialize();
	Vector3 shadowPos = transforms_["player_standby"].translate;//影位置
	//shadowPos.y = kShadowPositionY_;//影位置Y
	playerShadow_->SetTranslate(shadowPos);
}

void TitleScene::Update() {

	cameraRotateX_ += kDeltaTime_ * 4;
	cameraRotateX_ = std::clamp(cameraRotateX_,-45.0f,0.0f);
	//カメラコントロールの更新
	cameraControl_->SetRotation({cameraRotateX_,0,0});
	cameraControl_->Update(&*camera_);

	//プレイヤーが降ってくるところ
	if (transforms_["player_standby"].translate.y <= kLandingPointY_) {
		//座標を維持
		transforms_["player_standby"].translate.y = kLandingPointY_;
		//Maxになるまでタイマーを進ませる
		if (titleFallingTimer_ < kTitleFallingTimeMax_) {
			titleFallingTimer_ += kDeltaTime_;
		}
		else {
			//maxに
			titleFallingTimer_ = kTitleFallingTimeMax_;
		}
		//タイトルが上からくる(最終的にStartY_がEndY_と同じ値になる)
		titlePos_.y = EaseOut(logoPointEndY_, logoPointStartY_,  titleFallingTimer_);

	}
	else {
		//重力でゆっくり落ちる
		transforms_["player_standby"].translate.y -= kGravity_;
		transforms_["umbrella_Open"].translate.y -= kGravity_;
	}

	MoveTitleLogo();

	//タイトルが出てくるまで選択部分は通さない
	if (titleFallingTimer_ < kTitleFallingTimeMax_) {
		UpdateBehind();
		return;
	}

	//-選択-
	
	//文字が見えるまで回転する
	if (transforms_["Select_Start"].rotate.y <= 0.0f && transforms_["Select_End"].rotate.y <= 0.0f) {
		//カメラの方向に文字が見える
		transforms_["Select_Start"].rotate.y = 0.0f;
		transforms_["Select_End"].rotate.y = 0.0f;	
		arrowMoveX += kDeltaTime_ * kTwice_;
		transforms_["umbrella_Open"].translate.x = kUmbrellaArrowModePositionX_ + sin(arrowMoveX) * kDivideByThree_;//プレイヤーより少し右
	}
	else {
		//伏せていた文字を回転
		transforms_["Select_Start"].rotate.y -= kRotating_;
		transforms_["Select_End"].rotate.y -= kRotating_;
		//傘が矢印のように使う(配置、オブジェクト変更)
		visualActors[2]->ChangeObject("umbrella_Close.gltf");
		transforms_["umbrella_Open"].translate.x = kUmbrellaArrowModePositionX_;//プレイヤーより少し右
		transforms_["umbrella_Open"].rotate.z = kArrowRange_;//上向きから右向きに
		transforms_["umbrella_Open"].translate.y = transforms_["Select_Start"].translate.y;//「ゲームスタート」に合わせる
	}

	Operation();

	UpdateBehind();
}

void TitleScene::UpdateBehind() {

	for (auto& visualActor : visualActors) {
		visualActor->SetTransform(transforms_[visualActor->GetObjectName()]);
		visualActor->Update();
	}

	playerShadow_->Update();

	//パーティクル更新
	for (auto& particle : sceneParticles_) {
		particle.second->Update();
	}

	AABB shadowUnderAABB;
	shadowUnderAABB.min = { playerShadow_->GetCenter().x, 0,playerShadow_->GetCenter().z };
	shadowUnderAABB.max = { playerShadow_->GetCenter().x + 1, kShadowPositionY_,playerShadow_->GetCenter().z + 1 };

	CollisionManager::GetInstance().FrameCollision(shadowUnderAABB,playerShadow_->GetCenter(),CollisionTypes::TypeStage);

	CollisionManager::GetInstance().CollisionUpdate();
}

void TitleScene::Draw() {

	Object3dCommon::GetInstance().Command();

	playerShadow_->Draw();

	GLTFCommon::GetInstance().Command();

	for (auto& visualActor : visualActors) {
		visualActor->Draw();
	}

	SpriteCommon::GetInstance().Command();

	spriteMojiTitle_->Draw();

	ParticleCommon::GetInstance().Command();

	for (auto& particle : sceneParticles_) {
		particle.second->Draw();
	}
}

void TitleScene::Finalize() {}

void TitleScene::SceneUpdate() {
	//次のシーンに移動するとき
	if (SceneManager::GetInstance().NextSceneChangeFlag()) {
		//フェードを挟む(FadeIn)
		FadeScreen::GetInstance().FadeStart(type_fadeIn);
	}
}

void TitleScene::Operation(){
	//キーボード操作

	if (Input::GetInstance().TriggerKey(DIK_W) && !isSelect_) {
		ArrowSelectStart();//ゲームスタート
	}
	else if (Input::GetInstance().TriggerKey(DIK_S) && !isSelect_) {
		ArrowSelectEnd();//ゲーム終了
	}


	//ゲームパット操作
	if (Input::GetInstance().GetActiveGamePad() && !isSelect_) {
		//スティックの傾き度(LスティックYのみ)
		float padY = Input::GetInstance().LeftStickY();
		//上に傾いた
		if (padY > kStickPower_) {
			ArrowSelectStart();//ゲームスタート
		}//下に傾いた
		else if (padY < -kStickPower_) {
			ArrowSelectEnd();//ゲーム終了
		}
	}

	//spaceもしくはAボタンを押したら決定
	if ((Input::GetInstance().TriggerKey(DIK_SPACE) ||
		Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) && !isSelect_) {
		isSelect_ = true;//選択した
		//選択パーティクル
		sceneParticles_[particleBullet_.name]->SetParticleBorn(ParticleBorn::MomentMode);
		sceneParticles_[particleBullet_.name]->SetTranslate(transforms_["umbrella_Open"].translate);
		sceneParticles_[particleBullet_.name]->SetRotate({ 0.0f,0.0f,kArrowRange_ });
	}
}

void TitleScene::MoveTitleLogo() {
	//タイトル更新処理
	spriteMojiTitle_->SetPosition(titlePos_);//常時場所設定
	spriteMojiTitle_->Update();

	if (isSelect_) {
		//「ゲームスタート」を選択
		if (transforms_["umbrella_Open"].translate.y == transforms_["Select_Start"].translate.y) {
			transforms_["Select_Start"].translate.x += kMoveSelectMoji_;//文字が吹っ飛ぶ
		}
		//「ゲーム終了」を選択
		else if (transforms_["umbrella_Open"].translate.y == transforms_["Select_End"].translate.y) {
			transforms_["Select_End"].translate.x += kMoveSelectMoji_;//文字が吹っ飛ぶ
		}

		//飛ばされる時間
		bulletTimer_ += kDeltaTime_;
		if (bulletTimer_ >= kBulletTimeMax_) {
			if (transforms_["umbrella_Open"].translate.y == transforms_["Select_Start"].translate.y) {
				//セレクトシーンに移動
				SceneManager::GetInstance().ChangeScene(std::make_unique<SelectScene>());
			}
			else if (transforms_["umbrella_Open"].translate.y == transforms_["Select_End"].translate.y) {
				//ゲーム終了
				isGameEnd_ = true;
			}
		}
	}
}
