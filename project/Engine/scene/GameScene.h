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
#include "Enemy.h"

#include "BoxModel.h"

class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	
	XINPUT_STATE state,preState;

	Camera* camera = nullptr;
	Vector3 cameraRotate = { 0.0f,0.0f,0.0f };
	Vector3 cameraTranslate = { 0.0f,0.0f,-15.0f };
	WorldTransform worldTransformCamera_;

	Levelediter levelediter;
	Player* player_ = nullptr;
	std::vector<Enemy*> enemies;

	Object3d* stageobj;
	WorldTransform wt;

	//ステージの足場
	std::vector<AABB> stagesAABB;

	//テスト音源
	SoundData soundData_;
	SoundData BGMData_;
	float volume = 0.3f;

	BoxModel* skyBox = nullptr;

	Object_glTF* gltfOBJ = nullptr;
};