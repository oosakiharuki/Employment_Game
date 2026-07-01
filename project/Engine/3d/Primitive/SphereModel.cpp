/// --------------------
///
/// 球型のオブジェクト
/// 
/// --------------------
#include "SphereModel.h"
#include <TextureManager.h>

using namespace MyMath;
using namespace Primitive;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void SphereModel::Initialize() {
		modelData_ = CreateSphere();
		modelData_.materialData.textureFilePath = "resource/Sprite/white.dds";
		//テクスチャ読み込み
		TextureManager::GetInstance().LoadTexture(modelData_.materialData.textureFilePath);
		modelData_.materialData.textureIndex = TextureManager::GetInstance().GetSrvIndex(modelData_.materialData.textureFilePath);


		vertexResource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

		vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
		vertexBufferView_.StrideInBytes = sizeof(VertexData);

		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
		std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

		//Model用マテリアル
		//マテリアル用のリソース
		materialResource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(Material));
		//書き込むためのアドレス
		materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		//色の設定
		materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		materialData_->enableLighting = false;
		materialData_->uvTransform = MakeIdentity4x4();
		materialData_->shininess = 70;

		//テクスチャ読み込み
		TextureManager::GetInstance().LoadTexture(modelData_.materialData.textureFilePath);
		modelData_.materialData.textureIndex = TextureManager::GetInstance().GetSrvIndex(modelData_.materialData.textureFilePath);

		this->camera_ = debugWireframes_->GetDefaultCamera();
		wvpResource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(TransformationMatrix));
		wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

		wvpData_->World = MakeIdentity4x4();
		wvpData_->WVP = MakeIdentity4x4();

		//Phong Reflection Model
		cameraResource_ = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(CameraForGPU));
		cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

		cameraData_->worldPosition = { 0,0,0 };

	}

	void SphereModel::Update(const Matrix4x4& matworld) {

		Matrix4x4 WorldViewProjectionMatrix;
		if (camera_) {
			Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
			WorldViewProjectionMatrix = matworld * projectionMatrix;
		}
		else {
			WorldViewProjectionMatrix = matworld;
		}

		wvpData_->World = matworld;
		//wvpData->World = worldMatrix;
		wvpData_->WVP = WorldViewProjectionMatrix;

		materialData_->color = color_;

	}

	void SphereModel::Draw() {
		DirectXCommon::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance().GetSrvHandleGPU(modelData_.materialData.textureFilePath));
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
	}
}