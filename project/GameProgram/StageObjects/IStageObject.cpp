#include "IStageObject.h"

using namespace MyMath;

IStageObject::IStageObject(){}

IStageObject::~IStageObject() {
	delete object_;
}

AABB IStageObject::GetAABB() {
	AABB aabb;

	aabb.min = wt.translation_ + aabb_.min;
	aabb.max = wt.translation_ + aabb_.max;

	return aabb;
}