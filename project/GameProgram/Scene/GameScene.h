#pragma once
#include "IScene.h"

class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void SetStage(const std::string stage) { stage_file = stage; }

private:
	std::string stage_file;
	void StageMovement(const std::string leveleditor_file, const std::string stageObj);
	
	//
	uint32_t playerHp;

	SoundData soundData_;

	//残機
	uint32_t RemainingLife = 3;

	bool isGameOver = false;
	bool isGameClear = false;
	

	//次のステージの移動
	bool isNextStage = false;
	std::string nextStage_fileName;
	bool isChangeStage = true;

	//カメラズーム
	Segment cameraSegment{};
	float zumuTimer = 0.0f;
	//bool isZumuIn = false;

	void LoadEventCSV(std::string file);

	std::stringstream enemyPopCsvFile;

	void PopEventEneies();
	bool eventWave = false;
	
	//敵を生んだ・倒した数
	uint32_t enemyBornCount = 0;
	uint32_t enemyDeadCount = 0;

	bool isLoadCsv = true;

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

	//std::vector<std::unique_ptr<IEnemy>> eventEnmeies;
};