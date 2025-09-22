#pragma once
#include "MyMath.h"

class Levelediter {
public:

	void LoadLevelediter(std::string jsonName);

	//LevelData型
	struct LevelData {
		struct ObjectData {
			std::string fileName;
			Vector3 translation;
			Vector3 rotation;
			Vector3 scaling;
			AABB colliderAABB;
		};
		//プレイヤーのスポーン場所
		struct PlayerSpawnData {
			std::string fileName;
			Vector3 translation;
			Vector3 rotation;
			AABB colliderAABB;
		};
		//敵の生成場所
		struct EnemySpawnData {
			std::string EnemyName;
			std::string fileName;
			Vector3 translation;
			Vector3 rotation;
			AABB colliderAABB;
			//移動ルート
			Vector3 Point1;
			Vector3 Point2;
		};
		//カメラの初期位置
		struct CameraInitData {
			Vector3 translation;
			Vector3 rotation;
			//移動ルート
			Vector3 Point1;
			Vector3 Point2;
		};
		//ステージオブジェクトの生成
		struct StageObjectData {
			std::string ObjectName;
			std::string fileName;
			Vector3 translation;
			//Vector3 rotation;
			AABB colliderAABB;
		};


		std::vector<ObjectData> objects;
		std::vector<PlayerSpawnData> players;
		std::vector<EnemySpawnData> spawnEnemies;
		CameraInitData cameraInit;
		std::vector<StageObjectData> stageObjects;
	};

	LevelData* GetLevelData() { return levelData; }
	//レベルエディタリセット
	void ResetData();

private:

	LevelData* levelData;
};