/// ----------------------------------
///
/// 重力があるゲーム
/// 
/// ----------------------------------
#include "GravityActor.h"

#include "UseEveryOne.h"
#include "TimeScale.h"

using namespace UseEveryOne;

void GravityActor::IsGround(bool result) {
	isGround_ = result;
	//地面なら重力を0にする(沈まないようにする)
	if (isGround_) {
		gravity_ = 0;
	}
}

void GravityActor::GravityUpdate(float& translateY, bool isSlowTime) {

	//スローをかける
	if (isSlowTime) {
		//重力
		gravity_ -= kGravityPower_ * TimeScale::GetInstance().GetTimeScaleFacto();
	}
	else{
		//重力
		gravity_ -= kGravityPower_;
	}
	gravity_ = std::clamp(gravity_,kGravityMax_,0.0f);//上限設定

	//地面についていない
	if (!isGround_) {
		translateY += gravity_;
	}
	else {
		//重力パワーリセット
		gravity_ = 0.0f;
	}
}