#include "Reaction.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void Reaction::ScaleReaction(Vector3& value, bool& motionOn, const Vector3& power, float& timer, float maxTime) {
	if (!motionOn) return;
	//時間が半分になったら
	ReturnHalfTime(value, power, timer, maxTime);

	//経過時間がたったら終了
	if (timer >= maxTime) {
		Finish(value, kDefaultScale_, timer, motionOn);
	}

	//時間がが進む
	timer += kDeltaTime_;
}

void Reaction::FoundReaction(Vector3& value, bool& motion, const Vector3& power, float& timer, float maxTime, const Vector3& prePosition) {
	if (!motion) return;

	//時間が半分になったら
	ReturnHalfTime(value, power, timer, maxTime);

	//経過時間がたったら終了
	if (timer >= maxTime) {
		Finish(value, prePosition, timer, motion);
	}

	timer += kDeltaTime_;
}

void Reaction::ReturnHalfTime(Vector3& value, const Vector3& power, float& timer, float maxTime) {
	//時間が半分になったら
	if (timer >= maxTime * kDivideByTwo_) {
		//値を引く
		value -= power;
	}
	else {
		//値を足す
		value += power;
	}
}

void Reaction::Finish(Vector3& value, const Vector3& prePosition, float& timer, bool& flag) {
	//時間を初期値(0)にする
	timer = 0.0f;
	//元の値に
	value = prePosition;
	//モーションを終了する
	flag = false;
}
