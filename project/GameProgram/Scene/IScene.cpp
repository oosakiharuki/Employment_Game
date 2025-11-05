#include "IScene.h"
using namespace MyMath;

std::string IScene::sceneNo = "Select";

std::string IScene::nextSceneNo = "Select";

IScene::~IScene(){}

std::string IScene::GetSceneNo() { return sceneNo; }

void IScene::PreviousSceneData() {
	//前に残しておいたデータ
	data = NextStageSave::GetInstance()->GetNextStageSaveData();
	Stage_fileName = data.nextStageFile;
}

void IScene::LevelEditorObjectSetting(const std::string leveleditor_file) {

	//値が入っている場合
	if (leveleditor_file != "") {
		//代入
		Stage_fileName = leveleditor_file;
		data.playerHp = 3;
	}
		
	levelediter.LoadLevelediter("resource/Levelediter/" + Stage_fileName + ".json");

	camera = std::make_unique<Camera>();

	cameraControl_ = std::make_unique<CameraControl>();
	cameraControl_->Initialize();
	cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit["MainCamera"], false);

	Object3dCommon::GetInstance()->SetDefaultCamera(camera.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera.get());

	player_ = std::make_unique<Player>();
	player_->Initialize();

	//プレイヤー配置データがあるときプレイヤーを配置
	if (!levelediter.GetLevelData()->players.empty()) {
		auto& playerData = levelediter.GetLevelData()->players[0];
		player_->SetTranslate(playerData.translation);
		player_->SetRotate(playerData.rotation);
		player_->SetAABB(playerData.colliderAABB);

		player_->SetInit_Position(playerData.translation,playerData.rotation);
	}


	if (!levelediter.GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelediter.GetLevelData()->spawnEnemies) {

			std::unique_ptr<IEnemy> enemy;
			if (enemyData.EnemyName == "Turret") {
				enemy = std::make_unique<Enemy_Turret>();
			}
			else if (enemyData.EnemyName == "Bomb") {
				enemy = std::make_unique<Enemy_Bomb>();
			}
			else {
				enemy = std::make_unique<Enemy_Soldier>();
			}


			enemy->Initialize();

			enemy->SetTranslate(enemyData.translation);
			enemy->SetRotate(enemyData.rotation);
			enemy->SetInit_Position(enemyData.translation, enemyData.rotation);

			enemy->SetAABB(enemyData.colliderAABB);
			enemy->SetRoutePoint1(enemyData.Point1);
			enemy->SetRoutePoint2(enemyData.Point2);

			enemy->DirectionDegree();
			
			enemies.push_back(std::move(enemy));
		}
	}

	//
	if (!levelediter.GetLevelData()->eventTriggerDatas.empty()) {
		for (auto& eventTriggerData : levelediter.GetLevelData()->eventTriggerDatas) {
			EventData iterator;
			iterator.aabb = eventTriggerData.collisionAABB;
			iterator.center = eventTriggerData.center;
			iterator.csvFile = eventTriggerData.csvFile;
			iterator.cameraName = eventTriggerData.cameraName;

			std::unique_ptr<EventTrigger> eventTrigger;
			eventTrigger = std::make_unique<EventTrigger>();
			eventTrigger->SetEventData(iterator);

			eventTriggers.push_back(std::move(eventTrigger));
		}
	}

	if (!levelediter.GetLevelData()->objects.empty()) {
		for (auto& object : levelediter.GetLevelData()->objects) {

			Vector3 position = object.translation;

			AABB aabb;
			aabb.min = position + object.colliderAABB.min;
			aabb.max = position + object.colliderAABB.max;

			stagesAABB.push_back(aabb);
		}
	}

	//ステージオブジェクト地点
	if (!levelediter.GetLevelData()->stageObjects.empty()) {
		for (auto& stageObjectData : levelediter.GetLevelData()->stageObjects) {
			std::unique_ptr<IStageObject> stageObject;
			
			if (stageObjectData.ObjectName == "WarpGate") {
				stageObject = std::make_unique<WarpGate>();
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				stageObject = std::make_unique<CheckPoint>();
			}
			else if (stageObjectData.ObjectName == "Goal") {
				stageObject = std::make_unique<Goal>();
			}
			stageObject->Initialize();
			stageObject->SetPosition(stageObjectData.translation);
			stageObject->SetAABB(stageObjectData.colliderAABB);

			if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get())) {
				WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
				warpGate->SetNextStage(stageObjectData.fileName);
			}

			stageObjects.push_back(std::move(stageObject));
		}
	}


	for (auto& enemy : enemies) {
		enemy->SetStages(stagesAABB);
	}


	//プレイヤーの体力を上書き
	player_->SetHp(data.playerHp);

	//ステージの見た目
	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile(Stage_fileName + ".obj");
}

void IScene::WarpNextScene() {
	for (auto& stageObject : stageObjects) {
		//stageObjectsの中でワープゲートである場合
		if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get())) {
			WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
			//プレイヤーとワープゲートの当たり判定 + Eキーを押した時
			if (IsCollisionAABB(player_->GetAABB(), warpGate->GetAABB()) && Input::GetInstance()->TriggerKey(DIK_E)) {
				isWarp = true;
				cameraControl_->ZoomStart(player_->GetTranslate() + playerAwayPos);
				//次のステージに持ってくる情報
				NextStageSave::GetInstance()->SetNextStageFile(warpGate->GetNextStage());
				NextStageSave::GetInstance()->SetPlayerHp(player_->GetHp());
				player_->IsGround(true);
				break;
			}
		}
	}
}

void IScene::CollisionCommon() {
	//ゲーム内で使用する当たり判定
	CollisionManager::GetInstance()->AllCollisions(player_.get(),
		enemies,stageObjects,stagesAABB,eventTriggers,cameraControl_.get(),levelediter);
}

void IScene::NextSceneFadeInStart(const std::string& name) {
	FadeScreen::GetInstance()->FadeStart(type_fadeIn);
	nextSceneNo = name;
}

bool IScene::NextSceneFlag() {
	if (sceneNo != nextSceneNo) {
		return true;
	}
	return false;
}

void IScene::ChangeScene() {
	sceneNo = nextSceneNo;
}