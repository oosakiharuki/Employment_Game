#pragma once
#include "MyMath.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "Cubemap.h"

class BoxModel {
public:
	void Initialize(std::string textureFile);
	void Update(Matrix4x4 matworld);

	void Draw();


	struct Sphere {
		Vector3 center;
		float radius;
	};


	void CreateBox();

	void SetColor(Vector4 color) { color_ = color; }

private:

	Cubemap* cubemap = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	TransformationMatrix* wvpData = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	CameraForGPU* cameraData = nullptr;
	Camera* camera = nullptr;


	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;


	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	Vector4 color_ = { 1,1,1,1 };
};