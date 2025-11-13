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

void IStageObject::SetObjectName(const std::string& name) {
	objectName = name;
}

void IStageObject::SetNextStage(std::string fileName){}

std::string IStageObject::GetNextStage(){
	std::string null = "";
	return null;
}
