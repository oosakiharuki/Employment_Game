#include "NextStageSave.h"

std::unique_ptr<NextStageSave> NextStageSave::sInstance_ = nullptr;

NextStageSave& NextStageSave::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<NextStageSave>();
	}
	return *sInstance_;
}

void NextStageSave::Finalize() {
	sInstance_.reset();
}

void NextStageSave::SetPlayerHp(uint32_t playerHp) {
	sceneSaveData_.playerHp = playerHp;
}

void NextStageSave::SetPlayerRemain(uint32_t remain) {
	sceneSaveData_.playerRemain = remain;
}

void NextStageSave::SetPlayerMaxHp(uint32_t maxHp) {
	sceneSaveData_.playerMaxHp = maxHp;
}

void NextStageSave::SetCheckPoint(const Vector3& checkPoint) {
	sceneSaveData_.checkPoint = checkPoint;
}
