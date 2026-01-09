#include "IStageObject.h"

using namespace MyMath;

IStageObject::IStageObject(){}

IStageObject::~IStageObject() {}

AABB IStageObject::GetAABB() {
	AABB aabb;

	aabb.min = transform_.translate + aabb_.min;
	aabb.max = transform_.translate + aabb_.max;

	return aabb;
}

void IStageObject::SetObjectName(const std::string& name) {
	objectName_ = name;
}

void IStageObject::SetNextStage(const std::string& fileName){}

std::string IStageObject::GetNextStage(){
	std::string null = "";
	return null;
}
