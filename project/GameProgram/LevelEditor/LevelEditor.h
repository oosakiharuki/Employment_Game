#pragma once
#include "MyMath.h"
#include <unordered_map>
#include <memory>
#include <json.hpp>

//カメラの初期位置
struct CameraInitData {
	Transform transform;
	//移動ルート
	Vector3 leftPoint;
	Vector3 rightPoint;
};
/// <summary>
/// レベルエディタ(ステージのjson読み込み)
/// </summary>
class LevelEditor {
public:

	void LoadLevelEditor(const std::string& jsonName);

	//LevelData型
	struct LevelData {
		//通常オブジェクト
		struct ObjectData {
			std::string fileName;
			Transform transform;
			Vector3 colliderSize;
		};
		//プレイヤーのスポーン場所
		struct PlayerSpawnData {
			std::string fileName;
			Transform transform;
			Vector3 colliderSize;
		};
		//敵の生成場所
		struct EnemySpawnData {
			std::string EnemyName;
			std::string fileName;
			Transform transform;
			Vector3 colliderSize;
			//移動ルート
			Vector3 leftPoint;
			Vector3 rightPoint;
		};

		//ステージオブジェクトの生成場所
		struct StageObjectData {
			std::string ObjectName;
			std::string fileName;
			Transform transform;
			Vector3 colliderSize;
			//移動ルート
			Vector3 leftPoint;
			Vector3 rightPoint;
		};

		//イベントトリガーの生成場所
		struct EventTriggerData {
			Vector3 center;
			Vector3 size;
			Vector3 colliderSize;
			std::string csvFile;
			std::string cameraName;
		};
		/// <summary>
		/// ボスの生成データ
		/// </summary>
		struct BossData {
			std::string fileName;
			Transform transform;
			Vector3 colliderSize;
		};

		//構造体データの配列たち
		std::vector<ObjectData> objects;
		std::vector<PlayerSpawnData> players;
		std::vector<EnemySpawnData> spawnEnemies;
		std::unordered_map<std::string, CameraInitData> cameraInit;
		std::vector<StageObjectData> stageObjects;
		std::vector<EventTriggerData> eventTriggerDatas;
		std::vector<BossData> bosses;
	};

	/// <summary>
	/// getter‗レベルエディタ
	/// </summary>
	/// <returns></returns>作成したレベルエディタ
	LevelData* GetLevelData() { return levelData_.get(); }

private:

	//Json文字列から解凍したデータ
	nlohmann::json LoadJsonFile(const std::string& kFullpath);

	void LoadGameObjects(nlohmann::json& object);

	/// <summary>
	/// ステージの当たり判定設定
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	void LoadStage(nlohmann::json& object);

	/// <summary>
	/// カメラの配置、使用するカメラ設定
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	void LoadCamera(nlohmann::json& object);

	/// <summary>
	/// プレイヤーの配置
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	void LoadPlayer(nlohmann::json& object);

	/// <summary>
	/// 敵の配置
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	void LoadEnemies(nlohmann::json& object);

	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	void LoadStageObject(nlohmann::json& object);

	/// <summary>
	/// イベントトリガーの配置
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	/// <param name="translate">オブジェクト自体の中心</param>
	void LoadEventTrigger(nlohmann::json& object,const Vector3& translate);

	/// <summary>
	/// ボスの配置
	/// </summary>
	/// <param name="object">オブジェクトのデータ配置</param>
	void LoadBoss(nlohmann::json& object);

	/// <summary>
	/// トランスフォームの設定
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	/// <param name="objectTransform">導入するトランスフォーム</param>
	void SetTransform(nlohmann::json& object, Transform& objectTransform);

	/// <summary>
	/// コライダーの設定
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	/// <param name="objectVector3">導入するコライダーVector3</param>
	/// <param name="objectSize">オブジェクトの大きさ</param>
	void SetCollider(nlohmann::json& object,Vector3& objectVector3,const Vector3& objectSize);

	/// <summary>
	/// 移動ルートの設定
	/// </summary>
	/// <param name="object">オブジェクトのデータ配列</param>
	/// <param name="leftPoint">導入する左端</param>
	/// <param name="rightPoint">導入する右端</param>
	void SetTravelRoute(nlohmann::json& object, Vector3& leftPoint, Vector3& rightPoint);


	//レベルエディタ
	std::unique_ptr<LevelData> levelData_;

	//Blenderのカメラは0度だと真下を向くため補正
	const float kAdjustRange_ = 90.0f;
};