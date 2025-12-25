#include "NextStageSave.h"

std::shared_ptr<NextStageSave> NextStageSave::sInstance = nullptr;

std::shared_ptr<NextStageSave> NextStageSave::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<NextStageSave>();
	}
	return sInstance;
}

void NextStageSave::SetPlayerHp(uint32_t playerHp) {
	sceneSaveData_.playerHp = playerHp;
}

void NextStageSave::SetPlayerZanki(uint32_t zanki) {
	sceneSaveData_.playerZanki = zanki;
}

void NextStageSave::SetPlayerMaxHp(uint32_t maxHp) {
	sceneSaveData_.playerMaxHp = maxHp;
}
