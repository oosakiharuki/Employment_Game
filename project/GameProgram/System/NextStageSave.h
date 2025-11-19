#pragma once
#include "MyMath.h"
#include <memory>

/// <summary>
/// 移行する前に保存するデータ
/// </summary>
struct SceneSaveData {
	uint32_t playerMaxHp;//プレイヤーの体力
	uint32_t playerHp;//プレイヤーの体力
	std::string nextStageFile;//次のステージ名
};

class NextStageSave
{
public:

	static std::shared_ptr<NextStageSave> GetInstance();

	SceneSaveData GetNextStageSaveData() { return sceneSaveData_; }

	void SetNextStageFile(const std::string nextStageFile) { sceneSaveData_.nextStageFile = nextStageFile; }

	void SetPlayerHp(const uint32_t playerHp) { sceneSaveData_.playerHp = playerHp; }

private:

	SceneSaveData sceneSaveData_;

	static std::shared_ptr<NextStageSave> sInstance;
};

