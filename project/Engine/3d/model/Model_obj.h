#pragma once
#include "BaseModel.h"
#include <string>
#include "AnimationMath.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// .obj版のモデル(派生クラス)
	/// </summary>
	class Model_obj : public BaseModel {
	public:
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="directoryPath"></param>
		/// <param name="fileName"></param>
		void Initialize(const std::string& directoryPath, const std::string& fileName) override;

		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw() override;

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="textureFilePath"></param>テクスチャ変更
		void Draw(const std::string& textureFilePath);

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
		/// IndexResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		void InitIndexResource(ModelData modelData) override;

		/// <summary>
		/// テクスチャとマテリアルの作成
		/// </summary>
		/// <param name="directoryPath">リソースファイル(resource)</param>
		/// <param name="filename">テクスチャファイルパス</param>
		/// <param name="useMtl">マテリアル名</param>
		/// <returns></returns>
		static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& useMtl);
		/// <summary>
		/// .objファイルを読み取りモデルデータを作る
		/// </summary>
		/// <param name="directoryPath">リソースファイル(resource)</param>
		/// <param name="filename">テクスチャファイルパス</param>
		/// <param name="objName">オブジェクト拡張子</param>
		/// <returns>完成したモデルデータ(マルチマテリアル付き)</returns>
		static MyMath::ModelDataMulti LoadObjFile(const std::string& directoryPath, const std::string& filename, const std::string& objName);

		MyMath::ModelDataMulti InitialData_;
	};
}