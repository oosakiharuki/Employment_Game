#pragma once
#include "BaseModel.h"
#include <assimp/scene.h>

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// 構造体_スケルトンデータ
	/// </summary>
	struct Skeleton {
		int32_t root;//RootJointのIndex
		std::map <std::string, int32_t>jointMap;//コンテナ
		std::vector<MyMath::Joint> joints;//所属ジョイント
	};

	/// <summary>
	/// 構造体_スケルトンで使用
	/// </summary>
	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix; //位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix; //法線用
	};

	//std::spanは「配列の上限がない」みたいなやつ（決めることもできる）

	/// <summary>
	/// 構造体_スキニング
	/// </summary>
	struct SkinCluster {
		std::vector<Matrix4x4> inverseBindPoseMatrices;
		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<MyMath::VertexInfluence> mappedInfluence;
		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	};

	/// <summary>
	/// .gltf版のモデル(派生クラス)
	/// </summary>
	class Model_glTF : public BaseModel {
	public:
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="directoryPath">リソースファイルパス</param>
		/// <param name="fileName">オブジェクト名</param>
		void Initialize(const std::string& directoryPath, const std::string& fileName) override;

		/// <summary>
		/// アニメーション
		/// </summary>
		/// <param name="isAnimation">アニメーションを入れるか</param>
		/// <param name="isSkinning">スキニングであるか</param>
		void InitAnimation(const std::string& directoryPath, const std::string& fileName, bool isAnimation, bool isSkinning);

		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw() override;

		/// <summary>
		/// setter_環境マップの設定
		/// </summary>
		/// <param name="mapFile">環境マップ</param>
		void SetEnvironment(const std::string& mapFile);

		/// <summary>
		/// getter_モデルデータ
		/// </summary>
		/// <returns>現在使用しているモデルのデータ</returns>
		MyMath::ModelDataMulti GetModelData() { return modelData_; }

		/// <summary>
		/// getter_アニメーションデータ
		/// </summary>
		/// <returns>現在使用しているアニメーションのデータ</returns>
		std::vector<MyMath::Animation> GetAnimationData() { return animation_; }

		/// <summary>
		/// getter_スケルトンデータ
		/// </summary>
		/// <returns>現在使用しているアニメーションのスケルトンデータ</returns>
		std::vector<Skeleton> GetSkeleton() { return skeletons_; }
		/// <summary>
		/// getter_スキニングデータ
		/// </summary>
		/// <returns>現在使用しているアニメーションのスキニングデータ</returns>	
		std::vector<SkinCluster> GetSkinCluster() { return skinClusters_; }
		/// <summary>
		/// スキニングの作成
		/// </summary>
		/// <param name="skeleton">現在のアニメーションのスケルトンデータ</param>
		/// <param name="modelData">モデルデータ</param>
		/// <returns>完成したスキニングデータ</returns>
		SkinCluster CreateSkinCluster(const Skeleton& skeleton, const MyMath::ModelDataMulti& modelData);
		/// <summary>
		/// getter_マテリアルデータ
		/// </summary>
		/// <returns>現在のモデルのマテリアルデータ</returns>
		Material* GetMaterial() { return materialData_; }
		/// <summary>
		/// スキニングを使用しているか
		/// </summary>
		/// <returns>true している / false していない</returns>
		bool IsSkinning() const { return isSkinning_; }
		/// <summary>
		/// アニメーションを使用しているか
		/// </summary>
		/// <returns>true している / false していない</returns>
		bool IsAnimation() const { return isAnimation_; }

		/// <summary>
		/// マルチメッシュで使う用のカウントをリセット
		/// </summary>
		void ResetMeshCount() { multiMeshCount_ = 0; }

	private:

		/// <summary>
		/// VertexResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		void InitVertexResource(ModelData modelData) override;

		/// <summary>
		/// MaterialResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		void InitMaterialResource(ModelData modelData) override;

		/// <summary>
		/// MaterialResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		void InitIndexResource(ModelData modelData) override;

		/// <summary>
		/// ノードを読み込む
		/// </summary>
		/// <param name="node">ノード</param>
		/// <returns>アニメーションのノードを作成</returns>
		static MyMath::Node ReadNode(aiNode* node);

		/// <summary>
		/// モデルを読み込む
		/// </summary>
		/// <param name="directoryPath">リソースファイル(resource)</param>
		/// <param name="filename">使用するテクスチャ名</param>
		/// <returns>完成したモデルデータ</returns>
		static MyMath::ModelDataMulti LoadModelFile(const std::string& directoryPath, const std::string& filename);

		/// <summary>
		/// アニメーションを読み込む
		/// </summary>
		/// <param name="directoryPath">リソースファイル(resource)</param>
		/// <param name="filename">使用するテクスチャ名</param>
		/// <param name="Number">アニメーションの数(マルチメッシュ用)</param>
		/// <returns>完成したアニメーション</returns>
		static std::vector<MyMath::Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename, uint32_t Number);


		/// <summary>
		/// スケルトンを生成
		/// </summary>
		/// <param name="rootNode">ノード</param>
		/// <returns>作成したスケルトン</returns>
		Skeleton CreateSkelton(const MyMath::Node& rootNode);

		/// <summary>
		/// ジョイント(関節部分)を生成
		/// </summary>
		/// <param name="node">ノード</param>
		/// <param name="parent">親子関係</param>
		/// <param name="joints">ジョイント</param>
		/// <returns>生成したジョイント</returns>
		int32_t CreateJoint(const MyMath::Node& node, const std::optional<int32_t>& parent, std::vector<MyMath::Joint>& joints);

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource_; //index

		uint32_t* mappedIndex_ = nullptr;

		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView_; //index

		//アニメーション
		std::vector<MyMath::Animation> animation_;
		//スケルトン
		std::vector<Skeleton> skeletons_;
		//スキニング
		std::vector<SkinCluster> skinClusters_;
		//VertexBufferView二つ(通常アニメーション用、スキニングアニメーション用)
		D3D12_VERTEX_BUFFER_VIEW vbvs_[2];
		//環境マップのファイルパス
		std::string EnvironmentFile_;

		bool isAnimation_ = false;//アニメーション使用フラグ
		bool isSkinning_ = false;//スキニング使用フラグ

		uint32_t multiMeshCount_ = 0;//メッシュの数をカウントする

	};
}