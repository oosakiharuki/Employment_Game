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

/// <summary>
/// シーンの種類
/// </summary>
enum SCENE{
	Title,
	Game,
	Select,
	Clear,
	GameOver,
};

/// <summary>
/// 当たり判定:重なり部分を
/// </summary>
struct CollisionOverlap {
	bool isWall = false;
	bool isGround = false;
	Vector3 position = { 0,0,0 };
	Vector3 overlap = { 0,0,0 };
	AABB targetAABB = { { 0,0,0 }, { 0,0,0 } };
	AABB stageAABB = { { 0,0,0 }, { 0,0,0 } };
};

/// <summary>
/// イベントトリガーの構造体
/// </summary>
struct EventTrigger {
	bool isEvent = false; //イベント発動フラグ
	AABB aabb;            //イベント範囲
	Vector3 center;       //真ん中
	std::string csvFile;  //csvファイル
};


class IScene{
protected:
	static int sceneNo;

	XINPUT_STATE state, preState;

	std::unique_ptr<Camera> camera = nullptr;
	Vector3 cameraRotate;//回転
	Vector3 cameraTranslate;//座標
	//カメラ移動範囲
	Vector3 cameraPoint1;//幅1
	Vector3 cameraPoint2;//幅2
	WorldTransform worldTransformCamera_;

	Levelediter levelediter;

	std::unique_ptr<Player> player_ = nullptr;

	std::vector<std::unique_ptr<IEnemy>> enemies;

	std::list<std::unique_ptr<IStageObject>> stageObjects;

	std::unique_ptr<Object3d> stageobj;

	//ステージの足場
	std::vector<AABB> stagesAABB;

	//イベントトリガー
	std::vector<EventTrigger> eventTriggers;

	//テスト音源
	SoundData BGMData_;
	float volume = 0.02f;

	std::unique_ptr<BoxModel> skyBox = nullptr;

	//レベルエディタで配置
	void LevelEditorObjectSetting(const std::string leveleditor_file);
	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	//フェードスイッチ
	bool isfadeStart = false;

	//end
	bool isGameEnd = false;

	/// <summary>
	/// 対象(プレイヤー、敵など)の真下の床の位置に
	/// 影などで使用する
	/// </summary>
	/// <param name="stageAABB"></param>
	/// ステージ地面の全体
	/// <param name="shadowAABB"></param>
	/// 対象の影
	/// <param name="position"></param>
	/// 対象の場所
	/// <returns></returns>
	/// 対象から一番近い地面の上
	Vector3 UnderCollision(std::vector<AABB> stageAABB, AABB shadowAABB, Vector3 position);

	/// <summary>
	/// メインカメラ(プレイヤー中心カメラ)
	/// </summary>
	void MainCamera();

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	virtual ~IScene();

	int GetSceneNo();

	bool GetIsGameEnd() { return isGameEnd; }

private:

	/// <summary>
	/// 対象の重なった分戻す
	/// </summary>
	/// <param name="collisionBack"></param>
	/// 現在の対象の位置、重なった部分、壁/床のフラグが入ってある構造体
	/// <returns></returns>
	/// 現在の位置から重なる部分を引いた位置に、重なった部分が横なら壁で下なら床のフラグがtrueになる
	CollisionOverlap BackPosition(CollisionOverlap collisionBack);
};