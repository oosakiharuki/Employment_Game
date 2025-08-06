#pragma once
#include "MyMath.h"
#include "Model_obj.h"

//ComPtr
#include <wrl.h>
#include "d3d12.h"

#include "Camera.h"
#include "WorldTransform.h"

class Object3dCommon;

class Object3d
{
public:
	void Initialize();
	void Update();
	void Draw(const WorldTransform& worldTransform);
	void Draw(const WorldTransform& worldTransform, const std::string& textureData);


	//static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void SetModel(Model_obj* model) { this->model = model; }
	void SetModelFile(const std::string& filePath);
	void LightSwitch(bool isLight);

	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }

private:
	Object3dCommon* object3dCommon = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	TransformationMatrix* wvpData = nullptr;

	//ライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource;
	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightSphereData = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	CameraForGPU* cameraData = nullptr;

	//ポイントライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	//マテリアルにデータを書き込む
	PointLight* pointLightData = nullptr;

	//スポットライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;
	//マテリアルにデータを書き込む
	SpotLight* spotLightData = nullptr;


	Transform transform;

	Transform transformL;

	Model_obj* model = nullptr;
	Camera* camera = nullptr;
};