#pragma once
#include "BaseScene.h"
#include "SpitOutLevelEditor.h"
#include "UIManager.h"

/// <summary>
/// ゲームシーン(BaseSceneの派生クラス)
/// </summary>
class GameScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void SceneUpdate() override;
private:

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
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
	std::vector<std::shared_ptr<BaseEnemy>> enemies_;
	//ステージオブジェクトたち
	std::list<std::shared_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<Object3d> stageObj_;
	//ステージ全体の当たり判定AABB
	std::vector<AABB> stagesAABB_;

	//イベントトリガー
	std::vector<std::shared_ptr<EventTrigger>> eventTriggers_;

	//boss
	std::unique_ptr<Boss> boss_;

	//BGM
	SoundData BGMData_;
	float volume_ = 0.07f;//音量調節機能

	/// <summary>
	/// レベルエディタで配置処理
	/// </summary>
	/// <param name="levelEditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& levelEditor_file = "");

	//前ステージデータ
	SceneSaveData sceneSaveData_ = {
		3,3,2,"stage_0" //初期設定
	};

	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	/// <summary>
	/// プレイヤーがゴールする時の処理
	/// </summary>
	void PlayerGoal();

	//パーティクルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 200,100 };
	const Vector2 kSpriteTranslate_ = { 300,20 };

	//変更する場所(ジャンプ説明前は移動の説明)
	Guide kGuideMove_ = { "guide_move","guide_move",kSpriteTranslate_,kSpriteSize_, -100.0f,-70.0f };   //移動の説明
	Guide kGuideJump_ = { "guide_jump","guide_jump",kSpriteTranslate_,kSpriteSize_, -65.0f, -10.0f };   //ジャンプの説明
	Guide kGuideFire_ = { "guide_fire","guide_fire",kSpriteTranslate_,kSpriteSize_, -5.0f, 16.0f };     //攻撃の説明
	Guide kGuideShield_ = { "guide_shield","guide_shield",kSpriteTranslate_,kSpriteSize_, 16.0f, 70.0f }; //守るの説明
	Guide kGuideBrink_ = { "guide_brink","guide_brink",kSpriteTranslate_,kSpriteSize_, 90.0f, 105.0f };  //ブリンクの説明
	Guide kGuideGliding_ = { "guide_kakku","guide_kakku",kSpriteTranslate_,kSpriteSize_, 105.0f, 130.0f }; //滑空の説明
	Guide kGuideWarp_ = { "guide_warp","guide_warp",kSpriteTranslate_,kSpriteSize_, 140.0f, 200.0f };   //滑空の説明


	//ステージの.jsonファイル名
	std::string stageFileName_;

	//BGM
	SoundData soundData_;

	//スタート演出(水たまりから飛び出る感じに)
	bool isStartStage_ = true;
	Vector3 playerPoint_{};
	const float kStartWarpGateRange_ = 90.0f;

	float startPointY_ = -10.0f;
	//スタート時のワープゲート
	std::unique_ptr<WarpGate> startWarp_;
	//スタート時にプレイヤーを上げる変数
	const float kPlayerUp_ = 0.1f;
	//重ならないように影より上にする変数
	const float kWarpGateUpThanShadow_ = 0.02f;

	//落ちる限界
	const float kFallEndY_ = -10.0f;

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


	//シーンが変更するフラグ
	bool isNextLoadingStageScene = false;
	bool isNextClearScene = false;
	bool isNextGameOverScene = false;
};