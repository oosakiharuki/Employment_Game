#include "TimeScale.h"

std::unique_ptr<TimeScale> TimeScale::sInstance_ = nullptr;

TimeScale& TimeScale::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<TimeScale>();
	}
	return *sInstance_;
}


void TimeScale::Update() {

	if (timeScale_ != kDefaultValue_) {
		slowTime_ += kDefaultValue_;
	}

	if (slowTime_ >= kSlowTimeMax_) {
		ResetTimeScale();
		slowTime_ = 0.0f;
	}


}

void TimeScale::Finalize() {
	sInstance_.reset();
}
