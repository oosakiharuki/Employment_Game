#pragma once
#include "ModelCommon.h"
#include "MyMath.h"
#include <memory>

/// <summary>
/// Modelの基盤クラス
/// </summary>
class BaseModel
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelCommon"></param>
	/// <param name="directorypath"></param>
	/// <param name="fileName"></param>
	virtual void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName) = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// VertexResource作成(初期化)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	virtual void InitVertexResource(ModelData modelData) = 0;

	/// <summary>
	/// MaterialResource作成(初期化)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	virtual void InitMaterialResource(ModelData modelData) = 0;

	/// <summary>
	/// IndexResource作成(初期化)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	virtual void InitIndexResource(ModelData modelData) = 0;

	/// <summary>
	/// マテリアルの色を変更
	/// </summary>
	/// <param name="color">変更カラー</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// ライト設定
	/// </summary>
	/// <param name="Light"></param>
	void LightOn(bool Light);

protected:
	
	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;


	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource_;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_;
	VertexData* vertexData_ = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources_;
	Material* materialData_ = nullptr;

	ModelDataMulti modelData_;
};

