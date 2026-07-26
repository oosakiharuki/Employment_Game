#pragma once
#include "BaseScene.h"
#include "SpitOutLevelEditor.h"
#include "UIManager.h"
#include "BackGroundObject.h"
#include "PauseScreen.h"
#include "NextStageSave.h"


#include "BrokenBox.h"

/// <summary>
/// セレクト(ステージ選択)シーン(BaseSceneの派生クラス)
/// </summary>
class SelectScene : public EngineLayer::BaseScene{
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
	/// レベルエディタで配置処理
	/// </summary>
	/// <param name="levelEditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& levelEditor_file = "");
	/// <summary>
	/// ゲームオブジェクトの配置処理をまとめた
	/// </summary>
	void SpitOutGameObject();

	//カメラ
	std::unique_ptr<EngineLayer::Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標
	//カメラ移動範囲
	Vector3 cameraPointLeft_;//左端
	Vector3 cameraPointRight_;//右端
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	LevelEditor levelEditor_;
	SpitOutLevelEditor spitOut_;

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	const uint32_t kResetHp_ = 4;    //元の体力
	const uint32_t kResetRemain_ = 3;//元の残機

	//ステージオブジェクトたち
	std::list<std::unique_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<EngineLayer::Object3d> stageObj_;
	//ステージ全体の当たり判定AABB
	std::vector<AABB> stagesAABB_;

	//ステージの.jsonファイル名
	std::string stageFileName_;

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 128,64 };

	const Vector2 kSpriteTranslateMove_ = { 300,20 };
	const Vector2 kSpriteTranslateKeyE_ = { 600,20 };

	//操作ガイド
	std::vector<std::unique_ptr<Guide>> guides_;

	//背景
	std::unique_ptr<BackGroundObject> backGround = nullptr;
};