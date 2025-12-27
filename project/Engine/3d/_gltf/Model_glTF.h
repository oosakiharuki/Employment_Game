#pragma once
#include "BaseModel.h"
#include <assimp/scene.h>

/// <summary>
/// .gltf版のモデル
/// </summary>
class Model_glTF : public BaseModel{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelCommon"></param>
	/// <param name="directorypath"></param>
	/// <param name="fileName"></param>
	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& fileName) override;
	
	/// <summary>
	/// アニメーション
	/// </summary>
	/// <param name="isAnimation">アニメーションを入れるか</param>
	/// <param name="isSkinning">スキニングであるか</param>
	void InitAnimation(bool isAnimation, bool isSkinning);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	//gltf用
	static ModelDataMulti LoadModelFile(const std::string& directoryPath, const std::string& filename);
	static std::vector<Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename,uint32_t Number);

	/// <summary>
	/// ライト設定
	/// </summary>
	/// <param name="Light"></param>
	void LightOn(bool Light) {
		for (auto& material : materialResources_) {
			//書き込むためのアドレス
			material->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
			//全てのライトを on/off
			materialData_->enableLighting = Light;
		}
	}
	
	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="mapFile"></param>
	void SetEnvironment(const std::string& mapFile);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	static Node ReadNode(aiNode* node);

	/// <summary>
	/// getter_オブジェクトデータ
	/// </summary>
	/// <returns></returns>
	ModelDataMulti GetModelData() { return modelData_; }

	/// <summary>
	/// getter_アニメーションデータ
	/// </summary>
	/// <returns></returns>
	std::vector<Animation> GetAnimationData() { return animation_; }

	/// <summary>
	/// getter_スケルトンデータ
	/// </summary>
	/// <returns></returns>
	std::vector<Skeleton> GetSkeleton() { return skeletons_; }
	/// <summary>
	/// getter_スキンデータ
	/// </summary>
	/// <returns></returns>	
	std::vector<SkinCluster> GetSkinCluster() { return skinClusters_; }

	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelDataMulti& modelData);

	Material* GetMaterial() { return materialData_; }
	/// <summary>
	/// スキニングデータを使用しているか
	/// </summary>
	/// <returns></returns>
	bool IsSkinning() const { return isSkinning_; }
	/// <summary>
	/// アニメーションを使用しているか
	/// </summary>
	/// <returns></returns>
	bool IsAnimation() const { return isAnimation_; }

	/// <summary>
	/// マルチメッシュで使う用のカウントをリセット
	/// </summary>
	void ResetMeshCount() { multiMeshCount_ = 0; }

private:
	ModelCommon* modelCommon_ = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource_; //index

	uint32_t* mappedIndex_ = nullptr;

	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView_; //index

	ModelDataMulti InitialData_;
	
	//アニメーション
	std::vector<Animation> animation_;

	std::vector<Skeleton> skeletons_;

	std::vector<SkinCluster> skinClusters_;

	D3D12_VERTEX_BUFFER_VIEW vbvs_[2];
	
	std::string EnvironmentFile_;

	bool isAnimation_ = false;
	bool isSkinning_ = false;

	uint32_t multiMeshCount_ = 0;

};