#include "EventTrigger.h"
#include "UseEveryOne.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

void EventTrigger::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("EventGate.gltf");

	collisionType_ = CollisionTypes::event;
}

void EventTrigger::Update() {
	//当たり判定設定
	collisionAABB_ = eventData_.aabb;

	center_ = transform_.translate;
	CollisionManager::GetInstance().AddCollisions(this);
}

void EventTrigger::EventUpdate() {
	if (isLoadCsv_) {
		//Csvを読み込む
		LoadEventCSV(eventData_.csvFile);
	}
	//敵召喚
	PopEventEnemies();

	//範囲のオブジェクト
	transform_.translate = eventData_.center;
	transform_.scale = eventData_.size * kDivideByTwo_;
	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	for (auto& particle : summon_particles_) {
		//敵が出るまで
		(summonTimer_ > 0) ? particle->SetParticleBorn(ParticleBorn::TimerMode) : particle->SetParticleBorn(ParticleBorn::Stop);
		particle->Update();
	}
	//敵が出終わった後ちょっとしてからリセットする
	if (summonTimer_ <= -particleSummon_.frequency * kTwice_) {
		summon_particles_.clear();
	}

	EventData data = GetEventData();
}

void EventTrigger::Draw() {
	if (eventData_.isEvent) {
		object_->Draw();
	}
}

void EventTrigger::DrawParticle() {
	for (auto& particle : summon_particles_) {
		particle->Draw();
	}
}

void EventTrigger::LoadEventCSV(const std::string& fileName) {

	std::ifstream file;

	//ファイルを開く
	file.open(fileName);
	assert(file.is_open());

	enemyPopCsvFile_ << file.rdbuf();//fileをコピー

	//ファイルを閉じる
	file.close();

	//読み込み処理をはぶく
	isLoadCsv_ = false;
}

void EventTrigger::PopEventEnemies() {
	//敵召喚		
	EnemyPop();

	if (enemyPopDatas_.size() != 0) {
		return;
	}

	//ウェーブバトル
	WaveEnemyCount();

	//ウェーブ中はcsv読み取りを進行しない
	if (isEventWave_) {
		return;
	}

	//csvを読み取る
	LoadCsvWord();
}

void EventTrigger::WaveEnemyCount() {
	//敵の倒した数リセット(↓で無限に増えるから)
	enemyDeadCount_ = 0;

	//召喚した敵を倒すカウント
	//召喚は敵配列の最後尾から数える
	for (uint32_t number = (uint32_t)popEnemies_.size() - 1;
		number >= popEnemies_.size() - enemyBornCount_; number--) {
		//召喚した敵を倒した判定
		if (popEnemies_[number]->GetDeleteEnemy()) {
			enemyDeadCount_++;
		}
	}

	//倒した数と召喚した数が同じ
	if (enemyDeadCount_ == enemyBornCount_) {
		//次のウェーブに進む
		isEventWave_ = false;
		//召喚敵を最後尾から消す
		for (uint32_t i = 0; i < enemyBornCount_; i++) {
			popEnemies_.pop_back();
		}
		//生んだ数初期化
		enemyBornCount_ = 0;
		//Maxに戻す
		summonTimer_ = kSummonMaxTime_;
	}
}

void EventTrigger::LoadCsvWord() {
	std::string line;

	while (getline(enemyPopCsvFile_, line)) {

		std::istringstream line_stream(line);
		std::string word;

		getline(line_stream, word, ',');

		//コメントはパス
		if (word.find("//") == 0) {
			continue;
		}

		//終了
		if (word.find("end") == 0) {
			eventData_.isEvent = false;
			//イベント終了
			isEventEnd_ = true;
			break;
		}

		//ウェーブの配分
		if (word.find("wave") == 0) {
			isEventWave_ = true;
			break;
		}

		//敵の配置
		if (word.find("pop") == 0) {
			LoadPopEnemy(line_stream, word);
		}
	}
}


void EventTrigger::LoadPopEnemy(std::istringstream& line_stream, std::string& word) {

	EnemyPopData enemyPopData;
	//敵の名前
	getline(line_stream, word, ',');
	enemyPopData.enemyName = word.c_str();

	//召喚位置.x
	getline(line_stream, word, ',');
	enemyPopData.position.x = (float)std::atof(word.c_str());

	//召喚位置.y
	getline(line_stream, word, ',');
	enemyPopData.position.y = (float)std::atof(word.c_str());

	//召喚位置.z
	getline(line_stream, word, ',');
	enemyPopData.position.z = (float)std::atof(word.c_str());

	//トリガーの中心地点から足していく
	enemyPopData.position += eventData_.center;

	getline(line_stream, word, ',');
	if (word.find("right") == 0) {
		enemyPopData.rotate.y = kDirectionRight_;
	}
	else if (word.find("left") == 0) {
		enemyPopData.rotate.y = kDirectionLeft_;
	}

	enemyPopDatas_.push_back(enemyPopData);

	//召喚パーティクル
	particles_[particleSummon_.name] = ParticleManager::GetInstance().InitParticle(particleSummon_);
	particles_[particleSummon_.name]->SetTranslate(enemyPopData.position);
	summon_particles_.push_back(std::move(particles_[particleSummon_.name]));
}

void EventTrigger::EnemyPop() {
	summonTimer_ -= kDeltaTime_;

	if (summonTimer_ > 0.0f) {
		return;
	}

	for (auto& enemyPopData : enemyPopDatas_) {

		std::unique_ptr<BaseEnemy> popEnemy;
		//名前によって変更
		if (enemyPopData.enemyName == "soldier") {
			popEnemy = std::make_unique<Enemy_Soldier>();
		}
		else if (enemyPopData.enemyName == "turret") {
			popEnemy = std::make_unique<Enemy_Turret>();
		}
		else if (enemyPopData.enemyName == "bomb") {
			popEnemy = std::make_unique<Enemy_Bomb>();
		}

		popEnemy->Initialize();
		popEnemy->SetTranslate(enemyPopData.position);
		popEnemy->SetRotate(enemyPopData.rotate);

		//少しだけ動けるように
		Vector3 move = { kMoveX,0,0 };

		//敵の当たり判定更新
		popEnemy->SetColliderSize(kAABBSize_ * kDivideByTwo_);
		//popEnemy->SetRouteLeftPoint(enemyPopData.position - move);
		//popEnemy->SetRouteRightPoint(enemyPopData.position + move);
		//popEnemy->SetMoveInit(enemyPopData.position);

		popEnemy->DirectionDegree();

		popEnemies_.push_back(std::move(popEnemy));

		//敵の数
		enemyBornCount_++;
	}

	enemyPopDatas_.clear();
}

void EventTrigger::FailureEvent() {
	//ウェーブフラグ
	isEventWave_ = false;
	//また最初からcsvを読み込めるように
	isLoadCsv_ = true;
	//イベントをoffにする
	eventData_.isEvent = false;
	//生まれたカウントリセット
	enemyBornCount_ = 0;

	//リセット
	enemyPopCsvFile_.clear();	
	
	//最初の行にする
	enemyPopCsvFile_.seekg(0, std::ios_base::beg);

}

void EventTrigger::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::player) {
		//cameraControl_->CameraInterpolation(levelEditor.GetLevelData()->cameraInit[data.cameraName], true);
		StartEvent();
	}

}

//
//void CollisionManager::PlayerAndEventTrigger(Player* player, const std::vector<std::shared_ptr<EventTrigger>>& eventTriggers,
//	CameraControl* cameraControl_, LevelEditor& levelEditor) {
//	CollisionOverlap playerCollisionOverlap;
//	playerCollisionOverlap = SetTarget(player->GetTranslate(), player->GetAABB());
//
//	// - イベントトリガー -
//	for (auto& eventTrigger : eventTriggers) {
//		EventData data = eventTrigger->GetEventData();
//
//		//イベントが終了した時(順番3)
//		if (eventTrigger->EventEnd()) {
//			//一瞬だけ通す
//			if (cameraControl_->IsFixed()) {
//				cameraControl_->FixedMode(false);//カメラを固定しない
//				cameraControl_->CameraInterpolation(levelEditor.GetLevelData()->cameraInit["MainCamera"], false);//メインカメラに戻す
//			}
//		}
//		//イベントが発動している時(順番2)
//		else if (data.isEvent) {
//			Vector3 move_range = player->GetTranslate();
//			Vector3 size = player->GetScale();
//			//動ける範囲制限
//			move_range.x = std::clamp(move_range.x, data.aabb.min.x + size.x, data.aabb.max.x - size.x);
//			move_range.y = std::clamp(move_range.y, data.aabb.min.y + size.y, data.aabb.max.y - size.y);
//			move_range.z = std::clamp(move_range.z, data.aabb.min.z + size.z, data.aabb.max.z - size.z);
//			player->SetTranslate(move_range);
//		}
//		//イベントトリガーに入った直前(順番1)
//		else if (IsCollisionAABB(player->GetAABB(), data.aabb)) {
//			cameraControl_->CameraInterpolation(levelEditor.GetLevelData()->cameraInit[data.cameraName], true);
//			eventTrigger->StartEvent();
//		}
//	}
//}
