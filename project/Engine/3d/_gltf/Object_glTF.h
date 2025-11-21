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
/// <summary>
/// .gltf版のオブジェクト
/// </summary>
class Object_glTF
{
public:
	Object_glTF();
	~Object_glTF();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="worldTransform"></param> ワールド座標系
	void Update(const WorldTransform& worldTransform);
	/// <summary>
	/// 更新処理
	/// </summary>	
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>	
	void Draw();
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="textureData"></param> テクスチャ変更
	void Draw(const std::string& textureData);
	/// <summary>
	/// アニメーションの更新処理
	/// </summary>
	void AnimationUpdate();
	/// <summary>
	/// setter_modelのファイル名
	/// </summary>
	/// <param name="filePath"></param>
	void SetModelFile(const std::string& filePath);
	/// <summary>
	/// ライトのon/off
	/// </summary>
	/// <param name="isLight"></param>
	void LightSwitch(bool isLight);
	/// <summary>
	/// 環境マップ用
	/// </summary>
	/// <param name="filePath"></param>
	void SetEnvironment(const std::string& filePath);

	void ChangeAnimation(const std::string& filePath);
	Material* GetMaterial() { return material_; };

private:	
	/// <summary>
	/// アニメーション
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="animation"></param>
	/// <param name="animationTime"></param>
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
	/// <summary>
	/// joint読み込みのアニメーション操作
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="animation"></param>
	/// <param name="nextAnimation"></param>
	/// <param name="animationTime"></param>
	void Interpolation(Skeleton& skeleton, const Animation& animation, const Animation& nextAnimation, float animationTime);
	/// <summary>
	/// スケルトン処理
	/// </summary>
	/// <param name="skeleton"></param>スケルトンデータ
	void SkeletonUpdate(Skeleton& skeleton);
	/// <summary>
	/// スケルトン処理
	/// </summary>
	/// <param name="skeleton"></param>スケルトンデータ
	/// <param name="matWorld"></param>ワールド座標
	void SkeletonUpdate(Skeleton& skeleton,const Matrix4x4& matWorld);
	/// <summary>
	/// スキニング処理
	/// </summary>
	/// <param name="skinCluster"></param>スキニングデータ
	/// <param name="skeleton"></param>スケルトンデータ
	void SkinClusterUpdate(SkinCluster& skinCluster ,const Skeleton& skeleton);

	GLTFCommon* object3dCommon_ = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources_;
	std::vector<TransformationMatrix*> wvpDatas_;

	//ライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource_;
	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightSphereData_ = nullptr;

	//カメラ用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	//カメラデータを書き込む
	CameraForGPU* cameraData_ = nullptr;

	//ポイントライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	//マテリアルにデータを書き込む
	PointLight* pointLightData_ = nullptr;

	//スポットライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	//マテリアルにデータを書き込む
	SpotLight* spotLightData_ = nullptr;

	Model_glTF* model_ = nullptr;
	Camera* camera_ = nullptr;

	ModelData_glTF modelData_;


	std::vector<Animation> animations_;
	///アニメーションタイマー
	float animationTime_ = 0.0f;
	/// 補間タイマー
	float changeTime_ = 0.0f;
	
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	std::vector<SphereModel*> debugSpheres_;
	void SetWireframe();

	bool isChange_ = false;

	//変更前のアニメーション
	std::vector<Animation> preAnimations_;

	Material* material_;

	Matrix4x4 worldMatrix_;
	std::vector<Matrix4x4> localMatrices_;
};