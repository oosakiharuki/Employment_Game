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
	uint32_t RemainingLife = 2;

	bool isGameOver = false;
	bool isGameClear = false;

};