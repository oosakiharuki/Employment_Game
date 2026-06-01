#include "TimeScale.h"

std::unique_ptr<TimeScale> TimeScale::sInstance_ = nullptr;

TimeScale& TimeScale::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<TimeScale>();
	}
	return *sInstance_;
}


void TimeScale::Update() {

	//時間がデフォルトでないとき
	if (timeScale_ != kDefaultValue_) {
		//時間を加算
		returnTimer_ += kDefaultValue_;
	}

	//元に戻す値まで来たら
	if (returnTimer_ >= TimeMax_) {
		ResetTimeScale();//デフォルトに戻す
		returnTimer_ = 0.0f;
	}


}

void TimeScale::Finalize() {
	sInstance_.reset();
}
