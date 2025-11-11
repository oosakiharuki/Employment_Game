#include "IScene.h"
using namespace MyMath;

std::string IScene::sceneNo = "Title";

std::string IScene::nextSceneNo = "Title";

IScene::~IScene(){}

std::string IScene::GetSceneNo() { return sceneNo; }

void IScene::PreviousSceneData() {
	//前に残しておいたデータ
	data = NextStageSave::GetInstance()->GetNextStageSaveData();
	Stage_fileName = data.nextStageFile;
}

void IScene::LevelEditorObjectSetting(const std::string leveleditor_file) {
	
	player_ = std::make_unique<Player>();

	//値が入っている場合
	if (leveleditor_file != "") {
		//代入
		Stage_fileName = leveleditor_file;
		data.playerHp = player_->GetMaxHp();
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
	
	//プレイヤーの体力を上書き
	player_->SetHp(data.playerHp);
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
			iterator.size = eventTriggerData.size;
			iterator.csvFile = eventTriggerData.csvFile;
			iterator.cameraName = eventTriggerData.cameraName;

			std::unique_ptr<EventTrigger> eventTrigger;
			eventTrigger = std::make_unique<EventTrigger>();
			eventTrigger->Initialize();
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
				stageObject->SetNextStage(stageObjectData.fileName);
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				stageObject = std::make_unique<CheckPoint>();
			}
			else if (stageObjectData.ObjectName == "Goal") {
				stageObject = std::make_unique<Goal>();
			}
			stageObject->SetObjectName();//オブジェクトの名前保存
			stageObject->Initialize();
			stageObject->SetPosition(stageObjectData.translation);
			stageObject->SetAABB(stageObjectData.colliderAABB);

			stageObjects.push_back(std::move(stageObject));
		}
	}


	for (auto& enemy : enemies) {
		enemy->SetStages(stagesAABB);
	}

	//ステージの見た目
	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile(Stage_fileName + ".obj");


	if (Stage_fileName == "stage_0" || "stage_select") {

		for (uint32_t i = 0; i < 7; i++) {
			std::unique_ptr<Sprite> iterator = std::make_unique<Sprite>();
			iterator->Initialize("setumei_" + std::to_string(i) + ".png");
			iterator->SetSize({ 128,64 });
			iterator->SetPosition({300,20});
			setumei.push_back(std::move(iterator));
		}
	}
}

void IScene::DrawCommon() {

	if (Stage_fileName == "stage_select") {
		setumei[0]->Draw();
		setumei[6]->Draw();
	}
	else if (Stage_fileName == "stage_0") {
		if (player_->GetTranslate().x >= 105.0f) {
			setumei[5]->Draw();
		}
		else if (player_->GetTranslate().x >= 80.0f) {
			setumei[4]->Draw();
		}
		else if (player_->GetTranslate().x >= 16.0f) {
			setumei[3]->Draw();
		}
		else if (player_->GetTranslate().x >= 0.0f) {
			setumei[2]->Draw();
		}
		else if (player_->GetTranslate().x >= -60.0f) {
			setumei[1]->Draw();
		}
		else {
			setumei[0]->Draw();
		}
	}
}


void IScene::WarpNextScene() {
	if (CollisionManager::GetInstance()->IsWarp() && !player_->Performancing()) {
		cameraControl_->ZoomStart(player_->GetTranslate() + playerAwayPos);
		player_->IsPerformanceFlag(true);
		player_->SetRotate({ 0,0,0 });//向きを前に
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