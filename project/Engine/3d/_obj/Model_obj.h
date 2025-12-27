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
	/// 描画処理
	/// </summary>
	/// <param name="textureFilePath"></param>テクスチャ変更
	void Draw(const std::string& textureFilePath);

	void LightOn(bool Light) { materialData_->enableLighting = Light; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

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