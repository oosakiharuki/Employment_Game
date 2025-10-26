#pragma once
#include "MyMath.h"

class Levelediter {
public:

	void LoadLevelediter(std::string jsonName);

	//LevelData型
	struct LevelData {
		//通常オブジェクト
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
		//ステージオブジェクトの生成場所
		struct StageObjectData {
			std::string ObjectName;
			std::string fileName;
			Vector3 translation;
			AABB colliderAABB;
		};

		//イベントトリガーの生成場所
		struct EventTriggerData {
			Vector3 center;
			AABB collisionAABB;
			std::string csvFile;
		};

		//構造体データの配列たち
		std::vector<ObjectData> objects;
		std::vector<PlayerSpawnData> players;
		std::vector<EnemySpawnData> spawnEnemies;
		std::vector<CameraInitData> cameraInit;
		std::vector<StageObjectData> stageObjects;
		std::vector<EventTriggerData> eventTriggerDatas;
	};

	/// <summary>
	/// getter‗レベルエディタ
	/// </summary>
	/// <returns></returns>作成したレベルエディタ
	LevelData* GetLevelData() { return levelData; }
	/// <summary>
	/// レベルエディタリセット
	/// </summary>
	void ResetData();

private:
	//レベルエディタ
	LevelData* levelData;
};