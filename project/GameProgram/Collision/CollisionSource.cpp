#include "CollisionSource.h"


Vector3 CollisionSource::DistanceCollisionCenter(const Vector3& otherCenter) {
	Vector3 result{};
	result.x = std::abs(otherCenter.x) - std::abs(center_.x);
	result.y = std::abs(otherCenter.y) - std::abs(center_.y);
	result.z = std::abs(otherCenter.z) - std::abs(center_.z);
	return result;
}

void CollisionSource::OnCollision(CollisionSource* collision) {}

bool CollisionSource::TypeCheckUp(const CollisionTypes& collisionType) { return false; }