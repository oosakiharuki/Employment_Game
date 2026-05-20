#include "AnimationMath.h"
#include "Lerp.h"
#include <cassert>

namespace MyMath {


	Vector3 CalculateValue(const AnimationCurve<Vector3>& keyframes, float time) {
		assert(!keyframes.keyframes.empty());
		if (keyframes.keyframes.size() == 1 || time <= keyframes.keyframes[0].time) {
			return keyframes.keyframes[0].value; //アニメーションしない、止まっている時
		}
		for (size_t index = 0; index < keyframes.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (keyframes.keyframes[index].time <= time && time <= keyframes.keyframes[nextIndex].time) {
				float t = (time - keyframes.keyframes[index].time) / (keyframes.keyframes[nextIndex].time - keyframes.keyframes[index].time);
				return Lerp(keyframes.keyframes[index].value, keyframes.keyframes[nextIndex].value, t);
			}
		}
		return (*keyframes.keyframes.rbegin()).value;
	}

	Quaternion CalculateValueQuaternion(const AnimationCurve<Quaternion>& keyframes, float time) {
		assert(!keyframes.keyframes.empty());
		if (keyframes.keyframes.size() == 1 || time <= keyframes.keyframes[0].time) {
			return { keyframes.keyframes[0].value.x,keyframes.keyframes[0].value.y ,keyframes.keyframes[0].value.z,keyframes.keyframes[0].value.w };
		}
		for (size_t index = 0; index < keyframes.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (keyframes.keyframes[index].time <= time && time <= keyframes.keyframes[nextIndex].time) {
				float t = (time - keyframes.keyframes[index].time) / (keyframes.keyframes[nextIndex].time - keyframes.keyframes[index].time);
				return SLerp(keyframes.keyframes[index].value, keyframes.keyframes[nextIndex].value, t);
			}
		}

		return (*keyframes.keyframes.rbegin()).value;
	}


	Vector3 InterpolationValue(const AnimationCurve<Vector3>& key1, const AnimationCurve<Vector3>& key2, float time) {
		assert(!key1.keyframes.empty() || !key2.keyframes.empty());
		if (key1.keyframes.size() == 1 || time <= key1.keyframes[0].time ||
			key2.keyframes.size() == 1 || time <= key2.keyframes[0].time) {
			return { key2.keyframes[0].value };
		}

		Vector3 animation1, animation2;

		for (size_t index = 0; index < key1.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (key1.keyframes[index].time <= time && time <= key1.keyframes[nextIndex].time) {
				float t = (time - key1.keyframes[index].time) / (key1.keyframes[nextIndex].time - key1.keyframes[index].time);
				animation1 = Lerp(key1.keyframes[index].value, key1.keyframes[nextIndex].value, t);
			}
		}


		for (size_t index = 0; index < key2.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (key2.keyframes[index].time <= time && time <= key2.keyframes[nextIndex].time) {
				float t = (time - key2.keyframes[index].time) / (key2.keyframes[nextIndex].time - key2.keyframes[index].time);
				animation2 = Lerp(key2.keyframes[index].value, key2.keyframes[nextIndex].value, t);
			}
		}

		return Lerp(animation1, animation2, time);
	}

	Quaternion InterpolationValueQuaternion(const AnimationCurve<Quaternion>& key1, const AnimationCurve<Quaternion>& key2, float time) {
		assert(!key1.keyframes.empty() || !key2.keyframes.empty());
		if (key1.keyframes.size() == 1 || time <= key1.keyframes[0].time ||
			key2.keyframes.size() == 1 || time <= key2.keyframes[0].time) {
			return { key1.keyframes[0].value };
		}

		Quaternion animation1, animation2;

		for (size_t index = 0; index < key1.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			if (key1.keyframes[index].time <= time && time <= key1.keyframes[nextIndex].time) {
				float t = (time - key1.keyframes[index].time) / (key1.keyframes[nextIndex].time - key1.keyframes[index].time);
				animation1 = SLerp(key1.keyframes[index].value, key1.keyframes[nextIndex].value, t);
			}
		}


		for (size_t index = 0; index < key2.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			if (key2.keyframes[index].time <= time && time <= key2.keyframes[nextIndex].time) {
				float t = (time - key2.keyframes[index].time) / (key2.keyframes[nextIndex].time - key2.keyframes[index].time);
				animation2 = SLerp(key2.keyframes[index].value, key2.keyframes[nextIndex].value, t);
			}
		}

		return SLerp(animation1, animation2, time);
	}
}