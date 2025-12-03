#pragma once
#include "MyMath.h"
#include "ModelCommon.h"
/// <summary>
/// .obj版のモデル
/// </summary>
class Model_obj{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelCommon"></param>
	/// <param name="directorypath"></param>
	/// <param name="fileName"></param>
	/// <param name="objName"></param>
	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& fileName,const std::string& objName);
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
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

	ModelDataMulti modelData_;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;


	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_;

	ModelDataMulti InitialData_;
};