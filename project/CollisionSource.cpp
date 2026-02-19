#include "CollisionSource.h"


Vector3 CollisionSource::GetDistance(const Vector3& otherCenter) {
	Vector3 result{};
	result.x = std::abs(center_.x - otherCenter.x);
	result.y = std::abs(center_.y - otherCenter.y);
	result.z = std::abs(center_.z - otherCenter.z);
	return result;
}

void CollisionSource::OnCollision(CollisionSource* collision) {}
