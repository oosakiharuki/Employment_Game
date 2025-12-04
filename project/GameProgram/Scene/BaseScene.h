#pragma once
#include <cstdint>

#include "MyMath.h"
#include "Framework.h"

#include "Levelediter.h"
#include "Player.h"
#include "Enemy_Soldier.h"
#include "Enemy_Turret.h"
#include "Enemy_Bomb.h"
#include "CheckPoint.h"
#include "WarpGate.h"
#include "Goal.h"

#include "BoxModel.h"
#include "FadeScreen.h"

#include "EventTrigger.h"
#include "CameraControl.h"

#include "CollisionManager.h"
#include "NextStageSave.h"
/// <summary>
/// シーン共有処理
/// </summary>
class BaseScene {
protected:
	//現在のシーン
	static std::string sceneNo_;
	//次のシーン
	static std::string nextSceneNo_;

	//入力処理
	Input* input_ = Input::GetInstance().get();
	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標
	//カメラ移動範囲
	Vector3 cameraPointLeft_;//左端
	Vector3 cameraPointRight_;//右端
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//ズームするときのプレイヤーと少し離れた位置
	const Vector3 kPlayerAwayPos_ = { 0, 2, -15.0f };

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	Levelediter levelediter_;

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;
	//敵たち
	std::vector<std::shared_ptr<IEnemy>> enemies_;
	//ステージオブジェクトたち
	std::list<std::shared_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<Object3d> stageobj_;
	//ステージ全体の当たり判定AABB
	std::vector<AABB> stagesAABB_;

	//イベントトリガー
	std::vector<std::shared_ptr<EventTrigger>> eventTriggers_;

	//BGM
	SoundData BGMData_;
	float volume_ = 0.07f;//音量調節機能

	/// <summary>
	/// レベルエディタで配置処理
	/// </summary>
	/// <param name="leveleditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& leveleditor_file = "");

	/// <summary>
	/// 前のステージのデータ引継ぎ
	/// </summary>
	void PreviousSceneData();

	//前ステージデータ
	SceneSaveData sceneSaveData_ = {
		3,3,2,"stage_0" //初期設定
	};

	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	//end
	bool isGameEnd_ = false;

	/// <summary>
	/// 次のシーンの選択+フェードインを始める
	/// </summary>
	/// <param name="name">進めたいシーン</param>
	void NextSceneFadeInStart(const std::string& name);

	/// <summary>
	/// 次のシーンに進むフラグ(フェードなど間を開けたい時)
	/// </summary>
	/// <returns></returns>
	bool NextSceneFlag();

	/// <summary>
	/// sceneNo = nextSceneNoに
	/// </summary>
	void ChangeScene();

	/// <summary>
	/// ワープして次のシーンに進む処理
	/// </summary>
	void WarpNextScene(const std::string& nextScene);

	/// <summary>
	/// プレイヤーがゴールする時の処理
	/// </summary>
	void PlayerGoal();

	/// <summary>
	/// カメラがプレイヤーにズームする
	/// </summary>
	void CameraZoomPlayer();

	/// <summary>
	/// 共通の更新
	/// </summary>
	void UpdateGuide();

	/// <summary>
	/// 共通の描画
	/// </summary>
	void DrawGuide();

	//パーティクルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;

	/// <summary>
	/// 構造体_操作説明(ガイド)の設定
	/// </summary>
	struct Guide {
		std::string name;       //ガイドの名前(テクスチャ名)
		float lookPointX_left;  //見れる範囲_左端
		float lookPointX_right; //見れる範囲_右端
	};

	std::unordered_map<std::string, std::unique_ptr<Sprite>> spriteGuides_;

	//変更する場所(ジャンプ説明前は移動の説明)
	Guide kGuideMove_ = { "guide_move", -100.0f,-70.0f };   //移動の説明
	Guide kGuideJump_ = { "guide_jump", -65.0f, -10.0f };   //ジャンプの説明
	Guide kGuideFire_ = { "guide_fire", -5.0f, 16.0f };     //攻撃の説明
	Guide kGuideshield_ = { "guide_shield", 16.0f, 70.0f }; //守るの説明
	Guide kGuidebrink_ = { "guide_brink", 90.0f, 105.0f };  //ブリンクの説明
	Guide kGuideKakku_ = { "guide_kakku", 105.0f, 130.0f }; //滑空の説明
	Guide kGuideWarp_ = { "guide_warp", 140.0f, 200.0f };   //滑空の説明


public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Finalize() = 0;

	virtual ~BaseScene();

	/// <summary>
	/// シーン名で
	/// </summary>
	/// <returns></returns>現在のシーン
	std::string GetSceneNo();

	/// <summary>
	/// ゲーム終了処理
	/// </summary>
	/// <returns></returns>trueで終了
	bool GetIsGameEnd() { return isGameEnd_; }

	/// <summary>
	/// 次のシーンに進む処理
	/// </summary>
	/// <returns>作ったベースシーン</returns>
	std::unique_ptr<BaseScene> SetCurrentScene();

private:
	//ステージの.jsonファイル名
	std::string stageFileName_;

	//操作ガイドに必要な総数
	const uint32_t maxGuide = 7;

	/// <summary>
	/// 操作説明スプライトを作る
	/// </summary>
	/// <param name="guide">名前や座標が入った構造体</param>
	void CreateGuide(const Guide& guide);

	std::vector<Guide> guides_;

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 128,64 };
	const Vector2 kSpriteTranslate_ = { 300,20 };
};