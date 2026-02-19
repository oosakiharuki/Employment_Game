#include "IStageObject.h"

using namespace MyMath;

IStageObject::IStageObject(){}

IStageObject::~IStageObject() {}

void IStageObject::SetObjectName(const std::string& name) {
	objectName_ = name;
}

void IStageObject::SetNextStage(const std::string& fileName){}

std::string IStageObject::GetNextStage(){
	std::string null = "";
	return null;
}
