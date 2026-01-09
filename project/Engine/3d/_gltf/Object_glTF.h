#pragma once
#include "Model_glTF.h"
#include "SphereModel.h"
#include "BaseObject.h"

class GLTFCommon;
/// <summary>
/// .gltf版のオブジェクト
/// </summary>
class Object_glTF : public BaseObject
{
public:
	Object_glTF();
	~Object_glTF();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="worldTransform"></param> ワールド座標系
	void Update(WorldTransform& worldTransform) override;
	/// <summary>
	/// 更新処理
	/// </summary>	
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>	
	void Draw() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="textureData"></param> テクスチャ変更
	void Draw(const std::string& textureData) override;

	/// <summary>
	/// setter_modelの選択
	/// </summary>
	/// <param name="filePath">ファイル名</param>
	void SetModelFile(const std::string& filePath) override;

	/// <summary>
	/// ライトのon/off
	/// </summary>
	/// <param name="isLight"></param>
	void LightSwitch(bool isLight)override;

	/// <summary>
	/// 色の変更
	/// </summary>
	/// <param name="color">変更カラー</param>
	void SetColor(const Vector4& color) override;

	/// <summary>
	/// 環境マップ用
	/// </summary>
	/// <param name="filePath"></param>
	void SetEnvironment(const std::string& filePath);

	/// <summary>
	/// アニメーションを変更
	/// </summary>
	/// <param name="filePath">変更するファイルパス</param>
	void ChangeAnimation(const std::string& filePath);

private:

	/// <summary>
	/// アニメーションの更新処理
	/// </summary>
	void AnimationUpdate();
	/// <summary>
	/// フレームアニメーションの更新
	/// </summary>
	void FrameAnimation();

	/// <summary>
	/// カメラ更新処理
	/// </summary>
	void CameraUpdate();

	void CreateWVP();

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

	Model_glTF* model_ = nullptr;

	ModelDataMulti modelData_;


	std::vector<Animation> animations_;
	///アニメーションタイマー
	float animationTime_ = 0.0f;
	/// 補間タイマー
	float changeTime_ = 0.0f;
	
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	std::vector<SphereModel*> debugSpheres_;
	/// <summary>
	/// デバッグ用のワイヤーフレームを作成
	/// </summary>
	void SetWireframe();

	bool isChange_ = false;

	//変更前のアニメーション
	std::vector<Animation> preAnimations_;

	std::vector<Matrix4x4> localMatrices_;

	Matrix4x4 WorldViewProjectionMatrix;
};