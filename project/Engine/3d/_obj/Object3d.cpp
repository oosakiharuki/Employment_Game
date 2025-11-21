#include "Object3d.h"
#include "Object3dCommon.h"
#include "externals/imgui/imgui.h"
#include "TextureManager.h"

#include <fstream>
#include <sstream>
#include "ModelManager.h"
#include <numbers>

using namespace MyMath;

void Object3d::Initialize() {
	this->object3dCommon_ = Object3dCommon::GetInstance();
	this->camera_ = object3dCommon_->GetDefaultCamera();
	wvpResource_ = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	
	wvpData_->World = MakeIdentity4x4();
	wvpData_->WVP= MakeIdentity4x4();

	//ライト用のリソース
	directionalLightSphereResource_ = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightSphereResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData_));
	//色の設定
	directionalLightSphereData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData_->intensity = 0.0f;


	//Phong Reflection Model
	cameraResource_ =object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = { 0,0,0 };

	//ライト用のリソース
	pointLightResource_ = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(PointLight));
	//書き込むためのアドレス
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	//_
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,2.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 5.0f;
	pointLightData_->decay = 1.0f;


	//ライト用のリソース
	spotLightResource_ = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(SpotLight));
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

void Object3d::Update(const WorldTransform& worldTransform) {

	wvpData_->World = worldTransform.matWorld_;
	worldMatrix_ = worldTransform.matWorld_;

	directionalLightSphereData_->direction = Normalize(directionalLightSphereData_->direction);
}

void Object3d::Update() {

	wvpData_->World = MakeIdentity4x4();
	worldMatrix_ = MakeIdentity4x4();

	directionalLightSphereData_->direction = Normalize(directionalLightSphereData_->direction);
}

void Object3d::Draw() {
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_->WVP = worldMatrix_ * projectionMatrix;
	}
	else {
		wvpData_->WVP = worldMatrix_;
	}

	//モデル
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	if (model_) {
		model_->Draw();
	}
}

void Object3d::Draw(const std::string& textureData) {
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_->WVP = worldMatrix_ * projectionMatrix;
	}
	else {
		wvpData_->WVP = worldMatrix_;
	}

	//モデル
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	if (model_) {
		model_->Draw(textureData);
	}
}

void Object3d::SetModelFile(const std::string& filePath) {
	model_ = ModelManager::GetInstance()->FindModel_obj(filePath);
}

void Object3d::SetColor(Vector4 color) {
	if (model_) {
		model_->SetColor(color);
	}
}

void Object3d::LightSwitch(bool isLight) {
	if (model_) {
		model_->LightOn(isLight);
	}
}