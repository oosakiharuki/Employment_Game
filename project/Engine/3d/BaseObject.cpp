#include "BaseObject.h"
#include "DirectXCommon.h"

using namespace MyMath;

void BaseObject::DrawCommand() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
}

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
	directionalLightData_->color = kDefaultColor_;
	directionalLightData_->direction = kDirectionalLightDirection_;
	directionalLightData_->intensity = kIntensity_;
}

void BaseObject::CreatePointLight() {
	//ライト用のリソース
	pointLightResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(PointLight));
	//書き込むためのアドレス
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	//_
	pointLightData_->color = kDefaultColor_;
	pointLightData_->position = kPointLightPosition_;
	pointLightData_->intensity = kIntensity_;
	pointLightData_->radius =kPointLightRadius_;
	pointLightData_->decay = kPointLightDecay_;
}

void BaseObject::CreateSpotLight() {
	//ライト用のリソース
	spotLightResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(SpotLight));
	//書き込むためのアドレス
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	//設定
	spotLightData_->color = kDefaultColor_;
	spotLightData_->position = kSpotLightPosition_;
	spotLightData_->distance = kSpotLightDistance_;
	spotLightData_->direction = Normalize(kSpotLightDirection_);
	spotLightData_->intensity = 0.0f;
	spotLightData_->decay = kPointLightDecay_;
	spotLightData_->cosAngle = kCosAngle;
	spotLightData_->cosFalloffStart = kCosFalloffStart;
}