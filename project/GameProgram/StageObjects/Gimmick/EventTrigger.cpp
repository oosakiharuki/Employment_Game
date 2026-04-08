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

	collisionType_ = CollisionTypes::TypeEvent;
}

void EventTrigger::Update(CameraControl& cameraControl, LevelEditor& levelEditor , std::vector<std::unique_ptr<BaseEnemy>> enemies) {

	popEnemies_ = std::move(enemies);

	if (eventData_.isEvent) {
		ChangeCamera(cameraControl, levelEditor);	
		EventUpdate();
	}

	ReturnCamera(cameraControl, levelEditor);

	if (isEventEnd_) return;//終わった後当たり判定は作らない

	//当たり判定設定
	collisionAABB_ = eventData_.aabb;

	center_ = transform_.translate;
	CollisionManager::GetInstance().FrameCollision(this);
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
		//発生源をクリアする
		summon_particles_.clear();
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
	particles_[particleSummon_] = ParticleManager::GetInstance().InitParticle(particleSummon_);
	particles_[particleSummon_]->SetTranslate(enemyPopData.position);
	summon_particles_.push_back(std::move(particles_[particleSummon_]));
}

void EventTrigger::EnemyPop() {
	summonTimer_ -= kDeltaTime_;

	if (summonTimer_ > 0.0f) {
		return;
	}

	for (auto& enemyPopData : enemyPopDatas_) {

		
		//名前によって変更
		if (enemyPopData.enemyName == "soldier") {
			std::unique_ptr<Enemy_Soldier> popEnemy = std::make_unique<Enemy_Soldier>();
			EnemyTemplate(*popEnemy, enemyPopData);
			EnemyMoveRoute(*popEnemy, enemyPopData);
			//vectorに代入
			popEnemies_.push_back(std::move(popEnemy));
		}
		else if (enemyPopData.enemyName == "turret") {
			std::unique_ptr<Enemy_Turret> popEnemy = std::make_unique<Enemy_Turret>();
			EnemyTemplate(*popEnemy, enemyPopData);
			//vectorに代入
			popEnemies_.push_back(std::move(popEnemy));
		}
		else if (enemyPopData.enemyName == "bomb") {
			std::unique_ptr<Enemy_Bomb> popEnemy = std::make_unique<Enemy_Bomb>();
			EnemyTemplate(*popEnemy, enemyPopData);
			EnemyMoveRoute(*popEnemy, enemyPopData);
			//vectorに代入
			popEnemies_.push_back(std::move(popEnemy));
		}
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
	if (collision->GetType() == CollisionTypes::TypePlayer) {
		eventData_.isEvent = true;
	}
}

bool EventTrigger::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}

void EventTrigger::ChangeCamera(CameraControl& cameraControl, LevelEditor& levelEditor) {
	cameraControl.CameraInterpolation(levelEditor.GetLevelData()->cameraInit[eventData_.cameraName], true);
}

void EventTrigger::ReturnCamera(CameraControl& cameraControl, LevelEditor& levelEditor) {
	//イベントが終了した時(順番3)
	if (!isEventEnd_) return;


	//一瞬だけ通す
	if (cameraControl.IsFixed()) {
		cameraControl.FixedMode(false);//カメラを固定しない
		cameraControl.CameraInterpolation(levelEditor.GetLevelData()->cameraInit["MainCamera"], false);//メインカメラに戻す
	}
}



void EventTrigger::EnemyTemplate(BaseEnemy& enemy, EnemyPopData enemyData) {
	enemy.Initialize();//初期設定
	enemy.SetTranslate(enemyData.position);//座標
	enemy.SetRotate(enemyData.rotate);//向き
	enemy.SetColliderSize(enemyData.kAABBSize_ * kDivideByTwo_);//当たり判定
	//オブジェクト向き
	enemy.DirectionDegree();
}

void EventTrigger::EnemyMoveRoute(EnemyMoveCommand& enemy, EnemyPopData enemyData) {
	//少しだけ動けるように
	Vector3 move = { kMoveX,0,0 };

	enemy.SetRouteLeftPoint(enemyData.position - move);//移動ポイント1
	enemy.SetRouteRightPoint(enemyData.position + move);//移動ポイント2 (leftPoint < rightPoint)
	enemy.SetMoveInit(enemyData.position);//移動ポイント真ん中

	enemy.DirectionMove();
}
