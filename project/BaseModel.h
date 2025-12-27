#pragma once
#include "ModelCommon.h"
#include "MyMath.h"
#include <memory>

class BaseModel
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="directorypath"></param>
	/// <param name="fileName"></param>
	virtual void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName) = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

protected:
	
	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;


	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource_;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_;
	VertexData* vertexData_ = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources_;
	Material* materialData_ = nullptr;

	ModelDataMulti modelData_;
};

