#include "GameScene.h"
#include <sstream>
using namespace MyMath;

void GameScene::Initialize() {

	LevelEditorObjectSetting("resource/Levelediter/stage_1.json");

	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile("stage_1.obj");

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");

	wt.Initialize();

	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	Audio::GetInstance()->SoundPlayWave(BGMData_, 0.3f, true);


}

void GameScene::Update() {

	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();
		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			if (isGameClear) {
				//ゲームクリアシーンに移動
				sceneNo = Clear;
				Audio::GetInstance()->StopWave(BGMData_);
			}
			else if (isGameOver) {
				//ゲームオーバーシーンに移動
				sceneNo = GameOver;
				Audio::GetInstance()->StopWave(BGMData_);
			}
			else if (isNextStage) {
				StageMovement("resource/Levelediter/" + nextStage_fileName + ".json", nextStage_fileName + ".obj");
				player_->SpriteUpdate();
				isNextStage = false;
				zumuTimer = 0.0f;
			}
			isfadeStart = false;
		}
	}
	else {
		FadeScreen::GetInstance()->FedeOut();
	}

	if (isNextStage) {
		if (zumuTimer <= 1.0f) {
			zumuTimer += 1.0f / 60.0f;

		}
		camera->Zumu(cameraSegment, zumuTimer);

		if (zumuTimer >= 1.0f) {
			isfadeStart = true;
		}
	}

	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//ワープゲート
			if (stageObject.get() == dynamic_cast<WarpGate*>(stageObject.get()) && Input::GetInstance()->TriggerKey(DIK_E)) {
				WarpGate* warpGate = dynamic_cast<WarpGate*>(stageObject.get());
				cameraSegment.origin = camera->GetTranslate();//ズーム前のカメラ位置
				cameraSegment.diff = player_->GetTranslate() + Vector3(0, 2, -15.0f);//プレイヤーよりちょっと離れてる
				isNextStage = true;
				nextStage_fileName = warpGate->GetNextStage();
				break;
			}//ゴール
			else if (stageObject.get() == dynamic_cast<Goal*>(stageObject.get())) {
				isGameClear = true;
				isfadeStart = true;
				return;
			}
		}
	}

	skyBox->Update(wt.matWorld_ * MakeScaleMatrix({ 1000,1000,1000 }));//大きくするため

	camera->Update();

	player_->Update();

	if (isNextStage) {
		player_->IsAnimationOnlyUpdate();
		return;
	}
	
	for (auto& enemy : enemies) {
		enemy->SetPlayer(player_.get());
		enemy->Update();
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Update();
	}

	bool isChangeRespown = false;
	//リスポーン変更した時
	for (auto& stageObject : stageObjects) {
		if (IsCollisionAABB(player_->GetAABB(), stageObject->GetAABB())) {
			//チェックポイント
			if (stageObject.get() == dynamic_cast<CheckPoint*>(stageObject.get())) {
				CheckPoint* checkPoint = dynamic_cast<CheckPoint*>(stageObject.get());
				player_->SetRespownPosition(checkPoint->GetPosition());
				isChangeRespown = true;
			}
		}
	}

	//共有イベントフラグ
	bool isEventCommon = false;

	for (auto& eventTrigger : eventTriggers) {
		if (eventTrigger.isEvent) {
			if (isLoadCsv) {
				//Csvを読み込む
				LoadEventCSV(eventTrigger.csvFile);
			}
			//敵召喚
			PopEventEneies(&eventTrigger);
			isEventCommon = eventTrigger.isEvent;
		}
	}

	//プレイヤーが死んで、リスポーン地点が変更していないとき敵は復活する
	if (player_->GetIsPlayerDown() && player_->GetIsRespown()) {

		RemainingLife--;
		if (RemainingLife != 0) {
			for (auto& enemy : enemies) {
				enemy->RespownEnemy();
			}
			player_->AllRespownEnd();
		}
		else {
			isGameOver = true;
			isfadeStart = true;
		}

		//強制イベント中の場合
		if (isEventCommon) {
			//イベント終了
			isEventCommon = false;
			eventWave = false;
			//次に読み込めるように
			isLoadCsv = true;
			//カメラを元(メインカメラ)に戻す
			MainCamera();

			//召喚敵を最後尾から消す
			for (uint32_t i = 0; i < enemyBornCount; i++) {
				enemies.pop_back();
			}

			enemyBornCount = 0;

			//リセット
			enemyPopCsvFile.clear();
			//最初の行にする
			enemyPopCsvFile.seekg(0, std::ios_base::beg);

		}
	}
	
	CollisionCommon();

	//
	if (Input::GetInstance()->TriggerKey(DIK_F2)) {
		sceneNo = Clear;
		Audio::GetInstance()->StopWave(BGMData_);
	}


	//リスポーン地点を変更前に倒した敵は復活しない
	//if (isChangeRespown) {
	//	//敵を倒したら削除
	//	enemies.remove_if([](IEnemy* enemy) {
	//		if (enemy->GetDeleteEnemy()) {
	//			delete enemy;
	//			return true;
	//		}
	//		return false;
	//		});
	//	isChangeRespown = false;
	//}

	//カメラの移動範囲

	
	//イベント中はカメラが固定
	if (isEventCommon) {
		//カメラ固定
		worldTransformCamera_.rotation_ = cameraRotate;
		worldTransformCamera_.translation_ = cameraTranslate;
	}
	//次ステージ移動時はズームされるのでここは除外
	else if (!isNextStage) {
		if (cameraTranslate.x + cameraPoint1.x < player_->GetTranslate().x && cameraTranslate.x + cameraPoint2.x > player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = player_->GetTranslate().x;
		}
		else if (cameraTranslate.x + cameraPoint1.x >= player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint1.x;
		}
		else if (cameraTranslate.x + cameraPoint2.x <= player_->GetTranslate().x) {
			worldTransformCamera_.translation_.x = cameraTranslate.x + cameraPoint2.x;
		}

		if (cameraTranslate.y < player_->GetTranslate().y + 6.0f) {
			worldTransformCamera_.translation_.y = player_->GetTranslate().y + 6.0f;
		}
		else {
			worldTransformCamera_.translation_.y = cameraTranslate.y;
		}	
	}


	if (player_->GetTranslate().y < -10.0f) {
		player_->IsFall();
	}

	stageobj->Update(wt);

	wt.UpdateMatrix();
	worldTransformCamera_.UpdateMatrix();

	camera->SetRotate(worldTransformCamera_.rotation_);
	camera->SetTranslate(worldTransformCamera_.translation_);


#ifdef  USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &worldTransformCamera_.translation_.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &worldTransformCamera_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &worldTransformCamera_.rotation_.x);
	ImGui::SliderFloat("cameraRotateX", &worldTransformCamera_.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &worldTransformCamera_.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &worldTransformCamera_.rotation_.z, -360.0f, 360.0f);

	ImGui::Text("p1 : %f %f %f", cameraPoint1.x, cameraPoint1.y, cameraPoint1.z);
	ImGui::Text("p2 : %f %f %f", cameraPoint2.x, cameraPoint2.y, cameraPoint2.z);

	camera->SetRotate(worldTransformCamera_.rotation_);
	camera->SetTranslate(worldTransformCamera_.translation_);

	ImGui::SliderFloat("volume", &volume, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI


	Audio::GetInstance()->ControlVolume(BGMData_, volume);
}

void GameScene::Draw() {
	
	Cubemap::GetInstance()->Command();
	skyBox->Draw();
	

	//モデル描画処理
	GLTFCommon::GetInstance()->Command();

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();
	
	stageobj->Draw();

	player_->Draw();
	for (auto& enemy : enemies) {
		enemy->Draw();
	}

	for (auto& stageObject : stageObjects) {
		stageObject->Draw();
	}
	

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();

}

void GameScene::Finalize() {
	enemies.clear();
	stageObjects.clear();
	stagesAABB.clear();
}

void GameScene::StageMovement(const std::string leveleditor_file, const std::string stageObj) {
	playerHp = player_->GetHp();
	Finalize();
	levelediter.ResetData();
	//パーティクルナンバー初期化
	ParticleNum::number = 0;

	LevelEditorObjectSetting(leveleditor_file);	
	
	player_->SetHp(playerHp);

	stageobj = std::make_unique<Object3d>();
	stageobj->Initialize();
	stageobj->SetModelFile(stageObj);

	skyBox = std::make_unique<BoxModel>();
	skyBox->Initialize("resource/rostock_laage_airport_4k.dds");
}

void GameScene::LoadEventCSV(std::string fileName) {

	std::ifstream file;
	
	file.open(fileName);
	assert(file.is_open());

	enemyPopCsvFile << file.rdbuf();//fileをコピー

	file.close();

	//読み込みをなくす
	isLoadCsv = false;
}

void GameScene::PopEventEneies(EventTrigger* eventTrigger) {

	//敵の倒した数リセット(↓で無限に増えるから)
	enemyDeadCount = 0;

	//召喚した敵を倒すカウント
	//召喚は敵配列の最後尾から数える
	for (uint32_t number = (uint32_t)enemies.size() - 1; number >= enemies.size() - enemyBornCount; number--) {
		//召喚した敵を倒した判定
		if (enemies[number]->GetDeleteEnemy()) {
			enemyDeadCount++;//倒した数だけプラスされる
		}
	}
	
	//倒した数と召喚した数が同じ
	if (enemyDeadCount == enemyBornCount) {
		//次のウェーブに進む
		eventWave = false;	
		//召喚敵を最後尾から消す
		for (uint32_t i = 0; i < enemyBornCount;i++) {
			enemies.pop_back();
		}
		//生んだ数初期化
		enemyBornCount = 0;
	}

	if (eventWave) {
		return;
	}

	std::string line;

	while (getline(enemyPopCsvFile,line)) {

		std::istringstream line_stream(line);
		std::string word;

		getline(line_stream, word, ',');

		//コメントはパス
		if (word.find("//") == 0) {
			continue;
		}

		//終了
		if (word.find("end") == 0) {
			//イベント終了
			eventTrigger->isEvent = false;
			//次に読み込めるように
			isLoadCsv = true;
			//当たり判定を消す
			eventTriggers.erase(eventTriggers.begin());

			//カメラを元(メインカメラ)に戻す
			MainCamera();

			//リセット
			enemyPopCsvFile.clear();

			break;
		}

		//ウェーブの配分
		if (word.find("wave") == 0) {
			eventWave = true;
			break;
		}

		//敵の配置
		if (word.find("pop") == 0) {

			std::string enemyName;
			//敵の名前
			getline(line_stream, word, ',');
			enemyName = word.c_str();

			Vector3 position;
			//召喚位置.x
			getline(line_stream, word, ',');
			position.x = (float)std::atof(word.c_str());

			//召喚位置.y
			getline(line_stream, word, ',');
			position.y = (float)std::atof(word.c_str());

			//トリガーの中心地点から足していく
			position += eventTrigger->center;

			//召喚位置.zは使わないので0に
			position.z = 0.0f;

			Vector3 rotate = { 0,0,0 };

			
			getline(line_stream, word, ',');
			if (word.find("right") == 0) {
				rotate.y = 90.0f;
			}
			else if (word.find("left") == 0) {
				rotate.y = -90.0f;
			}

			//敵召喚
			EnemyPop(position, rotate, enemyName);
		}
	}

}

void GameScene::EnemyPop(const Vector3& position, const Vector3& rotation, const std::string& name) {
	std::unique_ptr<IEnemy> popEnemy;

	if (name == "soldier") {
		popEnemy = std::make_unique<Enemy_Soldier>();
	}
	else if (name == "turret") {
		popEnemy = std::make_unique<Enemy_Turret>();
	}
	else if (name == "bomb") {
		popEnemy = std::make_unique<Enemy_Bomb>();
	}

	popEnemy->Initialize();
	popEnemy->SetTranslate(position);
	popEnemy->SetRotate(rotation);
	
	//敵の当たり判定更新
	AABB aabb;
	aabb.min = { -1.0f,-1.0f,-1.5f };
	aabb.max = { 1.0f,1.0f,1.5f };

	popEnemy->SetAABB(aabb);
	
	enemies.push_back(std::move(popEnemy));
	
	//敵の数
	enemyBornCount++;
}