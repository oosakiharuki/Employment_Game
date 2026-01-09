#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "BaseScene.h"

/// <summary>
/// タイトルシーン(BaseSceneの派生クラス)
/// </summary>
class TitleScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
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
	/// オブジェクト初期化処理
	/// </summary>
	void MakeObject(const std::string& objectName,const Vector3& translate, const Vector3& rotate, const Vector3& scale);

	//入力処理
	Input* input_ = Input::GetInstance().get();
	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標

	//パーティクルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;


	//タイトルで使うワールド行列たち
	std::vector<WorldTransform> wts_;
	std::vector<Transform> transforms_;

	//タイトルで使うオブジェクトたち
	std::vector<std::unique_ptr<Object_glTF>> objects_;
	//必要なワールド行列
	const uint32_t kMaxWt_ = 5;

	/// <summary>
	/// オブジェクトたちを読み込む
	/// </summary>
	void ObjectLoading();

	/// <summary>
	/// 最後にする更新処理
	/// </summary>
	void UpdateBehind();

	std::unique_ptr<Shadow> playerShadow_;//プレイヤー影
	std::unique_ptr<Sprite> spriteMojiTitle_;//タイトル名

	ParticleParametars particleBullet_ = {
		"title_bullet", "resource/Sprite/cone.png", Primitive::CreateCone(), 1, 0.5f, {1,1,1}
	};

	//決定したら弾を発射
	bool isSelect_ = false;
	float bulletTimer_ = 0.0f;
	const float kBulletTimeMax_ = 1.0f;

	//時間
	float titleFallingTimer_ = 0.0f;
	const float kTitleFallingTimeMax_ = 1.0f;

	//タイトルロゴが登場する移動
	float appearsePointStartY_ = -300.0f;//スタート
	float appearsePointEndY_ = 32.0f;//ゴール
	//タイトル座標
	Vector2 titlePos_ = { 430,appearsePointStartY_ };

	//通常プレイと同じ重力
	const float kGravity_ = 0.05f;

	//プレイヤー初期値
	const Vector3 kPlayerInitPoint_ = { -4.5f,10.0f,0 };
	//プレイヤー着地地点
	const float kLandingPointY_ = -2.0f;
	//プレイヤー前に向かす
	const Vector3 kRotatePlayer_ = { 0.0f,180.0f,0.0f };

	//最初、文字をふせておく
	const Vector3 kRotateSelectMoji_ = { 0.0f,180.0f,0.0f };
	//回転速度
	const float kRotating_ = 30.0f;

	//セレクトの文字の大きさ
	const Vector3 kScaleSelectMoji_ = { 1.5f,1.5f ,1.5f };
	//選択後に飛ばされる強さ
	const float kMoveSelectMoji_ = 0.5f;

	const Vector3 kSelectMojiPosition_ = { 3.0f,-0.5f,0.0f };
	const float kSelectEndPositionY_ = -2.5f;
	
	//傘
	const Vector3 kUmbrellaInitPoint_ = { 0.0f,2.0f,0.0f };
	//傘の向き
	const float kUmbrellaRange_ = -90.0f;
	const float kUmbrellaRangeArrowMode_ = 90.0f;//矢印の時
	Vector3 umbrellaRange_ = { kUmbrellaRange_,0.0f,0.0f };//プレイヤーが降ってくるとき

	//場所
	const float kUmbrellaArrowModePositionX_ = -1.0f;

	//影
	const float kShadowPositionY_ = -3.79f;//(-3.8f + 0.01f)

	//タイトルシーンのカメラ座標
	const Vector3 kCameraTranslate_ = { 0.0f,0.0f,-30.0f };

	const float kStickPower_ = 0.5f;//スティックの倒し具合

	bool isNextSelectScene = false;
	bool isNextGameEnd = false;
};