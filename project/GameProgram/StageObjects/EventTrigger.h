#pragma once
#include "MyMath.h"
#include "sstream"
#include "IEnemy.h"

#include "Enemy_Bomb.h"
#include "Enemy_Soldier.h"
#include "Enemy_Turret.h"

/// <summary>
/// イベントの構造体
/// </summary>
struct EventData {
	bool isEvent = false; //イベント発動フラグ
	AABB aabb;            //イベント範囲
	Vector3 center;       //真ん中
	Vector3 size;         //サイズ
	std::string csvFile;  //csvファイル
	std::string cameraName;
};

struct EnemyPopData {
	Vector3 position = {0,0,0};
	Vector3 rotate = {0,0,0};
	std::string enemyName;
};

class EventTrigger {
public:

	void Initialize();
	void Update();
	void Draw();

	/// <summary>
	/// .csvを読み取る
	/// </summary>
	/// <param name="file"></param>読み込む.csv
	void LoadEventCSV(std::string file);

	/// <summary>
	/// 敵をcsvで召喚設備
	/// </summary>
	/// <param name="eventTrigger"></param>
	void PopEventEneies();

	/// <summary>
	/// 敵の召喚処理
	/// </summary>
	/// <param name="position"></param>
	/// 召喚位置
	/// <param name="position"></param>
	/// 向き
	/// <param name="name"></param>
	/// 敵の名前
	void EnemyPop();

	/// <summary>
	/// イベント内でプレイヤーが倒れたれたとき
	/// </summary>
	void FailureEvent();

	/// <summary>
	/// eventDataを導入(Levelediterで設定したやつの代入に使う)
	/// </summary>
	/// <param name="data"></param>
	void SetEventData(EventData data) { eventDatas = data; }

	/// <summary>
	/// eventDataの関数を使う
	/// </summary>
	/// <returns></returns>
	EventData GetEventData() { return eventDatas; }

	/// <summary>
	/// イベント開始
	/// </summary>
	void StartEvent() { eventDatas.isEvent = true; }

	/// <summary>
	/// GameSceneにある
	/// </summary>
	/// <param name="E"></param>
	void SetPopEnemies(std::vector<std::shared_ptr<IEnemy>> E) { popEnemies = E; }
	std::vector<std::shared_ptr<IEnemy>> GetPopEnemy() { return popEnemies; }

	/// <summary>
	/// イベント終了
	/// </summary>
	/// <returns></returns>イベント終了フラグ
	bool EventEnd() { return eventEnd; }

private:

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt;

	std::stringstream enemyPopCsvFile;

	bool eventWave = false;
	bool isLoadCsv = true;

	bool eventEnd = false;

	//敵を生んだ・倒した数
	uint32_t enemyBornCount = 0;
	uint32_t enemyDeadCount = 0;

	std::vector<std::shared_ptr<IEnemy>> popEnemies;

	EventData eventDatas;

	//パーティクル
	std::list<std::unique_ptr<Particle>> summon_particles_;
	//召喚タイマー
	const float kSummonMaxTime = 1.25f;
	float summonTimer_ = kSummonMaxTime;

	//敵を配置する変数
	std::list<EnemyPopData> enemyPopDatas_;

	//頻度
	const float kFrequency = kSummonMaxTime / 15.0f;

	//秒数時間
	const float kDeltaTime = 1.0f / 60.f;
};