#include "BaseObject.h"
#include "DirectXCommon.h"

using namespace MyMath;

void BaseObject::InitCamera() {
	cameraResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = { 0,0,0 };
}


void BaseObject::InitLight() {
	//ディレクショナルライト(平方向)
	CreateDirectionalLight();
	//ポイントライト
	CreatePointLight();
	//スポットライト
	CreateSpotLight();
}

void BaseObject::CreateDirectionalLight() {
	//ライト用のリソース
	directionalLightResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//色の設定
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 0.0f;
}

void BaseObject::CreatePointLight() {
	//ライト用のリソース
	pointLightResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(PointLight));
	//書き込むためのアドレス
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	//_
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,2.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 5.0f;
	pointLightData_->decay = 1.0f;
}

void BaseObject::CreateSpotLight() {
	//ライト用のリソース
	spotLightResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(SpotLight));
	//書き込むためのアドレス
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	//設定
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->distance = 70.0f;
	spotLightData_->direction = Normalize({ -1.0f,-1.0f,0.0f });
	spotLightData_->intensity = 0.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
}