#pragma once
#include "Framework.h"
#include "BaseScene.h"

/// <summary>
/// ゲームオーバーシーン(BaseSceneの派生クラス)
/// </summary>
class GameOverScene : public BaseScene{
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


	//スプライト
	std::unique_ptr<Sprite> sprite_;//ゲームオーバーの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//プレイヤー
	std::unique_ptr<Object_glTF> playerGltf_;
	WorldTransform wt_;
	Transform transform_{};
	
	//演出用ステージ
	std::unique_ptr<Object_glTF> stageGltf_;

	//ゲームオーバーのカメラ位置
	const Vector3 kCameraTranslate_ = { 0.0f,5.0f,-15.0f };//座標
	const Vector3 kCameraRotate_ = { 15.0f,0.0f,0.0f };//回転

	//スプライトの設定
	const Vector2 kSpritePositionGameOver_ = { 100.0f,100.0f };//[ゲームオーバー]文字の座標
	const Vector2 kSpritePositionBotton_ = { 800.0f,10.0f };//[ボタンで戻る]文字の座標
	const Vector2 kSpriteSizeBotton_ = { 256.0f, 64.0f };//[ボタンで戻る]文字のサイズ

	//回る速度
	const float kRotate_ = 0.5f;
};