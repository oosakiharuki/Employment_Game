#pragma once
#include "BaseModel.h"

/// <summary>
/// .obj版のモデル
/// </summary>
class Model_obj : public BaseModel{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="directorypath"></param>
	/// <param name="fileName"></param>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName) override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

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
	/// 描画処理
	/// </summary>
	/// <param name="textureFilePath"></param>テクスチャ変更
	void Draw(const std::string& textureFilePath);

private:	
	
	/// <summary>
	/// テクスチャとマテリアルの作成
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <param name="usemtl"></param>
	/// <returns></returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& usemtl);
	/// <summary>
	/// .objファイルを読み取る
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <param name="objName"></param>
	/// <returns></returns>
	static ModelDataMulti LoadObjFile(const std::string& directoryPath, const std::string& filename, const std::string& objName);
	
	ModelCommon* modelCommon_ = nullptr;

	ModelDataMulti InitialData_;
};