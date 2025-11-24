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
class IScene {
protected:
	//現在のシーン
	static std::string sceneNo_;
	//次のシーン
	static std::string nextSceneNo_;

	//入力処理
	Input* input_ = Input::GetInstance();
	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_;//回転
	Vector3 cameraTranslate_;//座標
	//カメラ移動範囲
	Vector3 cameraPointLeft_;//左端
	Vector3 cameraPointRight_;//右端
	std::unique_ptr<CameraControl> cameraControl_;

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
	/// レベルエディタで配置
	/// </summary>
	/// <param name="leveleditor_file"></param>指定したい場合は名前を入れることも可能
	void LevelEditorObjectSetting(const std::string leveleditor_file = "");
	
	/// <summary>
	/// 前のステージのデータ引継ぎ
	/// </summary>
	void PreviousSceneData();

	//前ステージデータ
	SceneSaveData data_;

	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	//end
	bool isGameEnd_ = false;

	/// <summary>
	/// 次のシーンの選択+フェードインを始める
	/// </summary>
	/// <param name="進めたいシーン"></param>
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
	/// ワープする時の処理
	/// </summary>
	void WarpNextScene();

	/// <summary>
	/// プレイヤーがゴールする時の処理
	/// </summary>
	void PlayerGoal();

	/// <summary>
	/// カメラがプレイヤーにズームする
	/// </summary>
	void CameraZoomPlayer();

	/// <summary>
	/// 共通の描画
	/// </summary>
	void DrawGuide();

	//操作ガイドのスプライト
	std::vector<std::unique_ptr<Sprite>> spriteGuide_;

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
	/// 削除処理
	/// </summary>
	virtual void Finalize() = 0;

	virtual ~IScene();

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

private:
	//ステージの.jsonファイル名
	std::string stageFileName_;

	//操作ガイドに必要な総数
	const uint32_t maxGuide = 7;
	//変更する場所
	const float kChangePointJump_ = -60.0f;
	const float kChangePointFire_ = 0.0f;
	const float kChangePointShield_ = 16.0f;
	const float kChangePointBrink_ = 80.0f;
	const float kChangePointKakku_ = 105.0f;

	//ゴール時前を向くように
	const Vector3 kPlayerForntAngle_ = { 0.0f,180.0f,0.0f };
};