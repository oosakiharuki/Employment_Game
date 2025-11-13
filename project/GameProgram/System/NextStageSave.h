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

	SceneSaveData GetNextStageSaveData() { return sceneSaveData; }

	void SetNextStageFile(std::string instance) { sceneSaveData.nextStageFile = instance; }

	void SetPlayerHp(uint32_t instance) { sceneSaveData.playerHp = instance; }

private:

	SceneSaveData sceneSaveData;

	static std::shared_ptr<NextStageSave> sInstance;
};

