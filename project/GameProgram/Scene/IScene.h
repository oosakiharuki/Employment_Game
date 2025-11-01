#pragma once
#include <cstdint>

#include "Sprite.h"
#include "Object3d.h"
#include "Object_glTF.h"
#include "Particle.h" 
#include "Audio.h"
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

#include "ParticleNumber.h"
#include "FadeScreen.h"

#include "EventTrigger.h"
#include "CameraControl.h"

#include "CollisionManager.h"
#include "NextStageSave.h"

class IScene{
protected:
	//現在のシーン
	static std::string sceneNo;
	//次のシーン
	static std::string nextSceneNo;

	//入力処理
	Input* input_ = Input::GetInstance();
	//ゲームパット用の入力変数
	XINPUT_STATE state, preState;

	//カメラ
	std::unique_ptr<Camera> camera = nullptr;
	Vector3 cameraRotate;//回転
	Vector3 cameraTranslate;//座標
	//カメラ移動範囲
	Vector3 cameraPoint1;//幅1
	Vector3 cameraPoint2;//幅2
	std::unique_ptr<CameraControl> cameraControl_;

	Vector3 playerAwayPos = { 0, 2, -15.0f };

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	Levelediter levelediter;

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;
	//敵たち
	std::vector<std::shared_ptr<IEnemy>> enemies;
	//ステージオブジェクトたち
	std::list<std::shared_ptr<IStageObject>> stageObjects;
	
	//ステージ全体のオブジェクト
	std::unique_ptr<Object3d> stageobj;
	//ステージ全体の当たり判定AABB
	std::vector<AABB> stagesAABB;

	//イベントトリガー
	std::vector<std::shared_ptr<EventTrigger>> eventTriggers;

	//BGM
	SoundData BGMData_;
	float volume = 0.07f;//音量調節機能

	//スカイボックス
	std::unique_ptr<BoxModel> skyBox = nullptr;

	/// <summary>
	/// レベルエディタで配置
	/// </summary>
	/// <param name="leveleditor_file"></param>指定したい場合は名前を入れることも可能
	void LevelEditorObjectSetting(const std::string leveleditor_file = "");
	
	/// <summary>
	/// 前のステージのデータ引継ぎ
	/// </summary>
	void PreviousSceneData();

	SceneSaveData data;

	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	//end
	bool isGameEnd = false;

	/// <summary>
	/// ゲームパット入力処理
	/// </summary>
	void InputGamePad();

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

	
	void WarpNextScene();

	bool isWarp = false;

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
	bool GetIsGameEnd() { return isGameEnd; }

private:
	//ステージの.jsonファイル名
	std::string Stage_fileName;
};