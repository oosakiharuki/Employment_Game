#pragma once
#include "MyMath.h"
#include "Model_glTF.h"

//ComPtr
#include <wrl.h>
#include "d3d12.h"

#include "Camera.h"
#include "WorldTransform.h"

#include "SphereModel.h"

class GLTFCommon;

class Object_glTF
{
public:
	Object_glTF();
	~Object_glTF();

	void Initialize();
	void Update(const WorldTransform& worldTransform);
	void Draw();
	void Draw(const std::string& textureData);


	//static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void SetModel(Model_glTF* model) { this->model = model; }
	void SetModelFile(const std::string& filePath);
	void LightSwitch(bool isLight);
	//環境マップ用
	void SetEnvironment(const std::string& filePath);

	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }

	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
	void Interpolation(Skeleton& skeleton, const Animation& animation, const Animation& nextAnimation, float animationTime);
	void SkeletonUpdate(Skeleton& skeleton);
	void SkeletonUpdate(Skeleton& skeleton,const Matrix4x4& matWorld);
	void SkinClusterUpdate(SkinCluster& skinCluster ,const Skeleton& skeleton);

	void ChangeAnimation(const std::string& filePath);
	Material* GetMaterial() { return material; };

private:
	GLTFCommon* object3dCommon = nullptr;

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

	Model_glTF* model = nullptr;
	Camera* camera = nullptr;

	ModelData_glTF modelData;


	Animation animation;
	///アニメーションタイマー
	float animationTime = 0.0f;
	/// 補間タイマー
	float changeTime = 0.0f;
	
	Skeleton skeleton;
	SkinCluster skinCluster;

	std::vector<SphereModel*> debugSphere;
	void SetWireframe();

	bool isChange = false;

	//変更前のアニメーション
	Animation preAnimation;

	Material* material;
};