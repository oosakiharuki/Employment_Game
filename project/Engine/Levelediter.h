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
		//チェックポイントの場所
		struct CheckpointData {
			std::string fileName;
			Vector3 translation;
			//Vector3 rotation;
			AABB colliderAABB;
		};

		struct WarpGateData {
			std::string fileName;
			Vector3 translation;
			AABB colliderAABB;
		};

		std::vector<ObjectData> objects;
		std::vector<PlayerSpawnData> players;
		std::vector<EnemySpawnData> spawnEnemies;
		CameraInitData cameraInit;
		std::vector<CheckpointData> checkpoints;
		std::vector<WarpGateData> warpGate;
	};

	LevelData* GetLevelData() { return levelData; }
	//レベルエディタリセット
	void ResetData();

private:

	LevelData* levelData;
};