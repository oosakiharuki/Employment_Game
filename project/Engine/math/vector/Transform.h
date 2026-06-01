#pragma once
#include"Vector3.h"

/// <summary>
/// 構造体_Transform(スケール、回転、座標)
/// </summary>
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
namespace MyMath {
	//inline = 多重定義を防ぐ

	inline bool operator==(const Transform& a, const Transform& b) {
		if (a.translate == b.translate && a.rotate == b.rotate && a.scale == b.scale) {
			return true;
		}
		return false;
	}

	inline bool operator!=(const Transform& a, const Transform& b) {
		if (a == b) {
			return false;
		}
		return true;
	}
}