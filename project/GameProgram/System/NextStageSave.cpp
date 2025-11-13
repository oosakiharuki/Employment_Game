#include "NextStageSave.h"

std::shared_ptr<NextStageSave> NextStageSave::sInstance = nullptr;

std::shared_ptr<NextStageSave> NextStageSave::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<NextStageSave>();
	}
	return sInstance;
}
