#include "EventTrigger.h"
using namespace MyMath;

void EventTrigger::Initialize() {
	wt.Initialize();

	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("EventGate.gltf");
}

void EventTrigger::Update() {

	if (eventDatas.isEvent) {
		if (isLoadCsv) {
			//Csvを読み込む
			LoadEventCSV(eventDatas.csvFile);
		}
		//敵召喚
		PopEventEneies();
		
		//範囲のオブジェクト
		wt.translation_ = eventDatas.center;
		wt.scale_ = eventDatas.size * 0.5f;
		wt.UpdateMatrix();
		object_->Update(wt);
	}
}

void EventTrigger::Draw() {
	if (eventDatas.isEvent) {
		object_->Draw();
	}
}

void EventTrigger::LoadEventCSV(std::string fileName) {

	std::ifstream file;

	//ファイルを開く
	file.open(fileName);
	assert(file.is_open());

	enemyPopCsvFile << file.rdbuf();//fileをコピー

	//ファイルを閉じる
	file.close();

	//読み込み処理をはぶく
	isLoadCsv = false;
}

void EventTrigger::PopEventEneies() {

	//敵の倒した数リセット(↓で無限に増えるから)
	enemyDeadCount = 0;

	//召喚した敵を倒すカウント
	//召喚は敵配列の最後尾から数える
	for (uint32_t number = (uint32_t)popEnemies.size() - 1;
		number >= popEnemies.size() - enemyBornCount; number--) {
		//召喚した敵を倒した判定
		if (popEnemies[number]->GetDeleteEnemy()) {
			enemyDeadCount++;
		}
	}

	//倒した数と召喚した数が同じ
	if (enemyDeadCount == enemyBornCount) {
		//次のウェーブに進む
		eventWave = false;
		//召喚敵を最後尾から消す
		for (uint32_t i = 0; i < enemyBornCount; i++) {
			popEnemies.pop_back();
		}
		//生んだ数初期化
		enemyBornCount = 0;
	}

	if (eventWave) {
		return;
	}

	std::string line;

	while (getline(enemyPopCsvFile, line)) {

		std::istringstream line_stream(line);
		std::string word;

		getline(line_stream, word, ',');

		//コメントはパス
		if (word.find("//") == 0) {
			continue;
		}

		//終了
		if (word.find("end") == 0) {
			eventDatas.isEvent = false;
			//イベント終了
			eventEnd = true;
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
			position += eventDatas.center;

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

void EventTrigger::EnemyPop(const Vector3& position, const Vector3& rotation, const std::string& name) {
	std::unique_ptr<IEnemy> popEnemy;
	//名前によって変更
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

	//少しだけ動けるように
	Vector3 center = { 3,0,0 };

	popEnemy->SetAABB(aabb);
	popEnemy->SetRoutePoint1(position - center);
	popEnemy->SetRoutePoint2(position + center);
	popEnemy->SetMoveInit(position);

	popEnemy->DirectionDegree();

	popEnemies.push_back(std::move(popEnemy));

	//敵の数
	enemyBornCount++;
}

void EventTrigger::FailureEvent() {
	//ウェーブフラグ
	eventWave = false;
	//次に読み込めるように
	isLoadCsv = true;
	//イベントをoffにする
	eventDatas.isEvent = false;
	//生まれたカウントリセット
	enemyBornCount = 0;

	//リセット
	enemyPopCsvFile.clear();	
	
	//最初の行にする
	enemyPopCsvFile.seekg(0, std::ios_base::beg);

}