#include "GameScene.h"
#include <sstream>
using namespace MyMath;

void GameScene::Initialize() {
	//ゲームデータ引継ぎ(Hp,ステージ面)
	sceneSaveData_ = NextStageSave::GetInstance()->GetNextStageSaveData();

	//ゲームオブジェクト配置
	LevelEditorObjectSetting();

	//BGM、SEの設定
	BGMData_ = Audio::GetInstance()->LoadWave("resource/sound/title.wav");
	soundData_ = Audio::GetInstance()->LoadWave("resource/sound/bane.wav");

	//BGM再生(リピート)
	Audio::GetInstance()->SoundPlayWave(BGMData_, volume_, true);

	//スタート演出
	WarterWarpExit();

	//演出時に重力が発動しないようにする
	for (auto& enemy : enemies_) {
		enemy->IsPerformanceFlag(true);
	}

	//フェードスタート
	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
	//ワープやゴールのフラグをオフ
	CollisionManager::GetInstance()->ResetFlag();
}

void GameScene::Update() {
	
	if (input_->TriggerKey(DIK_F2)) {
		NextSceneFadeInStart("Clear");
	}

	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
		Audio::GetInstance()->StopWave(BGMData_);//BGM停止
	}
	//演出用のワープゲート出口
	startWarp_->Update();

	//カメラコントロール
	cameraControl_->SetPlayerPosition(player_->GetTranslate());
	//プレイヤーが倒されたらシェイク
	(player_->GetIsDead()) ? cameraControl_->ShakeMode(true) : cameraControl_->ResetShakeTime();
	//更新処理
	cameraControl_->Update(&*camera_.get());

	//プレイヤーがゴールした
	PlayerGoal();

	//プレイヤー更新処理
	player_->Update();
	
	//次のシーンに移動する演出
	WarpNextScene("NextStage");

	//ステージの更新処理
	stageobj_->Update();

	if (isStartStage_) {
		//プレイヤー配置座標 + 地面当たり判定によって上げられる分
		if (startPointY_ >= playerPoint_.y) {
			player_->SetTranslate({ playerPoint_.x,startPointY_,playerPoint_.z });
			isStartStage_ = false;
			player_->IsPerformanceFlag(false);//演出モードを終了し操作できるように
			player_->IsJumping();//強制的にジャンプさせて飛び出たようにする
			//演出フラグ解除
			for (auto& enemy : enemies_) {
				enemy->IsPerformanceFlag(false);
			}
			return;
		}

		startPointY_ += kPlayerUp_;
		player_->SetTranslate({ playerPoint_.x,startPointY_,playerPoint_.z });
	}

	Respawn();

	//敵やオブジェクトを止める(時間停止)
	if (player_->GetIsDead()) {
		return;
	}
	
	for (auto& enemy : enemies_) {
		enemy->SetPlayer(player_.get());
		enemy->Update();
	}

	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}

	for (auto& eventTrigger : eventTriggers_) {

		if (eventTrigger->GetEventData().isEvent) {
			eventTrigger->SetPopEnemies(enemies_);
			eventTrigger->Update();

			enemies_ = std::move(eventTrigger->GetPopEnemy());
		}
	}

	if (!isStartStage_) {
		startWarp_->Vanish();//出てきた後消えるようにする	
	}

	CollisionCommon();

	//落ちた場合
	if (player_->GetTranslate().y < kFallEndY_) {
		player_->IsFall();
	}

	//ガイド更新処理
	UpdateGuide();

#ifdef  USE_IMGUI

	ImGui::Begin("camera");

	//カメラの端
	ImGui::Text("p1 : %f %f %f", cameraPointLeft_.x, cameraPointLeft_.y, cameraPointLeft_.z);
	ImGui::Text("p2 : %f %f %f", cameraPointRight_.x, cameraPointRight_.y, cameraPointRight_.z);

	ImGui::SliderFloat("volume", &volume_, 0.0f, 1.0f);


	ImGui::End();

#endif //  USE_IMGUI

	Audio::GetInstance()->ControlVolume(BGMData_, volume_);
}

void GameScene::Draw() {
	
	Cubemap::GetInstance()->Command();
	//skyBox->Draw();//見ずらいため一度コメントアウト
	

	//モデル描画処理
	GLTFCommon::GetInstance()->Command();
	
	for (auto& eventTrigger : eventTriggers_) {
		eventTrigger->Draw();
	}

	//モデル描画処理
	Object3dCommon::GetInstance()->Command();

	stageobj_->Draw();


	for (auto& enemy : enemies_) {
		enemy->Draw();
		enemy->DrawCommon();
	}

	for (auto& stageObject : stageObjects_) {
		stageObject->Draw();
	}
	
	startWarp_->Draw();
	
	player_->Draw();

	//パーティクル描画処理
	ParticleCommon::GetInstance()->Command();
	for (auto& enemy : enemies_) {
		enemy->DrawParticle();
	}
	player_->DrawP();

	//スプライト描画処理(UI用)
	SpriteCommon::GetInstance()->Command();
	//説明ガイド
	DrawGuide();
}

void GameScene::Finalize() {}


void GameScene::LevelEditorObjectSetting(const std::string& leveleditor_file) {

	//- プレイヤー配置 -
	player_ = std::make_unique<Player>();

	stageFileName_ = sceneSaveData_.nextStageFile;//ステージの全体層(.obj)

	//値が入っている場合
	if (leveleditor_file != "") {
		//代入
		stageFileName_ = leveleditor_file;
		sceneSaveData_.playerHp = player_->GetMaxHp();
	}

	//ステージのjsonを読み取る
	levelediter_.LoadLevelediter("resource/Levelediter/" + stageFileName_ + ".json");

	//- カメラ配置 -
	camera_ = std::make_unique<Camera>();
	//カメラコントロール設定
	cameraControl_ = std::make_unique<CameraControl>();
	cameraControl_->Initialize();//初期化
	//メインのカメラ
	cameraControl_->CameraSetting(levelediter_.GetLevelData()->cameraInit["MainCamera"], false);

	//各デフォルトカメラの設定
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());
	GLTFCommon::GetInstance()->SetDefaultCamera(camera_.get());
	ParticleCommon::GetInstance()->SetDefaultCamera(camera_.get());
	DebugWireframes::GetInstance()->SetDefaultCamera(camera_.get());
	Cubemap::GetInstance()->SetDefaultCamera(camera_.get());

	//プレイヤーの体力を上書き
	player_->SetHp(sceneSaveData_.playerHp);
	player_->SetZanki(sceneSaveData_.playerZanki);
	player_->Initialize();//初期設定

	//プレイヤー配置データがあるときプレイヤーを配置
	if (!levelediter_.GetLevelData()->players.empty()) {
		//一番最初のデータ位置に配置する
		auto& playerData = levelediter_.GetLevelData()->players[0];
		player_->SetTranslate(playerData.translation);//座標
		player_->SetRotate(playerData.rotation);//向き
		player_->SetUmbrellaRotate();//傘の向き
		player_->SetAABB(playerData.colliderAABB);//当たり判定
		//初期状態(位置、回転)設定
		player_->SetInit_Position(playerData.translation, playerData.rotation);
	}

	//- 敵配置 -
	//敵配置データがあるとき
	if (!levelediter_.GetLevelData()->spawnEnemies.empty()) {
		for (auto& enemyData : levelediter_.GetLevelData()->spawnEnemies) {

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
			enemies_.push_back(std::move(enemy));
		}
	}

	//- イベントトリガー配置 -
	//イベントトリガー配置データがあるとき
	if (!levelediter_.GetLevelData()->eventTriggerDatas.empty()) {
		for (auto& eventTriggerData : levelediter_.GetLevelData()->eventTriggerDatas) {
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

			eventTriggers_.push_back(std::move(eventTrigger));
		}
	}

	//- ステージ全体の当たり判定設定 -

	//ステージ自体の見た目
	stageobj_ = std::make_unique<Object3d>();
	stageobj_->Initialize();
	stageobj_->SetModelFile(stageFileName_ + ".obj");

	//MESH配置データがある場合
	if (!levelediter_.GetLevelData()->objects.empty()) {
		for (auto& object : levelediter_.GetLevelData()->objects) {
			//中心座標
			Vector3 position = object.translation;
			//aabbの大きさ
			AABB aabb;
			aabb.min = position + object.colliderAABB.min;
			aabb.max = position + object.colliderAABB.max;

			stagesAABB_.push_back(aabb);
		}
	}

	//- ステージオブジェクト(ギミック)配置 -
	//ステージオブジェクトの配置データがあるとき
	if (!levelediter_.GetLevelData()->stageObjects.empty()) {
		for (auto& stageObjectData : levelediter_.GetLevelData()->stageObjects) {
			std::unique_ptr<IStageObject> stageObject;
			//ステージオブジェクトの名前で変更する
			if (stageObjectData.ObjectName == "WarpGate") {
				//ワープゲート
				stageObject = std::make_unique<WarpGate>();
				stageObject->SetNextStage(stageObjectData.fileName);//次のステージ用の.json名
			}
			else if (stageObjectData.ObjectName == "Checkpoint") {
				//チェックポイント
				stageObject = std::make_unique<CheckPoint>();
			}
			else if (stageObjectData.ObjectName == "Goal") {
				//ゴール
				stageObject = std::make_unique<Goal>();
			}
			stageObject->SetObjectName(stageObjectData.ObjectName);//オブジェクトの名前保存
			stageObject->Initialize();//初期化
			stageObject->SetPosition(stageObjectData.translation);//座標位置
			stageObject->SetAABB(stageObjectData.colliderAABB);//AABB

			stageObjects_.push_back(std::move(stageObject));
		}
	}

	//敵がステージ全体当たり判定をもらう(プレイヤーを見つける処理に使う)
	for (auto& enemy : enemies_) {
		enemy->SetStages(stagesAABB_);
	}

	//チュートリアル用の操作方法スプライト
	if (stageFileName_ == "stage_0" || stageFileName_ == "stage_select") {
		CreateGuide(kGuideMove_);
		CreateGuide(kGuideJump_);
		CreateGuide(kGuideFire_);
		CreateGuide(kGuideshield_);
		CreateGuide(kGuidebrink_);
		CreateGuide(kGuideKakku_);
		CreateGuide(kGuideWarp_);
	}
}

void GameScene::CreateGuide(const Guide& guide) {
	std::unique_ptr<Sprite>& iterator = spriteGuides_[guide.name];
	//guideを設定
	iterator = std::make_unique<Sprite>();
	iterator->Initialize(guide.name + ".png");//初期化
	iterator->SetSize(kSpriteSize_);          //サイズ設定
	iterator->SetPosition(kSpriteTranslate_); //座標設定

	//Guide構造体をvectorにまとめる
	guides_.push_back(guide);
}

void GameScene::UpdateGuide() {
	for (auto& sprite : spriteGuides_) {
		sprite.second->Update();
	}
}

void GameScene::DrawGuide() {
	//チュートリアルの出る順番
	if (stageFileName_ == "stage_select") {
		spriteGuides_[kGuideMove_.name]->Draw();
		spriteGuides_[kGuideWarp_.name]->Draw();
	}
	else if (stageFileName_ == "stage_0") {
		for (auto& guide : guides_) {
			//プレイヤーの座標Xがガイドの設定した左端と右端の間にいるか
			if (player_->GetTranslate().x >= guide.lookPointX_left &&
				player_->GetTranslate().x < guide.lookPointX_right) {
				spriteGuides_[guide.name]->Draw();
			}
		}
	}
}

void GameScene::WarpNextScene(const std::string& nextScene) {
	//ワープするときじゃないなら
	if (!CollisionManager::GetInstance()->IsWarp()) {
		return;
	}

	//プレイヤーが演出判定でない
	//「!player_->GetPerformanceMode()」は何度もplayer_のGetTranslateを読み取ることで予定の速度より速くならないようにするため
	if (!player_->GetPerformanceMode()) {
		//プレイヤーにカメラズーム
		CameraZoomPlayer();
		player_->BackDirection();//向きを前に(Z方向)
	}
	//カメラがズームし終わった
	if (cameraControl_->ZoomEnd()) {
		//次のステージに進む時Hpなどパラメータがリセットされないようにする
		sceneSaveData_.playerHp = player_->GetHp(); //現在のプレイヤー体力を保存
		sceneSaveData_.playerZanki = player_->GetZanki(); //現在のプレイヤー残機を保存
		NextStageSave::GetInstance()->SetPlayerParameta(sceneSaveData_); //移行データを代入する
		//フェードインした後、次のシーンに
		NextSceneFadeInStart(nextScene);
	}
}

void GameScene::PlayerGoal() {
	//ゴールしてないなら
	if (!CollisionManager::GetInstance()->IsGoal()) {
		return;
	}
	//プレイヤーにカメラズーム
	CameraZoomPlayer();           //何度も読み取ってワープより早く移動する
	player_->DirectionTheCamera();//向きをカメラのほうに(-Z方向)

	if (cameraControl_->ZoomEnd()) {
		NextSceneFadeInStart("Clear");//クリアシーンに移動
	}
}

void GameScene::CameraZoomPlayer() {
	//ズーム開始(カメラ現在地点 -> プレイヤー座標 + 少し離れた場所)
	cameraControl_->ZoomStart(player_->GetTranslate() + kPlayerAwayPos_);
	player_->IsPerformanceFlag(true);//演出モード
}

void GameScene::CollisionCommon() {
	//ゲーム内で使用する当たり判定
	CollisionManager::GetInstance()->PlayerAndEnemy(player_.get(), enemies_);
	CollisionManager::GetInstance()->PlayerAndStage(player_.get(), stagesAABB_);
	CollisionManager::GetInstance()->PlayerAndStageObject(player_.get(), stageObjects_);
	
	CollisionManager::GetInstance()->PlayerAndEventTrigger(player_.get(), 
		eventTriggers_, cameraControl_.get(),levelediter_);

	CollisionManager::GetInstance()->EnemyAndStage(enemies_,stagesAABB_);

}









void GameScene::WarterWarpExit() {
	
	//初期化
	startWarp_ = std::make_unique<WarpGate>();
	startWarp_->Initialize();
	isStartStage_ = true;

	playerPoint_ = player_->GetTranslate();
	startPointY_ = playerPoint_.y + startPointY_;//プレイヤーが真下からくるように設定する

	//ワープゲート出口の位置決め
	Vector3 warpPosition = player_->GetTranslate();

	//当たり判定
	AABB startWarpAABB;
	startWarpAABB.max = warpPosition;
	startWarpAABB.min = warpPosition + Vector3{ 0,startPointY_,0 };

	//プレイヤー初期位置の真下に
	warpPosition = CollisionManager::GetInstance()->UnderCollision(stagesAABB_, startWarpAABB, playerPoint_);
	warpPosition.y += kWarpGateUpThanShadow_;//重ならないように影より上にする

	startWarp_->SetPosition(warpPosition);//playerの真下に
	startWarp_->SetRotation({ kStartWarpGateRange_ ,0.0f,0.0f });//下向きにして水たまりに

	player_->IsPerformanceFlag(true);
}

void GameScene::Respawn() {
	//死んでしまった、復活(リスポーン)する時
	if (player_->GetIsDead() && player_->GetIsRespawn()) {

		if (player_->GetZanki() == 0) {
			//残機が0で倒された場合ゲームオーバー
			NextSceneFadeInStart("GameOver");
			FadeScreen::GetInstance()->SetMaskTexture("fade02.png");
			FadeScreen::GetInstance()->SetBackGround("black.png");
			return;
		}

		//敵が復活
		for (auto& enemy : enemies_) {
			enemy->RespawnEnemy();
		}
		//プレイヤー復活
		player_->RespawnPlayer();
		//突破できてないならやり直し
		for (auto& eventTrigger : eventTriggers_) {
			eventTrigger->FailureEvent();
			cameraControl_->CameraSetting(levelediter_.GetLevelData()->cameraInit["MainCamera"], false);
		}
	}
}