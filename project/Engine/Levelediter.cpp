#include "Levelediter.h"
#include <json.hpp>
#include <fstream>
#include <cassert>

using namespace MyMath;

void Levelediter::LoadLevelediter(std::string jsonName) {
	//json
	levelData = new LevelData();

	//ファイルを選択
	const std::string fullpath = jsonName;

	//ファイルストリーム
	std::ifstream file;

	//読み取れない場合
	file.open(fullpath);

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


		std::string type = object["type"].get<std::string>();

		if (type.compare("MESH") == 0) {
			//要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});
			//
			LevelData::ObjectData& objectData = levelData->objects.back();

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
				objectData.colliderAABB.min = center - (size * objectData.scaling / 2.0f);
				objectData.colliderAABB.max = center + (size * objectData.scaling / 2.0f);
			}
		}
		else if (type.compare("PlayerSpawn") == 0) {
			//要素追加
			levelData->players.emplace_back(LevelData::PlayerSpawnData{});
			//
			LevelData::PlayerSpawnData& playerSpawnData = levelData->players.back();

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
				playerSpawnData.colliderAABB.min = center - (size / 2.0f);
				playerSpawnData.colliderAABB.max = center + (size / 2.0f);
			}
		}
		else if (type.compare("EnemySpawn") == 0) {
			//要素追加
			levelData->spawnEnemies.emplace_back(LevelData::EnemySpawnData{});
			//
			LevelData::EnemySpawnData& enemySpawnData = levelData->spawnEnemies.back();

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

			//enemySpawnData.fileName = transform["name"];

			//コライダー
			nlohmann::json& collider = object["collider"];

			if (collider != nullptr) {

				//Vectorに変換
				Vector3 center = { (float)collider["center"][0],(float)collider["center"][2], (float)collider["center"][1] };
				Vector3 size = { (float)collider["size"][0],(float)collider["size"][2], (float)collider["size"][1] };

				//AABBに追加
				enemySpawnData.colliderAABB.min = center - (size / 2.0f);
				enemySpawnData.colliderAABB.max = center + (size / 2.0f);
			}
		}
		else if (type.compare("CAMERA") == 0) {
			//要素追加
			levelData->cameraInit = LevelData::CameraInitData{};
			//
			LevelData::CameraInitData& cameraInitData = levelData->cameraInit;
			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//移動
			cameraInitData.translation.x = (float)transform["translation"][0];
			cameraInitData.translation.y = (float)transform["translation"][2];
			cameraInitData.translation.z = (float)transform["translation"][1];
			//回転
			cameraInitData.rotation.x = (float)transform["rotation"][0] - 90.0f;//blenderだと0度で真下を向くため
			cameraInitData.rotation.y = (float)transform["rotation"][2];
			cameraInitData.rotation.z = (float)transform["rotation"][1];

		}

		//子ノード
		if (object.contains("children")) {

		}
	}

}
