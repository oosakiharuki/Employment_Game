#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "BaseScene.h"

/// <summary>
/// クリアシーン(BaseSceneの派生クラス)
/// </summary>
class ClearScene : public BaseScene{
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
	void InitObject();

	//入力処理
	Input* input_ = Input::GetInstance().get();
	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標

	//パーティクル
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;


	//スプライト描画
	std::unique_ptr<Sprite> sprite_;//ゲームクリアの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//プレイヤー
	std::unique_ptr<Object_glTF> playerGltf_;	
	WorldTransform wt_;
	Transform transform_{};

	//演出用のステージ
	std::unique_ptr<Object_glTF> stageGltf_;

	//紙吹雪のパーティクルたち
	const uint32_t kParticleMaxNum_ = 3;
	
	ParticleParametars particleFanfare_ = {
		"clear_fanfare", "resource/Sprite/white.png", Primitive::CreatePlane(),10,1.5f,{ 1,1,1 }
	};
	std::vector<std::unique_ptr<Particle>> particleFanfares_;

	//クリアシーンのカメラ座標
	const Vector3 kCameraTranslate_ = { 0.0f,2.0f,-18.0f };

	//スプライト
	const Vector2 kSpritePositionGameClear_ = { 100, 100 };//[ゲームクリア]文字の座標
	const Vector2 kSpritePositionButton_ = { 800,10 };//[ボタンで戻る]文字の座標
	const Vector2 kSpriteSizeButton_ = { 256,64 };//[ボタンで戻る]文字のサイズ

	//プレイヤーを前に向かす
	const float kPlayerFrontRange_ = 180.0f;

	//紙吹雪パーティクルたちの座標をずらす
	const float kFanfareX_ = 3.0f;
	//紙吹雪パーティクルの[左側]の座標
	const Vector3 kFanfareInitTranslate_ = { -kFanfareX_,2,0 };
	//紙吹雪パーティクルの座標
	Vector3 fanfareTranslate_ = kFanfareInitTranslate_;

};