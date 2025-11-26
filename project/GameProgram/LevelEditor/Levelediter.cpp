#include "Levelediter.h"
#include <json.hpp>
#include <fstream>
#include <cassert>
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void Levelediter::LoadLevelediter(std::string jsonName) {
	//json
	levelData_ = new LevelData();

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
			//要素追加
			levelData_->objects.emplace_back(LevelData::ObjectData{});
			//
			LevelData::ObjectData& objectData = levelData_->objects.back();

			if (object.contains("file_name")) {
				//ファイル名
				objectData.fileName = object["file_name"];
			}
			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//BlenderのY軸とZ軸と違うため y = [2],z = [1]
			//移動
			objectData.translation.x = (float)transform["translation"][0];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = (float)transform["translation"][1];
			//回転
			objectData.rotation.x = (float)transform["rotation"][0];
			objectData.rotation.y = (float)transform["rotation"][2];
			objectData.rotation.z = (float)transform["rotation"][1];
			//スケール
			objectData.scaling.x = (float)transform["scaling"][0];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][1];
			
			//コライダー
			nlohmann::json& collider = object["collider"];

			if (collider != nullptr) {
				//Vectorに変換
				Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
				Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

				//AABBに追加+objのサイズに合わせて
				objectData.colliderAABB.min = center - (size * objectData.scaling * kDivideByTwo_);
				objectData.colliderAABB.max = center + (size * objectData.scaling * kDivideByTwo_);
			}

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

				//オブジェクトの真ん中 + eventTrigger自体の真ん中
				eventTrigger.center = objectData.translation + center;
				//オブジェクトサイズ(モデルでつかう)
				eventTrigger.size = size;
				//オブジェクトの真ん中 + eventTrigger自体の真ん中 ± サイズの半分
				eventTrigger.collisionAABB.min = objectData.translation + center - size * kDivideByTwo_;
				eventTrigger.collisionAABB.max = objectData.translation + center + size * kDivideByTwo_;

				//Blenderで設定したcsvファイル名を入れる
				eventTrigger.csvFile = "resource/csv/" + csvName + ".csv";

				eventTrigger.cameraName = cameraName;
			}
		}//プレイヤーの配置
		else if (type.compare("PlayerSpawn") == 0) {
			//要素追加
			levelData_->players.emplace_back(LevelData::PlayerSpawnData{});
			//
			LevelData::PlayerSpawnData& playerSpawnData = levelData_->players.back();

			if (object.contains("file_name")) {
				//ファイル名
				playerSpawnData.fileName = object["file_name"];
			}
			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//BlenderのY軸とZ軸と違うため y = [2],z = [1]
			//移動
			playerSpawnData.translation.x = (float)transform["translation"][0];
			playerSpawnData.translation.y = (float)transform["translation"][2];
			playerSpawnData.translation.z = (float)transform["translation"][1];
			//回転
			playerSpawnData.rotation.x = (float)transform["rotation"][0];
			playerSpawnData.rotation.y = (float)transform["rotation"][2];
			playerSpawnData.rotation.z = (float)transform["rotation"][1];

			//コライダー
			nlohmann::json& collider = object["collider"];

			if (collider != nullptr) {				
				//Vectorに変換
				Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
				Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

				//AABBに追加
				playerSpawnData.colliderAABB.min = center - (size * kDivideByTwo_);
				playerSpawnData.colliderAABB.max = center + (size * kDivideByTwo_);
			}
		}//敵の配置
		else if (type.compare("EnemySpawn") == 0) {
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

			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//BlenderのY軸とZ軸と違うため y = [2],z = [1]
			//移動
			enemySpawnData.translation.x = (float)transform["translation"][0];
			enemySpawnData.translation.y = (float)transform["translation"][2];
			enemySpawnData.translation.z = (float)transform["translation"][1];
			//回転
			enemySpawnData.rotation.x = (float)transform["rotation"][0];
			enemySpawnData.rotation.y = (float)transform["rotation"][2];
			enemySpawnData.rotation.z = (float)transform["rotation"][1];

			//コライダー
			nlohmann::json& collider = object["collider"];

			if (collider != nullptr) {

				//Vectorに変換
				Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
				Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

				//AABBに追加
				enemySpawnData.colliderAABB.min = center - (size * kDivideByTwo_);
				enemySpawnData.colliderAABB.max = center + (size * kDivideByTwo_);
			}

			//移動ルート
			nlohmann::json& travelRoute = object["travel_route"];

			if (travelRoute != nullptr) {
				enemySpawnData.Point1.x = (float)travelRoute["start"][0];
				enemySpawnData.Point1.y = (float)travelRoute["start"][2];
				enemySpawnData.Point1.z = (float)travelRoute["start"][1];

				enemySpawnData.Point2.x = (float)travelRoute["end"][0];
				enemySpawnData.Point2.y = (float)travelRoute["end"][2];
				enemySpawnData.Point2.z = (float)travelRoute["end"][1];
			}
		}
		else if (type.compare("CAMERA") == 0) {

			//名前を設定
			std::string CameraName = object["name"].get<std::string>();

			//要素追加
			CameraInitData& cameraInitData = levelData_->cameraInit[CameraName];

			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//移動
			cameraInitData.translation.x = (float)transform["translation"][0];
			cameraInitData.translation.y = (float)transform["translation"][2];
			cameraInitData.translation.z = (float)transform["translation"][1];
			//回転
			cameraInitData.rotation.x = -((float)transform["rotation"][0] - 90.0f);//blenderだと0度で真下を向くため
			cameraInitData.rotation.y = (float)transform["rotation"][2];
			cameraInitData.rotation.z = (float)transform["rotation"][1];

			//移動ルート
			nlohmann::json& travelRoute = object["travel_route"];

			if (travelRoute != nullptr) {
				//移動範囲_1
				cameraInitData.Point1.x = (float)travelRoute["start"][0];
				cameraInitData.Point1.y = (float)travelRoute["start"][2];
				cameraInitData.Point1.z = (float)travelRoute["start"][1];
				//移動範囲_2
				cameraInitData.Point2.x = (float)travelRoute["end"][0];
				cameraInitData.Point2.y = (float)travelRoute["end"][2];
				cameraInitData.Point2.z = (float)travelRoute["end"][1];
			}

		}//ステージオブジェクトの配置
		else if (type.compare("StageObjectSpawn") == 0) {
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
			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//BlenderのY軸とZ軸と違うため y = [2],z = [1]
			//移動
			stageObjectData.translation.x = (float)transform["translation"][0];
			stageObjectData.translation.y = (float)transform["translation"][2];
			stageObjectData.translation.z = (float)transform["translation"][1];
			
			//回転　必要じゃなさそう
			//checkpointData.rotation.x = (float)transform["rotation"][0];
			//checkpointData.rotation.y = (float)transform["rotation"][2];
			//checkpointData.rotation.z = (float)transform["rotation"][1];

			//コライダー
			nlohmann::json& collider = object["collider"];

			if (collider != nullptr) {
				//Vectorに変換
				Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
				Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

				//AABBに追加
				stageObjectData.colliderAABB.min = center - (size * kDivideByTwo_);
				stageObjectData.colliderAABB.max = center + (size * kDivideByTwo_);
			}
		}
	}

}

void Levelediter::ResetData() {
	delete levelData_;
	levelData_ = nullptr;
}
