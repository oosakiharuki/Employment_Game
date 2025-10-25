#pragma once
#include "IScene.h"

/// <summary>
/// ゲームシーン(ISceneの派生クラス)
/// </summary>
class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	/// <summary>
	/// 次のステージに移る
	/// </summary>
	/// <param name="leveleditor_file"></param>次のステージの名前
	/// <param name="stageObj"></param>次のステージのオブジェクト
	void StageMovement(const std::string leveleditor_file, const std::string stageObj);
	
	//シーンチェンジ時に初期化されない用のプレイヤーHp保存場所
	uint32_t playerHp;

	SoundData soundData_;

	//残機
	uint32_t RemainingLife = 3;

	//次のステージの移動
	bool isNextStage = false;
	std::string nextStage_fileName;

	//スタート演出(水たまりから飛び出る感じに)
	bool isStartStage = true;
	Vector3 playerPoint{};
	float startPointY = 0.0f;

	//カメラズーム
	Segment cameraSegment{};
	float zumuTimer = 0.0f;

	void LoadEventCSV(std::string file);

	std::stringstream enemyPopCsvFile;

	void PopEventEneies(EventTrigger* eventTrigger);
	bool eventWave = false;
	
	//敵を生んだ・倒した数
	uint32_t enemyBornCount = 0;
	uint32_t enemyDeadCount = 0;

	bool isLoadCsv = true;

	//スタート時のワープゲート
	std::unique_ptr<WarpGate> startWarp;

	/// <summary>
	/// 水たまりのようなワープゲート出口
	/// </summary>
	void WarterWarpExit();

	/// <summary>
	/// 敵の召喚処理
	/// </summary>
	/// <param name="position"></param>
	/// 召喚位置
	/// <param name="position"></param>
	/// 向き
	/// <param name="name"></param>
	/// 敵の名前
	void EnemyPop(const Vector3& position,const Vector3& rotation,const std::string& name);

	/// <summary>
	/// イベントリセット(共有)
	/// </summary>
	void ResetEvent();

	void Respawn();

	/// <summary>
	/// チェックポイントを変更する
	/// </summary>
	void ChangeCheckPoint();

	/// <summary>
	/// カメラコントロール
	/// </summary>
	void CameraControl();

};