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
private:
	//スプライト
	std::unique_ptr<Sprite> sprite_;//ゲームオーバーの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//プレイヤー
	std::unique_ptr<Object_glTF> playerGltf_;
	WorldTransform wt_;
	
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