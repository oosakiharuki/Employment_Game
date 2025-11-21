#pragma once
#include "MyMath.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "Cubemap.h"
#include "Primitive.h"
/// <summary>
/// 四角型モデル(キューブマップ用)
/// </summary>
class BoxModel {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="textureFile"></param>
	void Initialize(std::string textureFile);
	void Update(Matrix4x4 matworld);
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// setter_色
	/// </summary>
	/// <param name="color"></param>
	void SetColor(Vector4 color) { color_ = color; }

private:

	Cubemap* cubemap_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;
	Camera* camera_ = nullptr;


	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;


	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	Vector4 color_ = { 1,1,1,1 };
};