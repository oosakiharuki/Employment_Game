#include "NextStageSave.h"

std::shared_ptr<NextStageSave> NextStageSave::sInstance = nullptr;

std::shared_ptr<NextStageSave> NextStageSave::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<NextStageSave>();
	}
	return sInstance;
}

void NextStageSave::SetPlayerParameta(const SceneSaveData& saveData) {
	sceneSaveData_.playerHp = saveData.playerHp;
	sceneSaveData_.playerMaxHp = saveData.playerMaxHp;
	sceneSaveData_.playerZanki = saveData.playerZanki;
}