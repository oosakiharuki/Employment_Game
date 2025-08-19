#pragma once

#include "Sprite.h"
#include "Object3d.h"
#include "Object_glTF.h"
#include "Particle.h" 
#include "Audio.h"
#include "MyMath.h"
#include "Framework.h"
#include "IScene.h"

#include "Levelediter.h"
#include "Player.h"
#include "Enemy_Soldier.h"
#include "Enemy_Turret.h"
#include "Enemy_Bomb.h"
#include "CheckPoint.h"

#include "BoxModel.h"

class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	ModelManager* modelManager;

	XINPUT_STATE state,preState;

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

	Object3d* stageobj;
	WorldTransform wt;

	//ステージの足場
	std::vector<AABB> stagesAABB;

	//テスト音源
	SoundData soundData_;
	SoundData BGMData_;
	float volume = 0.3f;

	BoxModel* skyBox = nullptr;

};