#include "NextStageSave.h"

std::shared_ptr<NextStageSave> NextStageSave::sInstance_ = nullptr;

std::shared_ptr<NextStageSave> NextStageSave::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<NextStageSave>();
	}
	return sInstance_;
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
