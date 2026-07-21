#pragma once
#include "BaseScene.h"
#include "SpitOutLevelEditor.h"
#include "UIManager.h"
#include "BackGroundObject.h"
#include "PauseScreen.h"
#include "NextStageSave.h"
#include "WarpGate.h"

/// <summary>
/// ゲームシーン(BaseSceneの派生クラス)
/// </summary>
class GameScene : public EngineLayer::BaseScene {
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
	//敵たち
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;
	//ステージオブジェクトたち
	std::list<std::unique_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<EngineLayer::Object3d> stageObj_;

	//イベントトリガー
	std::list<std::unique_ptr<EventTrigger>> eventTriggers_;

	//boss
	std::unique_ptr<Boss> boss_;

	//BGM
	const std::string kBGMSoundName_ = "resource/sound/title.mp3";
	float volume_ = 0.07f;//音量調節機能

	/// <summary>
	/// レベルエディタで配置処理
	/// </summary>
	/// <param name="levelEditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& levelEditor_file = "");

	//パーティクルコンテナ
	std::unordered_map<std::string, std::unique_ptr<EngineLayer::Particle>> sceneParticles_;

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 200,100 };
	const Vector2 kSpriteTranslate_ = { 300,20 };

	//操作ガイド
	std::vector<std::unique_ptr<Guide>> guides_;

	//ステージの.jsonファイル名
	std::string stageFileName_;

	//スタート時のワープゲート(水たまりから飛び出る感じに)
	std::unique_ptr<WarpGate> startWarp_;
	//角度変更
	const float kStartWarpGateRange_ = 90.0f;
	//重ならないように影より上にする変数
	const float kWarpGateUpThanShadow_ = 0.02f;

	/// <summary>
	/// 水たまりのようなワープゲート出口
	/// </summary>
	void WaterWarpExit();

	/// <summary>
	/// リスポーン
	/// </summary>
	void Respawn();

	/// <summary>
	/// プレイヤーが生きている状態の場合の更新処理
	/// </summary>
	void PlayerAliveUpdate();
	/// <summary>
	/// ゲームオブジェクトの配置処理をまとめた
	/// </summary>
	void SpitOutGameObject();

	//背景
	std::unique_ptr<BackGroundObject> backGround = nullptr;
};