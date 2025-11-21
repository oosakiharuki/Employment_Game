#include "Model_obj.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>

using namespace MyMath;

void Model_obj::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName, const std::string& objName) {
	this->modelCommon_ = modelCommon;
	// (resource) / (Object / モデルファイル) / (オブジェクト名.obj)
	modelData_ = LoadObjFile(directorypath, fileName, objName);

	InitialData_ = modelData_;

	int i = 0;
	//全ての頂点数
	for (auto& multi : modelData_.vertices) {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexR;
		D3D12_VERTEX_BUFFER_VIEW vertexB;

		vertexR = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * multi.size());

		vertexB.BufferLocation = vertexR->GetGPUVirtualAddress();
		vertexB.SizeInBytes = UINT(sizeof(VertexData) * multi.size());
		vertexB.StrideInBytes = sizeof(VertexData);

		vertexR->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
		std::memcpy(vertexData_, multi.data(), sizeof(VertexData) * multi.size());

		vertexResource_.push_back(vertexR);
		vertexBufferView_.push_back(vertexB);


	}
	for (auto& material : modelData_.material) {
		//テクスチャ読み込み
		TextureManager::GetInstance()->LoadTexture(material.textureFilePath);
		material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(material.textureFilePath);
	}

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource_ = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色の設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 70;

}

void Model_obj::Draw() {
	int i = 0;
	for (auto& multi : modelData_.vertices) {
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material[i].textureFilePath));
		modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(multi.size()), 1, 0, 0);
		i++;
	}
}

void Model_obj::Draw(const std::string& textureFilePath) {
	for (auto& material : modelData_.material) {
		material.textureFilePath = textureFilePath;
	}

	int i = 0;
	for (auto& multi : modelData_.vertices) {
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material[i].textureFilePath));
		modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(multi.size()), 1, 0, 0);
		i++;
	}
}

MaterialData Model_obj::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& usemtl) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	bool isSame = false;
	//ファイルを開く
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if (identifier == "newmtl") {
			std::string mtlName;
			s >> mtlName;

			//使用したいマテリアルならtrue
			if (usemtl == mtlName) {
				isSame = true;
			}
		}
		if (identifier == "map_Kd" && isSame) {
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/Sprite/" + textureFilename;
			isSame = false;
		}
	}
	return materialData;
};


ModelDataMulti Model_obj::LoadObjFile(const std::string& directoryPath, const std::string& filename, const std::string& objName) {
	ModelDataMulti modelData;

	//VertexData
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	//ファイルから読んだ1行を格納する
	std::string line;
	//ファイルを読み取る
	std::ifstream file(directoryPath + "/" + filename + "/" + objName);
	assert(file.is_open());

	//mtlファイルを読み取る
	std::string materialFilename;

	//マテリアルの頂点情報を別々に作る
	std::vector<VertexData> iVertices;
	bool firstMaterial = false;

	//構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の義別子 (v ,vt, vn, f) を読み取る

		//modeldataの建築
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;//左から順に消費 = 飛ばしたり、もう一度使うことはできない	
			position.s = 1.0f;

			//反転
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;

			//原点変更
			texcoord.y = 1.0f - texcoord.y;

			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;

			//反転
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];

			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //  "/"でインデックスを区切る
					elementIndices[element] = std::stoi(index);

				}


				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);

				triangle[faceVertex] = { position,texcoord,normal };

			}
			iVertices.push_back(triangle[2]);
			iVertices.push_back(triangle[1]);
			iVertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib") {
			s >> materialFilename;
		}
		else if (identifier == "o") {
			//各マテリアルの頂点情報を取得
			if (firstMaterial) {
				modelData.vertices.push_back(iVertices);
				iVertices.clear();
			} else {
				//最初の o はパス
				firstMaterial = true;
			}
		}
		else if (identifier == "usemtl") {
			std::string mtlName;
			s >> mtlName;
			MaterialData material;

			material = LoadMaterialTemplateFile(directoryPath, filename + "/" + materialFilename, mtlName);
			modelData.material.push_back(material);
		}
	}

	//最後のマテリアル頂点情報を取得
	modelData.vertices.push_back(iVertices);

	return modelData;
}


