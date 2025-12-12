#include "Reaction.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void Reaction::ScaleReaction(Vector3& value, bool& mosionOn, const Vector3& power, float& timer, float maxTime) {
	//時間が半分になったら
	ReturnHalfTime(value, power, timer, maxTime);

	//経過時間がたったら終了
	if (timer >= maxTime) {
		//時間を初期値(0)にする
		timer = 0.0f;
		//元の大きさに{1,1,1}
		value = kDefaultScale_;
		//モーションを終了する
		mosionOn = false;
	}

	//時間がが進む
	timer += kDeltaTime_;
}

void Reaction::FoundReaction(Vector3& value, bool& mosion, const Vector3& power, float& timer, float maxTime, const Vector3& prePosition) {
	//時間が半分になったら
	ReturnHalfTime(value, power, timer, maxTime);

	//経過時間がたったら終了
	if (timer >= maxTime) {
		//時間を初期値(0)にする
		timer = 0.0f;
		//元の位置に
		value = prePosition;
		//モーションを終了する
		mosion = false;
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