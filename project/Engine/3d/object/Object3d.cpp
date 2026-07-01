/// ------------------------------
///
/// .objファイル型のオブジェクト
/// 
/// ------------------------------
#include "Object3d.h"
#include "Object3dCommon.h"
#include "externals/imgui/imgui.h"
#include "TextureManager.h"

#include <fstream>
#include <sstream>
#include "ModelManager.h"
#include <numbers>

using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Object3d::Initialize() {
		this->camera_ = Object3dCommon::GetInstance().GetDefaultCamera();
		wvpResource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(TransformationMatrix));
		wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));


		wvpData_->World = MakeIdentity4x4();
		wvpData_->WVP = MakeIdentity4x4();

		//カメラ初期化
		InitCamera();

		//ライト初期化処理まとめ
		InitLight();
	}


	void Object3d::Update(WorldTransform& worldTransform) {

		wvpData_->World = worldTransform.GetMatWorld();
		worldMatrix_ = worldTransform.GetMatWorld();

		directionalLightData_->direction = Normalize(directionalLightData_->direction);
	}

	void Object3d::Update() {

		wvpData_->World = MakeIdentity4x4();
		worldMatrix_ = MakeIdentity4x4();

		directionalLightData_->direction = Normalize(directionalLightData_->direction);
	}

	void Object3d::Draw() {
		CameraUpdate();
		//モデル
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		DrawCommand();
		if (model_) {
			model_->Draw();
		}
	}

	void Object3d::Draw(const std::string& textureData) {
		CameraUpdate();
		//モデル
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		DrawCommand();
		if (model_) {
			model_->Draw(textureData);
		}
	}

	void Object3d::CameraUpdate() {
		if (camera_) {
			Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
			wvpData_->WVP = worldMatrix_ * projectionMatrix;
		}
		else {
			wvpData_->WVP = worldMatrix_;
		}
	}

	void Object3d::SetModelFile(const std::string& filePath) {
		model_ = ModelManager::GetInstance().FindModel_obj(filePath);
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
}