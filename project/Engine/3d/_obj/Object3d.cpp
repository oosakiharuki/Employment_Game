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
	this->object3dCommon = Object3dCommon::GetInstance();	
	this->camera = object3dCommon->GetDefaultCamera();
	wvpResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	
	wvpData->World = MakeIdentity4x4();
	wvpData->WVP= MakeIdentity4x4();

	//ライト用のリソース
	directionalLightSphereResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightSphereResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData));
	//色の設定
	directionalLightSphereData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData->intensity = 0.0f;


	//Phong Reflection Model
	cameraResource =object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	cameraData->worldPosition = { 0,0,0 };

	//ライト用のリソース
	pointLightResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(PointLight));
	//書き込むためのアドレス
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	//設定
	pointLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData->position = { 0.0f,2.0f,0.0f };
	pointLightData->intensity = 1.0f;
	pointLightData->radius = 5.0f;
	pointLightData->decay = 1.0f;


	//ライト用のリソース
	spotLightResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(SpotLight));
	//書き込むためのアドレス
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	//設定
	spotLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData->position = { 2.0f,1.25f,0.0f };
	spotLightData->distance = 70.0f;
	spotLightData->direction = Normalize({ -1.0f,-1.0f,0.0f });
	spotLightData->intensity = 0.0f;
	spotLightData->decay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);






	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	transformL = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };




}

void Object3d::Update() {

	//モデル
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 WorldViewProjectionMatrix;
	if (camera) {
		Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = Multiply(worldMatrix, projectionMatrix);
	}
	else {
		WorldViewProjectionMatrix = worldMatrix;
	}
	wvpData->World = worldMatrix;
	wvpData->WVP = WorldViewProjectionMatrix;

	directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);


}


void Object3d::Draw(const WorldTransform& worldTransform) {
	Matrix4x4 WorldViewProjectionMatrix;
	if (camera) {
		Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = Multiply(worldTransform.matWorld_, projectionMatrix);
	}
	else {
		WorldViewProjectionMatrix = worldTransform.matWorld_;
	}

	wvpData->World = worldTransform.matWorld_;
	//wvpData->World = worldMatrix;
	wvpData->WVP = WorldViewProjectionMatrix;

	directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);


	//モデル
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());
	if (model) {
		model->Draw();
	}
}

void Object3d::Draw(const WorldTransform& worldTransform, const std::string& textureData) {

	Matrix4x4 WorldViewProjectionMatrix;
	if (camera) {
		Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = Multiply(worldTransform.matWorld_, projectionMatrix);
	}
	else {
		WorldViewProjectionMatrix = worldTransform.matWorld_;
	}

	wvpData->World = worldTransform.matWorld_;
	//wvpData->World = worldMatrix;
	wvpData->WVP = WorldViewProjectionMatrix;

	directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);


	//モデル
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());
	if (model) {
		model->Draw(textureData);
	}
}

void Object3d::SetModelFile(const std::string& filePath) {
	model = ModelManager::GetInstance()->FindModel_obj(filePath);
}

void Object3d::LightSwitch(bool isLight) {
	if (model) {
		model->LightOn(isLight);
	}
}