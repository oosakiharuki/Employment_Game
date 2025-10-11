#include "IStageObject.h"

using namespace MyMath;

IStageObject::IStageObject(){}

IStageObject::~IStageObject() {}

AABB IStageObject::GetAABB() {
	AABB aabb;

	aabb.min = wt.translation_ + aabb_.min;
	aabb.max = wt.translation_ + aabb_.max;

	return aabb;
}