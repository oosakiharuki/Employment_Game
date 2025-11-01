#include "NextStageSave.h"

std::shared_ptr<NextStageSave> NextStageSave::instance = nullptr;

std::shared_ptr<NextStageSave> NextStageSave::GetInstance() {
	if (instance == nullptr) {
		instance = std::make_unique<NextStageSave>();
	}
	return instance;
}
