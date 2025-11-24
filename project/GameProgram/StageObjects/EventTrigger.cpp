#include "EventTrigger.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void EventTrigger::Initialize() {
	wt_.Initialize();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("EventGate.gltf");
}

void EventTrigger::Update() {

	if (eventData_.isEvent) {
		if (isLoadCsv_) {
			//Csvを読み込む
			LoadEventCSV(eventData_.csvFile);
		}
		//敵召喚
		PopEventEneies();
		
		//範囲のオブジェクト
		wt_.translation_ = eventData_.center;
		wt_.scale_ = eventData_.size * kDivideByTwo_;
		wt_.UpdateMatrix();
		object_->Update(wt_);

		for (auto& particle : summon_particles_) {
			particle->SetParticleCount(10);
			particle->Update();
			//敵が出てきたので止める
			if (summonTimer_ < 0) {
				particle->SetParticleBorn(ParticleBorn::Stop);
			}
		}

		//敵が出終わった後ちょっとしてからリセットする
		if (summonTimer_ <= -kSummonMaxTime_) {
			summon_particles_.clear();
		}

	}
}

void EventTrigger::Draw() {
	if (eventData_.isEvent) {
		object_->Draw();
	}

	ParticleCommon::GetInstance()->Command();
	
	for (auto& particle : summon_particles_) {
		particle->Draw();
	}

	GLTFCommon::GetInstance()->Command();
}

void EventTrigger::LoadEventCSV(std::string fileName) {

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

void EventTrigger::PopEventEneies() {

	//敵召喚		
	EnemyPop();

	if (enemyPopDatas_.size() != 0) {
		return;
	}

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

	if (isEventWave_) {
		return;
	}

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

			//トリガーの中心地点から足していく
			enemyPopData.position += eventData_.center;

			getline(line_stream, word, ',');
			if (word.find("right") == 0) {
				enemyPopData.rotate.y = kDirectionRight_;
			}
			else if (word.find("left") == 0) {
				enemyPopData.rotate.y = kDirectionRight_;
			}

			enemyPopDatas_.push_back(enemyPopData);

			//召喚パーティクル
			std::unique_ptr<Particle> gParticle;
			gParticle = std::make_unique<Particle>();
			gParticle->Initialize("enemies_summon","resource/Sprite/white.png",PrimitiveType::sphere);
			gParticle->SetParticleMosion(ParticleMosion::Exprosion);
			gParticle->SetParticleBorn(ParticleBorn::TimerMode);
			gParticle->SetFrequency(kFrequency_);
			gParticle->SetTranslate(enemyPopData.position);
			gParticle->SetScale({gSize,gSize,gSize});

			summon_particles_.push_back(std::move(gParticle));
		}
	}

}

void EventTrigger::EnemyPop() {
	summonTimer_ -= kDeltaTime;

	if (summonTimer_ > 0.0f) {
		return;
	}

	for (auto& enemyPopData : enemyPopDatas_) {

		std::unique_ptr<IEnemy> popEnemy;
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

		//敵の当たり判定更新
		AABB aabb;
		aabb.min = -kAABBSize_ * kDivideByTwo_;
		aabb.max = kAABBSize_ * kDivideByTwo_;

		//少しだけ動けるように
		Vector3 move = { kMoveX,0,0 };

		popEnemy->SetAABB(aabb);
		popEnemy->SetRoutePoint1(enemyPopData.position - move);
		popEnemy->SetRoutePoint2(enemyPopData.position + move);
		popEnemy->SetMoveInit(enemyPopData.position);

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