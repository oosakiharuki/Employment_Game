#pragma once
#include "MyMath.h"
#include "sstream"
#include "BaseEnemy.h"

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

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画処理_パーティクル
	/// </summary>
	void DrawParticle();

	/// <summary>
	/// .csvを読み取る
	/// </summary>
	/// <param name="file"></param>読み込む.csv
	void LoadEventCSV(const std::string& file);

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
	/// eventDataを導入(LevelEditorで設定したやつの代入に使う)
	/// </summary>
	/// <param name="data"></param>
	void SetEventData(const EventData& data) { eventData_ = data; }

	/// <summary>
	/// eventDataの関数を使う
	/// </summary>
	/// <returns></returns>
	EventData GetEventData() { return eventData_; }

	/// <summary>
	/// イベント開始
	/// </summary>
	void StartEvent() { eventData_.isEvent = true; }

	/// <summary>
	/// GameSceneにある
	/// </summary>
	/// <param name="E"></param>
	void SetPopEnemies(const std::vector<std::shared_ptr<BaseEnemy>>& enemies) { popEnemies_ = enemies; }
	std::vector<std::shared_ptr<BaseEnemy>> GetPopEnemy() const { return popEnemies_; }

	/// <summary>
	/// イベント終了
	/// </summary>
	/// <returns></returns>イベント終了フラグ
	bool EventEnd() { return isEventEnd_; }

private:

	/// <summary>
	/// 召喚した敵を数えて
	/// 全て倒したら次のウェーブに進む処理
	/// </summary>
	void WaveEnemyCount();

	void LoadCsvWord();

	/// <summary>
	/// popを読み込んだ時の処理
	/// </summary>
	/// <param name="line_stream">コードライン</param>
	/// <param name="word">内容</param>
	void LoadPopEnemy(std::istringstream& line_stream ,std::string& word);

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt_;

	//読み取ったcsv
	std::stringstream enemyPopCsvFile_;

	//ウェーブで一度読み込みを停止する
	bool isEventWave_ = false;
	//csvを読み取る
	bool isLoadCsv_ = true;
	//イベント終了
	bool isEventEnd_ = false;

	//敵を生んだ・倒した数
	uint32_t enemyBornCount_ = 0;
	uint32_t enemyDeadCount_ = 0;

	//登場する敵たち
	std::vector<std::shared_ptr<BaseEnemy>> popEnemies_;

	//イベントのデータ
	EventData eventData_;

	//パーティクル
	std::list<std::unique_ptr<Particle>> summon_particles_;
	//召喚タイマー
	const float kSummonMaxTime_ = 1.25f;
	float summonTimer_ = kSummonMaxTime_;

	//敵を配置する変数
	std::list<EnemyPopData> enemyPopDatas_;

	//パーティクルサイズ
	const float gSize = 0.5f;

	//動ける範囲
	const float kMoveX = 3.0f;

	//AABBのサイズ
	const Vector3 kAABBSize_ = { 2.0f,2.0f,3.0f };

	//向き(左右)
	const float kDirectionRight_ = 90.0f;
	const float kDirectionLeft_ = -90.0f;

	//召喚するとき
	std::unordered_map<std::string, std::unique_ptr<Particle>> particles_;

	ParticleParametars particleSummon_ = {
		"enemies_summon", "resource/Sprite/white.png", Primitive::CreateSphere(), 10,
		kSummonMaxTime_ / 6.0f, { gSize,gSize,gSize }
	};
};