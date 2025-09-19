#pragma once
#include "MyMath.h"
#include "ModelCommon.h"

class Model_obj{
public:

	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& fileName,const std::string& objName);

	void Draw();
	void Draw(const std::string& textureFilePath);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& usemtl);
	static ModelDataMulti LoadObjFile(const std::string& directoryPath, const std::string& filename, const std::string& objName);
	
	void LightOn(bool Light) { materialData->enableLighting = Light; }
	void SetColor(Vector4 color) { materialData->color = color; }

private:
	ModelCommon* modelCommon = nullptr;

	ModelDataMulti modelData;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;


	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView;

	ModelDataMulti InitialData;
};