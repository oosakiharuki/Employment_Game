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
	this->object3dCommon_ = Object3dCommon::GetInstance().get();
	this->camera_ = object3dCommon_->GetDefaultCamera();
	wvpResource_ = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	
	wvpData_->World = MakeIdentity4x4();
	wvpData_->WVP= MakeIdentity4x4();
	
	//カメラ初期化
	InitCamera();

	//ライト初期化処理まとめ
	InitLight();
}


void Object3d::Update(const WorldTransform& worldTransform) {

	wvpData_->World = worldTransform.matWorld_;
	worldMatrix_ = worldTransform.matWorld_;

	directionalLightData_->direction = Normalize(directionalLightData_->direction);
}

void Object3d::Update() {

	wvpData_->World = MakeIdentity4x4();
	worldMatrix_ = MakeIdentity4x4();

	directionalLightData_->direction = Normalize(directionalLightData_->direction);
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
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
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
	object3dCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
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

void Object3d::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

void Object3d::LightSwitch(bool isLight) {
	if (model_) {
		model_->LightOn(isLight);
	}
}