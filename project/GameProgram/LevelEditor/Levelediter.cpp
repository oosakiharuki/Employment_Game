#include "Levelediter.h"
#include <json.hpp>
#include <fstream>
#include <cassert>
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void Levelediter::LoadLevelediter(const std::string& jsonName) {
	//json
	levelData_ = std::make_unique<LevelData>();

	//ファイルを選択
	const std::string kFullpath = jsonName;

	//ファイルストリーム
	std::ifstream file;

	//読み取れない場合
	file.open(kFullpath);

	if (file.fail()) {
		assert(0);
	}

	//Json文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍処理
	file >> deserialized;

	assert(deserialized.is_object());//オブジェクトがあるか
	assert(deserialized.contains("name"));//名前があるか
	assert(deserialized["name"].is_string());//stringであるか

	//["name"]文字列として取得
	std::string name = deserialized["name"].get<std::string>();

	//正しいレベルデータファイルなのか
	assert(name.compare("scene") == 0);

	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		if (object.contains("disabled")) {
			bool disabled = object["disabled"].get<bool>();//jsonの値を入れる
			if (disabled) {
				//Trueの場合 配置しない
				continue;
			}
		}

		//タイプの設定
		std::string type = object["type"].get<std::string>();

		//ステージ当たり判定、イベントトリガーの配置
		if (type.compare("MESH") == 0) {
			LoadStage(object);
		}//プレイヤーの配置
		else if (type.compare("PlayerSpawn") == 0) {
			LoadPlayer(object);
		}//敵の配置
		else if (type.compare("EnemySpawn") == 0) {
			LoadEnemies(object);
		}
		else if (type.compare("CAMERA") == 0) {
			LoadCamera(object);
		}//ステージオブジェクトの配置
		else if (type.compare("StageObjectSpawn") == 0) {
			LoadStageObject(object);
		}
	}
}

void Levelediter::LoadStage(nlohmann::json& object) {
	//要素追加
	levelData_->objects.emplace_back(LevelData::ObjectData{});
	//
	LevelData::ObjectData& objectData = levelData_->objects.back();

	if (object.contains("file_name")) {
		//ファイル名
		objectData.fileName = object["file_name"];
	}

	//BlenderのY軸とZ軸と違うため y = [2],z = [1]
	//トランスフォーム
	SetTransform(object,objectData.transform);

	//コライダー
	SetCollider(object, objectData.colliderAABB, objectData.transform.scale);

	//イベントトリガー
	LoadEventTrigger(object,objectData.transform.translate);
}

void Levelediter::LoadCamera(nlohmann::json& object) {
	//名前を設定
	std::string CameraName = object["name"].get<std::string>();

	//要素追加
	CameraInitData& cameraInitData = levelData_->cameraInit[CameraName];

	//トランスフォーム
	SetTransform(object, cameraInitData.transform);

	cameraInitData.transform.rotate.x -= kAdjustRange_;//blenderだと0度で真下を向くため
	cameraInitData.transform.rotate.x = -cameraInitData.transform.rotate.x;//負の値にする
	
	//移動ルート
	SetTravelRoute(object, cameraInitData.leftPoint, cameraInitData.rightPoint);
}

void Levelediter::LoadPlayer(nlohmann::json& object) {
	//要素追加
	levelData_->players.emplace_back(LevelData::PlayerSpawnData{});
	//
	LevelData::PlayerSpawnData& playerSpawnData = levelData_->players.back();

	if (object.contains("file_name")) {
		//ファイル名
		playerSpawnData.fileName = object["file_name"];
	}
	//BlenderのY軸とZ軸と違うため y = [2],z = [1]
	//トランスフォーム
	SetTransform(object, playerSpawnData.transform);

	//コライダー
	SetCollider(object, playerSpawnData.colliderAABB,playerSpawnData.transform.scale);
}

void Levelediter::LoadEnemies(nlohmann::json& object) {
	//要素追加
	levelData_->spawnEnemies.emplace_back(LevelData::EnemySpawnData{});
	LevelData::EnemySpawnData& enemySpawnData = levelData_->spawnEnemies.back();

	//各敵の名前
	if (object.contains("EnemyName")) {
		enemySpawnData.EnemyName = object["EnemyName"];
	}

	if (object.contains("file_name")) {
		//ファイル名
		enemySpawnData.fileName = object["file_name"];
	}

	//トランスフォーム
	SetTransform(object,enemySpawnData.transform);

	//コライダー
	SetCollider(object,enemySpawnData.colliderAABB,enemySpawnData.transform.scale);

	//移動ルート
	SetTravelRoute(object, enemySpawnData.leftPoint,enemySpawnData.rightPoint);
}

void Levelediter::LoadStageObject(nlohmann::json& object) {
	//要素追加
	levelData_->stageObjects.emplace_back(LevelData::StageObjectData{});
	//
	LevelData::StageObjectData& stageObjectData = levelData_->stageObjects.back();

	if (object.contains("StageObjectName")) {
		stageObjectData.ObjectName = object["StageObjectName"];
	}

	if (object.contains("file_name")) {
		//ファイル名
		stageObjectData.fileName = object["file_name"];
	}
	//トランスフォーム
	SetTransform(object,stageObjectData.transform);

	//コライダー
	SetCollider(object, stageObjectData.colliderAABB,stageObjectData.transform.scale);
}

void Levelediter::LoadEventTrigger(nlohmann::json& object, const Vector3& translate) {
	//イベントトリガー
	nlohmann::json& trigger = object["event_trigger"];

	if (trigger != nullptr) {
		//Vectorに変更
		Vector3 center = { (float)trigger["center"][0],(float)trigger["center"][2],(float)trigger["center"][1] };
		Vector3 size = { (float)trigger["size"][0],(float)trigger["size"][2],(float)trigger["size"][1] };
		std::string cameraName = trigger["camera"];
		std::string csvName = trigger["csv"];

		levelData_->eventTriggerDatas.emplace_back(LevelData::EventTriggerData{});
		LevelData::EventTriggerData& eventTrigger = levelData_->eventTriggerDatas.back();

		//オブジェクトの真ん中(translate) + eventTrigger自体の真ん中
		eventTrigger.center = translate + center;
		//オブジェクトサイズ(モデルでつかう)
		eventTrigger.size = size;
		//オブジェクトの真ん中 + eventTrigger自体の真ん中 ± サイズの半分
		eventTrigger.collisionAABB.min = translate + center - size * kDivideByTwo_;
		eventTrigger.collisionAABB.max = translate + center + size * kDivideByTwo_;

		//Blenderで設定したcsvファイル名を入れる
		eventTrigger.csvFile = "resource/csv/" + csvName + ".csv";

		eventTrigger.cameraName = cameraName;
	}
}

void Levelediter::SetTransform(nlohmann::json& object, Transform& objectTransform) {
	//トランスフォームのパラメータ読み込み
	nlohmann::json& transform = object["transform"];

	//移動
	objectTransform.translate.x = (float)transform["translation"][0];
	objectTransform.translate.y = (float)transform["translation"][2];
	objectTransform.translate.z = (float)transform["translation"][1];

	//回転
	objectTransform.rotate.x = (float)transform["rotation"][0];
	objectTransform.rotate.y = (float)transform["rotation"][2];
	objectTransform.rotate.z = (float)transform["rotation"][1];

	//スケール
	objectTransform.scale.x = (float)transform["scaling"][0];
	objectTransform.scale.y = (float)transform["scaling"][2];
	objectTransform.scale.z = (float)transform["scaling"][1];
}

void Levelediter::SetCollider(nlohmann::json& object, AABB& objectAABB, const Vector3& objectSize) {
	//コライダー
	nlohmann::json& collider = object["collider"];

	if (collider != nullptr) {
		//Vectorに変換
		Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
		Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

		//AABBに追加
		objectAABB.min = center - (size * objectSize * kDivideByTwo_);
		objectAABB.max = center + (size * objectSize * kDivideByTwo_);
	}
}

void Levelediter::SetTravelRoute(nlohmann::json& object, Vector3& leftPoint, Vector3& rightPoint) {
	//移動ルート
	nlohmann::json& travelRoute = object["travel_route"];

	if (travelRoute != nullptr) {
		//移動範囲_1
		leftPoint.x = (float)travelRoute["start"][0];
		leftPoint.y = (float)travelRoute["start"][2];
		leftPoint.z = (float)travelRoute["start"][1];
		//移動範囲_2
		rightPoint.x = (float)travelRoute["end"][0];
		rightPoint.y = (float)travelRoute["end"][2];
		rightPoint.z = (float)travelRoute["end"][1];
	}
}
