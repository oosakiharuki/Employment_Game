#include "IScene.h"
using namespace MyMath;

std::string IScene::sceneNo = "Game";

std::string IScene::nextSceneNo = "Game";

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

	//ステージのjsonを読み取る
	levelediter.LoadLevelediter("resource/Levelediter/" + Stage_fileName + ".json");

	camera = std::make_unique<Camera>();

	//カメラコントロール設定
	cameraControl_ = std::make_unique<CameraControl>();
	cameraControl_->Initialize();
	cameraControl_->CameraSetting(levelediter.GetLevelData()->cameraInit["MainCamera"], false);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance()->SetDefaultCamera(camera.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera.get());
	
	//プレイヤーの体力を上書き
	player_->SetHp(data.playerHp);
	player_->Initialize();//初期設定

	//プレイヤー配置データがあるときプレイヤーを配置
	if (!levelediter.GetLevelData()->players.empty()) {
		//一番最初のデータ位置に配置する
		auto& playerData = levelediter.GetLevelData()->players[0];
		player_->SetTranslate(playerData.translation);//座標
		player_->SetRotate(playerData.rotation);//向き
		player_->SetAABB(playerData.colliderAABB);//当たり判定
		//初期状態(位置、回転)設定
		player_->SetInit_Position(playerData.translation,playerData.rotation);
	}

	//敵配置データがあるとき
	if (!levelediter.GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelediter.GetLevelData()->spawnEnemies) {

			std::unique_ptr<IEnemy> enemy;
			//敵の名前で変更する
			if (enemyData.EnemyName == "Turret") {
				//固定大砲(タレット)
				enemy = std::make_unique<Enemy_Turret>();
			}
			else if (enemyData.EnemyName == "Bomb") {
				//自爆特攻(ボム)
				enemy = std::make_unique<Enemy_Bomb>();
			}
			else {
				//兵隊(ソルジャー)
				enemy = std::make_unique<Enemy_Soldier>();
			}


			enemy->Initialize();//初期設定

			enemy->SetTranslate(enemyData.translation);//座標
			enemy->SetRotate(enemyData.rotation);//向き
			//初期状態(位置、回転)設定
			enemy->SetInit_Position(enemyData.translation, enemyData.rotation);

			enemy->SetAABB(enemyData.colliderAABB);//当たり判定
			enemy->SetRoutePoint1(enemyData.Point1);//移動ポイント1
			enemy->SetRoutePoint2(enemyData.Point2);//移動ポイント2 (point1 < point2)
			//オブジェクト向き
			enemy->DirectionDegree();
			//vectorに代入
			enemies.push_back(std::move(enemy));
		}
	}

	//イベントトリガー配置データがあるとき
	if (!levelediter.GetLevelData()->eventTriggerDatas.empty()) {
		for (auto& eventTriggerData : levelediter.GetLevelData()->eventTriggerDatas) {
			EventData iterator;
			iterator.aabb = eventTriggerData.collisionAABB;//当たり判定
			iterator.center = eventTriggerData.center;//真ん中座標
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

	//MESH配置データがある場合
	if (!levelediter.GetLevelData()->objects.empty()) {
		for (auto& object : levelediter.GetLevelData()->objects) {
			//中心座標
			Vector3 position = object.translation;
			//aabbの大きさ
			AABB aabb;
			aabb.min = position + object.colliderAABB.min;
			aabb.max = position + object.colliderAABB.max;

			stagesAABB.push_back(aabb);
		}
	}

	//ステージオブジェクトの配置データがあるとき
	if (!levelediter.GetLevelData()->stageObjects.empty()) {
		for (auto& stageObjectData : levelediter.GetLevelData()->stageObjects) {
			std::unique_ptr<IStageObject> stageObject;
			//ステージオブジェクトの名前で変更する
			if (stageObjectData.ObjectName == "WarpGate") {
				//
				stageObject = std::make_unique<WarpGate>();
				stageObject->SetNextStage(stageObjectData.fileName);
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				stageObject = std::make_unique<CheckPoint>();
			}
			else if (stageObjectData.ObjectName == "Goal") {
				stageObject = std::make_unique<Goal>();
			}
			stageObject->SetObjectName(stageObjectData.ObjectName);//オブジェクトの名前保存
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

	//チュートリアル用の
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
	//チュートリアルの出る順番
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
	//現在のシーンと次のシーンが違う場合(例: Select , Game true / Select , Select false)
	if (sceneNo != nextSceneNo) {
		return true;
	}
	return false;
}

void IScene::ChangeScene() {
	//ステージの変更
	sceneNo = nextSceneNo;
}