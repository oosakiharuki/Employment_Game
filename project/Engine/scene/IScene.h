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

#include "BoxModel.h"

#include "ParticleNumber.h"


enum SCENE{
	Title,
	Game,
	Select,
	Clear,
	GameOver,
};


class IScene{
protected:
	static int sceneNo;

	XINPUT_STATE state, preState;

	Camera* camera = nullptr;
	Vector3 cameraRotate;//回転
	Vector3 cameraTranslate;//座標
	//カメラ移動範囲
	Vector3 cameraPoint1;//幅1
	Vector3 cameraPoint2;//幅2
	WorldTransform worldTransformCamera_;

	Levelediter levelediter;

	Player* player_ = nullptr;
	std::list<IEnemy*> enemies;

	std::vector<CheckPoint*> checkPoints;
	std::list<WarpGate*> warpGates;

	Object3d* stageobj;
	WorldTransform wt;

	//ステージの足場
	std::vector<AABB> stagesAABB;

	//テスト音源
	SoundData BGMData_;
	float volume = 0.3f;

	BoxModel* skyBox = nullptr;

	//レベルエディタで配置
	void LevelEditorObjectSetting(const std::string leveleditor_file);
	//全シーン共有できる当たり判定
	void CollisionCommon();

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	virtual ~IScene();

	int GetSceneNo();
};