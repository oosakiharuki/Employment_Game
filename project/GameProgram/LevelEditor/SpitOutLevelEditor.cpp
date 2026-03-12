#include "SpitOutLevelEditor.h"
#include "NextStageSave.h"
#include "WarpGate.h"
#include "CheckPoint.h"
#include "Goal.h"

using namespace MyMath;

void SpitOutLevelEditor::SpitOutCamera(std::unique_ptr<CameraControl>& cameraControl) {
	//カメラコントロール設定
	cameraControl = std::make_unique<CameraControl>();
	cameraControl->Initialize();//初期化
	//メインのカメラ
	cameraControl->CameraSetting(levelEditor_->GetLevelData()->cameraInit["MainCamera"], false);
}

void SpitOutLevelEditor::CameraStartPointPlayer(std::unique_ptr<CameraControl>& cameraControl) {
	//メインのカメラ
	cameraControl->CameraStartPointPlayer(levelEditor_->GetLevelData()->cameraInit["MainCamera"], 
		levelEditor_->GetLevelData()->players[0].transform.translate);
}

void SpitOutLevelEditor::SpitOutPlayer(std::unique_ptr<Player>& player) {
	//プレイヤー配置データがあるときプレイヤーを配置
	if (!levelEditor_->GetLevelData()->players.empty()) {
		//一番最初のデータ位置に配置する
		auto& playerData = levelEditor_->GetLevelData()->players[0];
		player->SetTranslate(playerData.transform.translate);//座標
		player->SetRotate(playerData.transform.rotate);//向き
		player->InitUmbrellaRotateY();//傘の向き
		player->SetColliderSize(playerData.colliderSize);//当たり判定
		NextStageSave::GetInstance().SetCheckPoint(playerData.transform.translate);//チェックポイント設定
	}
}

std::vector<std::unique_ptr<BaseEnemy>> SpitOutLevelEditor::SpitOutEnemies() {
	std::vector<std::unique_ptr<BaseEnemy>> enemies;
	//- 敵配置 -
	//敵配置データがあるとき
	if (!levelEditor_->GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelEditor_->GetLevelData()->spawnEnemies) {
			//敵の名前で変更する
			if (enemyData.EnemyName == "Turret") {
				//固定大砲(タレット)
				std::unique_ptr<Enemy_Turret> enemy = std::make_unique<Enemy_Turret>();
				EnemyTemplate(*enemy,enemyData);
				//vectorに代入
				enemies.push_back(std::move(enemy));
			}
			else if (enemyData.EnemyName == "Bomb") {
				//自爆特攻(ボム)
				std::unique_ptr<Enemy_Bomb> enemy = std::make_unique<Enemy_Bomb>();
				EnemyTemplate(*enemy, enemyData);
				EnemyMoveRoute(*enemy, enemyData);//移動する敵
				//vectorに代入
				enemies.push_back(std::move(enemy));
			}
			else {
				//兵隊(ソルジャー)
				std::unique_ptr<Enemy_Soldier> enemy = std::make_unique<Enemy_Soldier>();
				EnemyTemplate(*enemy, enemyData);
				EnemyMoveRoute(*enemy, enemyData);//移動する敵
				//vectorに代入
				enemies.push_back(std::move(enemy));
			}
		}
	}
	return enemies;
}

void SpitOutLevelEditor::EnemyTemplate(BaseEnemy& enemy, LevelEditor::LevelData::EnemySpawnData enemyData) {
	enemy.Initialize();//初期設定
	enemy.SetTranslate(enemyData.transform.translate);//座標
	enemy.SetRotate(enemyData.transform.rotate);//向き
	enemy.SetColliderSize(enemyData.colliderSize);//当たり判定
	//オブジェクト向き
	enemy.DirectionDegree();
}

void SpitOutLevelEditor::EnemyMoveRoute(EnemyMoveCommand& enemy, LevelEditor::LevelData::EnemySpawnData enemyData) {
	enemy.SetRouteLeftPoint(enemyData.leftPoint);//移動ポイント1
	enemy.SetRouteRightPoint(enemyData.rightPoint);//移動ポイント2 (leftPoint < rightPoint)
}


void SpitOutLevelEditor::SpitOutStage(std::unique_ptr<Object3d>& stageObj, const std::string& stageFileName) {
	//- ステージ全体の当たり判定設定 -
	// 
	//ステージ自体の見た目
	stageObj = std::make_unique<Object3d>();
	stageObj->Initialize();
	stageObj->SetModelFile(stageFileName + ".obj");

	//MESH配置データがある場合
	if (!levelEditor_->GetLevelData()->objects.empty()) {
		for (auto& object : levelEditor_->GetLevelData()->objects) {
			//中心座標
			Vector3 position = object.transform.translate;
			//AABBの大きさ
			AABB aabb{};
			aabb.min = position - object.colliderSize;
			aabb.max = position + object.colliderSize;

			CollisionManager::GetInstance().CreateStageCollision(aabb,position,CollisionTypes::TypeStage);
		}
	}
}

std::list<std::unique_ptr<IStageObject>> SpitOutLevelEditor::SpitOutStageObject() {
	std::list<std::unique_ptr<IStageObject>> stageObjects;

	//- ステージオブジェクト(ギミック)配置 -
	//ステージオブジェクトの配置データがあるとき
	if (!levelEditor_->GetLevelData()->stageObjects.empty()) {
		for (auto& stageObjectData : levelEditor_->GetLevelData()->stageObjects) {
			//ステージオブジェクトの名前で変更する
			if (stageObjectData.ObjectName == "WarpGate") {
				//ワープゲート
				std::unique_ptr<WarpGate>stageObject = std::make_unique<WarpGate>();
				SettingStageObject(*stageObject.get(), stageObjectData);
				stageObject->SetNextStage(stageObjectData.fileName);//次のステージ用の.json名
				stageObjects.push_back(std::move(stageObject));
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				//チェックポイント
				std::unique_ptr<CheckPoint>stageObject = std::make_unique<CheckPoint>();
				SettingStageObject(*stageObject.get(), stageObjectData);
				stageObjects.push_back(std::move(stageObject));
			}
			else if (stageObjectData.ObjectName == "Goal") {
				//ゴール
				std::unique_ptr<Goal>stageObject = std::make_unique<Goal>();
				SettingStageObject(*stageObject.get(), stageObjectData);
				stageObjects.push_back(std::move(stageObject));
			}
		}
	}
	return stageObjects;
}

void SpitOutLevelEditor::SettingStageObject(IStageObject& stageObject, LevelEditor::LevelData::StageObjectData data) {
	stageObject.SetObjectName(data.ObjectName);//オブジェクトの名前保存
	stageObject.Initialize();//初期化
	stageObject.SetPosition(data.transform.translate);//座標位置
	stageObject.SetScale(data.transform.scale);
	stageObject.SetColliderSize(data.colliderSize);//当たり判定の大きさ
}


std::list<std::unique_ptr<EventTrigger>> SpitOutLevelEditor::SpitOutEventTrigger() {
	std::list<std::unique_ptr<EventTrigger>> eventTriggers;

	//- イベントトリガー配置 -
	//イベントトリガー配置データがあるとき
	if (!levelEditor_->GetLevelData()->eventTriggerDatas.empty()) {
		for (auto& eventTriggerData : levelEditor_->GetLevelData()->eventTriggerDatas) {
			EventData iterator;
			iterator.center = eventTriggerData.center;//真ん中座標
			
			iterator.aabb.min = iterator.center - eventTriggerData.colliderSize;//当たり判定
			iterator.aabb.max = iterator.center + eventTriggerData.colliderSize;//当たり判定
			
			iterator.size = eventTriggerData.size;//大きさ(フィールドを囲うオブジェクト用に使う)
			iterator.csvFile = eventTriggerData.csvFile;//csvを読み取る
			iterator.cameraName = eventTriggerData.cameraName;//カメラの変更

			std::unique_ptr<EventTrigger> eventTrigger;
			eventTrigger = std::make_unique<EventTrigger>();
			eventTrigger->Initialize();//初期設定
			eventTrigger->SetEventData(iterator);//データを代入

			eventTriggers.push_back(std::move(eventTrigger));
		}
	}
	return eventTriggers;
}

void SpitOutLevelEditor::SpitOutBoss(std::unique_ptr<Boss>& boss) {
	//bossがいない場合はスキップ
	if (!levelEditor_->GetLevelData()->bosses.empty()) {
		boss = std::make_unique<Boss>();
		boss->Initialize();
		//データを渡す
		auto& bossData = levelEditor_->GetLevelData()->bosses[0];
		boss->SetTranslate(bossData.transform.translate);
		boss->SetRotate(bossData.transform.rotate);
		boss->SetColliderSize(bossData.colliderSize);
		//真ん中を設定(横移動で使用)
		boss->BossCenter(bossData.transform.translate);
	}
}

std::vector<std::unique_ptr<Guide>> SpitOutLevelEditor::SpitOutGuide() {
	std::vector<std::unique_ptr<Guide>> guides;
	//- 操作ガイド配置 -
	//配置データがあるとき
	if (!levelEditor_->GetLevelData()->guides.empty()) {
		for (auto& guideData : levelEditor_->GetLevelData()->guides) {
			std::unique_ptr<Guide> guide = std::make_unique<Guide>();
			guide->Initialize(guideData.transform,guideData.textureFile);
			guides.push_back(std::move(guide));
		}
	}
	return guides;
}

std::vector<std::unique_ptr<VisualActor>> SpitOutLevelEditor::SpitOutVisualActor() {
	std::vector<std::unique_ptr<VisualActor>> visualActors;

	if (!levelEditor_->GetLevelData()->objects.empty()) {
		for (auto& objectData : levelEditor_->GetLevelData()->objects) {
			//fileNameに名前があるとき
			if (objectData.fileName != "") {
				std::unique_ptr<VisualActor> visualActor = std::make_unique<VisualActor>();
				visualActor->Initialize(objectData.fileName);
				visualActor->SetTransform(objectData.transform);
				visualActors.push_back(std::move(visualActor));
			}
		}
	}
	return visualActors;
}