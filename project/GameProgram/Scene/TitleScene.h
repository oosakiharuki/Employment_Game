#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "BaseScene.h"
#include "VisualActor.h"

#include "LevelEditor.h"
#include "SpitOutLevelEditor.h"
#include "CameraControl.h"

/// <summary>
/// タイトルシーン(BaseSceneの派生クラス)
/// </summary>
class TitleScene : public BaseScene {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// シーン変更処理
	/// </summary>
	void SceneUpdate() override;
private:

	/// <summary>
	/// スプライト初期化処理
	/// </summary>
	void InitSprite();
	/// <summary>
	/// カメラ初期化処理
	/// </summary>
	void InitCamera();
	/// <summary>
	/// オブジェクトたちを読み込む
	/// </summary>
	void ObjectLoading();
	/// <summary>
	/// 最後にする更新処理
	/// </summary>
	void UpdateBehind();

	void MoveTitleLogo();

	/// <summary>
	/// 選択操作
	/// </summary>
	void Operation();

	/// <summary>
	/// ゲームスタートを選択中
	/// </summary>
	void ArrowSelectStart(){
		transforms_["umbrella_Open"].translate.y = transforms_["Select_Start"].translate.y;//ゲームスタート
	}
	/// <summary>
	/// ゲーム終了を選択中
	/// </summary>
	void ArrowSelectEnd() {
		transforms_["umbrella_Open"].translate.y = transforms_["Select_End"].translate.y;//ゲーム終了
	}

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//パーティクルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;

	std::vector<std::unique_ptr<VisualActor>> visualActors;
	std::unordered_map<std::string ,Transform> transforms_;//各々の変更用

	std::unique_ptr<Shadow> playerShadow_;//プレイヤー影
	std::unique_ptr<Sprite> spriteMojiTitle_;//タイトル名

	ParticleParameters particleBullet_ = {
		"title_bullet", "resource/Sprite/cone.dds", Primitive::CreateCone(), 1, 0.5f, {1,1,1}
	};

	//決定したら弾を発射
	bool isSelect_ = false;
	float bulletTimer_ = 0.0f;
	const float kBulletTimeMax_ = 1.0f;

	//時間
	float titleFallingTimer_ = 0.0f;
	const float kTitleFallingTimeMax_ = 1.0f;

	//タイトルロゴが登場する移動
	float logoPointStartY_ = -300.0f;//スタート
	float logoPointEndY_ = 32.0f;//ゴール
	//タイトル座標
	Vector2 titlePos_ = { 430,logoPointStartY_ };

	//通常プレイと同じ重力
	const float kGravity_ = 0.05f;

	//プレイヤー着地地点
	const float kLandingPointY_ = -2.0f;

	//文字回転最大値
	const float kRotateMaxSelectMoji_ = 180.0f;
	//文字回転速度
	const float kRotating_ = 30.0f;

	//選択後に飛ばされる強さ
	const float kMoveSelectMoji_ = 0.5f;

	const Vector3 kSelectMojiPosition_ = { 3.0f,-0.5f,0.0f };
	const float kSelectEndPositionY_ = -2.5f;
	
	//傘
	float const kArrowRange_ = -90.0f;

	//場所
	const float kUmbrellaArrowModePositionX_ = 0.0f;
	float arrowMoveX = 0.0f;

	//影
	const float kShadowPositionY_ = -3.79f;//(-3.8f + 0.01f)

	const float kStickPower_ = 0.5f;//スティックの倒し具合

	std::unique_ptr<CameraControl> cameraControl_;
	float cameraRotateX_ = -15.0f;

	LevelEditor levelEditor_;
	SpitOutLevelEditor spitOut_;

};