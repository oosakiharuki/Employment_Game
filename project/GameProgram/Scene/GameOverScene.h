#pragma once
#include "Framework.h"
#include "BaseScene.h"

#include "LevelEditor.h"
#include "SpitOutLevelEditor.h"
#include "CameraControl.h"
/// <summary>
/// ゲームオーバーシーン(BaseSceneの派生クラス)
/// </summary>
class GameOverScene : public BaseScene{
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
	/// オブジェクト初期化処理
	/// </summary>
	void InitObject();

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	LevelEditor levelEditor_;
	SpitOutLevelEditor spitOut_;
	//オブジェクト描画
	std::vector<std::unique_ptr<VisualActor>> visualActors;
	std::unordered_map<std::string, Transform> transforms_;//各々の変更用
	//回る速度
	const float kRotate_ = 0.5f;

	//スプライト
	std::unique_ptr<Sprite> sprite_;//ゲームオーバーの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//スプライトの設定
	const Vector2 kSpritePositionGameOver_ = { 100.0f,100.0f };//[ゲームオーバー]文字の座標
	const Vector2 kSpritePositionButton_ = { 800.0f,10.0f };//[ボタンで戻る]文字の座標
	const Vector2 kSpriteSizeButton_ = { 256.0f, 64.0f };//[ボタンで戻る]文字のサイズ
};