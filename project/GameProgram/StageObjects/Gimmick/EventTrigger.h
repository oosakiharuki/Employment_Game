#pragma once
#include "MyMath.h"
#include "sstream"
#include "BaseEnemy.h"

#include "Enemy_Bomb.h"
#include "Enemy_Soldier.h"
#include "Enemy_Turret.h"

#include "CollisionManager.h"

#include "CameraControl.h"
#include "LevelEditor.h"

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
	//AABBのサイズ
	const Vector3 kAABBSize_ = { 2.0f,2.0f,3.0f };
	std::string enemyName;
};

/// <summary>
/// 敵ラッシュのイベントトリガー
/// </summary>
class EventTrigger : public CollisionSource {
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(CameraControl& cameraControl,LevelEditor& levelEditor, std::vector<std::unique_ptr<BaseEnemy>> enemies);
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();


	/// <summary>
	/// 更新処理
	/// </summary>
	void EventUpdate();

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
	void PopEventEnemies();

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

	std::vector<std::unique_ptr<BaseEnemy>> GetPopEnemy()  { return std::move(popEnemies_); }

	/// <summary>
	/// イベント終了
	/// </summary>
	/// <returns></returns>イベント終了フラグ
	bool EventEnd() { return isEventEnd_; }

	/// <summary>
	/// イベントカメラに変更
	/// </summary>
	/// <param name="cameraControl">カメラをコントロールしているクラス</param>
	/// <param name="levelEditor">レベルエディタからカメラデータを読み取る</param>
	void ChangeCamera(CameraControl& cameraControl, LevelEditor& levelEditor);

	/// <summary>
	/// メインカメラに戻す
	/// </summary>
	/// <param name="cameraControl">カメラをコントロールしているクラス</param>
	/// <param name="levelEditor">レベルエディタからカメラデータを読み取る</param>
	void ReturnCamera(CameraControl& cameraControl, LevelEditor& levelEditor);

private:

	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

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

	std::unique_ptr<EngineLayer::Object_glTF> object_;
	EngineLayer::WorldTransform wt_;
	Transform transform_{};

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
	std::vector<std::unique_ptr<BaseEnemy>> popEnemies_;

	//イベントのデータ
	EventData eventData_;

	//パーティクル
	std::list<std::unique_ptr<EngineLayer::Particle>> summon_particles_;
	//召喚タイマー
	static constexpr float kSummonMaxTime_ = 1.25f;
	float summonTimer_ = kSummonMaxTime_;

	//敵を配置する変数
	std::list<EnemyPopData> enemyPopDatas_;

	//動ける範囲
	static constexpr float kMoveX = 3.0f;

	//向き(左右)
	static constexpr float kDirectionRight_ = 90.0f;
	static constexpr float kDirectionLeft_ = -90.0f;

	//召喚するとき
	std::unordered_map<std::string, std::unique_ptr<EngineLayer::Particle>> particles_;

	const std::string& particleSummon_ = "enemies_summon";

	/// <summary>
	/// 敵の共通テンプレート処理
	/// </summary>
	/// <param name="enemy">敵の基盤/派生クラスを入れる</param>
	/// <param name="enemyData">LevelEditorからもらった敵データ</param>
	void EnemyTemplate(BaseEnemy& enemy, EnemyPopData enemyData);
	/// <summary>
	/// 移動能力がある敵キャラ
	/// </summary>
	/// <param name="enemy">MoveCommandを持っている敵</param>
	/// <param name="enemyData">LevelEditorからもらった敵データ</param>
	void EnemyMoveRoute(EnemyMoveCommand& enemy, EnemyPopData enemyData);

	//半透明に
	Vector4 color_ = { 1,1,1,0.9f };
};