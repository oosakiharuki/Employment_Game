#include "CollisionMath.h"

namespace MyMath {

	bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2) {
		//AABBのMaxとMinが同じ状態(点)ならfalse
		if ((aabb1.max.x == aabb1.min.x && aabb1.max.y == aabb1.min.y && aabb1.max.z == aabb1.min.z) ||
			(aabb2.max.x == aabb2.min.x && aabb2.max.y == aabb2.min.y && aabb2.max.z == aabb2.min.z)) {
			return false;
		}

		if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && //x軸
			(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && //y軸
			(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) { //z軸
			return true;
		}
		return false;
	}

	bool IsCollisionAABB_outSide(const AABB& aabb1, const AABB& aabb2) {
		if ((aabb1.min.x >= aabb2.min.x && aabb1.max.x <= aabb2.max.x) && //x軸
			(aabb1.min.y >= aabb2.min.y && aabb1.max.y <= aabb2.max.y) && //y軸
			(aabb1.min.z >= aabb2.min.z && aabb1.max.z <= aabb2.max.z)) { //z軸
			return true;
		}
		return false;
	}


	Vector3 OverAABB(const AABB& aabb1, const AABB& aabb2) {
		Vector3 result;
		float overX1 = aabb2.max.x - aabb1.min.x;
		float overX2 = aabb1.max.x - aabb2.min.x;

		//重なっているのは値が小さいほう
		if (overX1 < overX2) {
			result.x = overX1;
		}
		else {
			result.x = overX2;
		}

		float overY1 = aabb2.max.y - aabb1.min.y;
		float overY2 = aabb1.max.y - aabb2.min.y;

		if (overY1 < overY2) {
			result.y = overY1;
		}
		else {
			result.y = overY2;
		}

		float overZ1 = aabb2.max.z - aabb1.min.z;
		float overZ2 = aabb1.max.z - aabb2.min.z;

		if (overZ1 < overZ2) {
			result.z = overZ1;
		}
		else {
			result.z = overZ2;
		}

		return result;
	}


	bool IsCollisionAABB_Segment(const AABB& aabb, const Segment& segment) {

		//二次元の四角の点を作る
		Vector2 topLeft = { aabb.min.x, aabb.max.y };
		Vector2 topRight = { aabb.max.x, aabb.max.y };
		Vector2 bottomLeft = { aabb.min.x, aabb.min.y };
		Vector2 bottomRight = { aabb.max.x, aabb.min.y };

		Vector2 p1 = { segment.origin.x,segment.origin.y };
		Vector2 p2 = { segment.diff.x,segment.diff.y };


		// AABBの4辺との交差判定
		// 線ごとの当たり判定
		if (Intersect(p1, p2, topLeft, topRight)) return true;
		if (Intersect(p1, p2, topRight, bottomRight)) return true;
		if (Intersect(p1, p2, bottomRight, bottomLeft)) return true;
		if (Intersect(p1, p2, bottomLeft, topLeft)) return true;


		return false;
	}

	bool Intersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2) {

		float d = (a2.x - a1.x) * (b2.y - b1.y) - (a2.y - a1.y) * (b2.x - b1.x);//クロス積
		if (d == 0) {
			return false; // 平行
		}

		float u = ((b1.x - a1.x) * (b2.y - b1.y) - (b1.y - a1.y) * (b2.x - b1.x)) / d;
		float v = ((b1.x - a1.x) * (a2.y - a1.y) - (b1.y - a1.y) * (a2.x - a1.x)) / d;

		return (u >= 0 && u <= 1) && (v >= 0 && v <= 1);
	}
}
