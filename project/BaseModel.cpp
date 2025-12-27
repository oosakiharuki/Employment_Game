#include "BaseModel.h"


void BaseModel::SetColor(const Vector4& color) {

	for (auto& materialResource :  materialResources_) {
		//書き込むためのアドレス
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		materialData_->color = color;
	}
}

void BaseModel::LightOn(bool Light) {
	for (auto& material : materialResources_) {
		//書き込むためのアドレス
		material->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		//全てのライトを on/off
		materialData_->enableLighting = Light;
	}
}