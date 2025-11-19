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

	//スカイボックス
	std::unique_ptr<BoxModel> skyBox_ = nullptr;

	/// <summary>
	/// レベルエディタで配置
	/// </summary>
	/// <param name="leveleditor_file"></param>指定したい場合は名前を入れることも可能
	void LevelEditorObjectSetting(const std::string leveleditor_file = "");
	
	/// <summary>
	/// 前のステージのデータ引継ぎ
	/// </summary>
	void PreviousSceneData();

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

	
	void WarpNextScene();

	void DrawCommon();

	std::vector<std::unique_ptr<Sprite>> setumei_;

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
};