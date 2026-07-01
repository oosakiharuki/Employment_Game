/// --------------------------
///
/// .objファイル型の3Dモデル
/// 
/// --------------------------
#include "Model_obj.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>

using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Model_obj::Initialize(const std::string& directoryPath, const std::string& fileName) {
		// (resource) / (Object / モデルファイル) / (オブジェクト名.obj)
		modelData_ = LoadObjFile(directoryPath, fileName, ".obj");

		InitialData_ = modelData_;

		int i = 0;
		//全ての頂点数
		for (auto& modelData : modelData_.Data) {

			InitVertexResource(modelData);

			InitMaterialResource(modelData);
		}
	}

	void Model_obj::InitVertexResource(ModelData modelData) {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexR;
		D3D12_VERTEX_BUFFER_VIEW vertexB;


		std::vector<VertexData> vertexDatas = modelData.vertices;

		vertexR = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(VertexData) * vertexDatas.size());

		vertexB.BufferLocation = vertexR->GetGPUVirtualAddress();
		vertexB.SizeInBytes = UINT(sizeof(VertexData) * vertexDatas.size());
		vertexB.StrideInBytes = sizeof(VertexData);

		vertexR->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
		std::memcpy(vertexData_, vertexDatas.data(), sizeof(VertexData) * vertexDatas.size());

		vertexResource_.push_back(vertexR);
		vertexBufferView_.push_back(vertexB);
	}

	void Model_obj::InitMaterialResource(ModelData modelData) {

		MaterialData materialData = modelData.materialData;

		//テクスチャ読み込み
		TextureManager::GetInstance().LoadTexture(materialData.textureFilePath);
		materialData.textureIndex = TextureManager::GetInstance().GetSrvIndex(materialData.textureFilePath);

		//Model用マテリアル
		//マテリアル用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		materialResource = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(Material));
		//書き込むためのアドレス
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		//色の設定
		materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		materialData_->enableLighting = true;
		materialData_->uvTransform = modelData.materialData.uvTransform;
		materialData_->shininess = 70;

		//vector
		materialResources_.push_back(materialResource);
	}

	void Model_obj::InitIndexResource(ModelData modelData) {}

	void Model_obj::Draw() {

		//元々のデータを読み取る
		modelData_ = InitialData_;

		int i = 0;
		for (auto& multi : modelData_.Data) {
			DirectXCommon::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[i]->GetGPUVirtualAddress());
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance().GetSrvHandleGPU(multi.materialData.textureFilePath));
			DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(UINT(multi.vertices.size()), 1, 0, 0);
			i++;
		}
	}

	void Model_obj::Draw(const std::string& textureFilePath) {
		int i = 0;
		for (auto& multi : modelData_.Data) {
			//テクスチャを変更
			multi.materialData.textureFilePath = "resource/Sprite/" + textureFilePath + ".dds";

			//テクスチャ読み込み
			TextureManager::GetInstance().LoadTexture(multi.materialData.textureFilePath);
			multi.materialData.textureIndex = TextureManager::GetInstance().GetSrvIndex(multi.materialData.textureFilePath);


			DirectXCommon::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_[i]);
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[i]->GetGPUVirtualAddress());
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance().GetSrvHandleGPU(multi.materialData.textureFilePath));
			DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(UINT(multi.vertices.size()), 1, 0, 0);
			i++;
		}
	}

	MaterialData Model_obj::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& useMtl) {
		MaterialData materialData;
		std::string line;
		std::ifstream file(directoryPath + "/Object/" + filename);
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
				if (useMtl == mtlName) {
					isSame = true;
				}
			}
			if (identifier == "map_Kd" && isSame) {
				std::string textureFilename;
				s >> textureFilename;

				//テクスチャUVのトランスフォーム
				//aiUVTransform uvTransform;
				materialData.uvTransform = MakeIdentity4x4();//通常行列
				//マッピング(位置)
				if (textureFilename == "-o") {
					//座標位置の変更
					Vector3 uvTranslate;
					s >> uvTranslate.x >> uvTranslate.y >> uvTranslate.z;
					materialData.uvTransform = MakeTranslateMatrix(uvTranslate);//行列にして移行
					s >> textureFilename;
				}
				//マッピング(サイズ)
				if (textureFilename == "-s") {
					//通常行列にスケールをかける
					Vector3 uvScale;//大きさを設定
					s >> uvScale.x >> uvScale.y >> uvScale.z;
					materialData.uvTransform = materialData.uvTransform * MakeScaleMatrix(uvScale);//行列にしてかける
					s >> textureFilename;
				}

				size_t pos1;
				std::string extension;

				//最後の'.'を読み込む  ○○'.'png
				pos1 = textureFilename.find('.');

				//npos = 拡張子がない
				if (pos1 != std::wstring::npos) {
					extension = textureFilename.substr(0, pos1);
				}

				materialData.textureFilePath = directoryPath + "/Sprite/" + extension + ".dds";
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
		std::ifstream file(directoryPath + "/Object/" + filename + "/" + filename + objName);
		assert(file.is_open());

		//mtlファイルを読み取る
		std::string materialFilename;

		//各モデルデータで読み出しするデータ
		ModelData iModelData;


		//マテリアルの頂点情報を別々に作る
		std::vector<VertexData> iVertices;
		bool firstMaterial = false;

		uint32_t vertexNum = 0;
		uint32_t materialNum = 0;

		//構築
		while (std::getline(file, line)) {
			std::string identifier;
			std::istringstream s(line);
			s >> identifier; //先頭の義別子 (v ,vt, vn, f) を読み取る

			//modelDataの建築
			if (identifier == "v") {//点座標
				Vector4 position;
				s >> position.x >> position.y >> position.z;//左から順に消費 = 飛ばしたり、もう一度使うことはできない	
				position.s = 1.0f;

				//反転
				position.x *= -1.0f;
				positions.push_back(position);
			}
			else if (identifier == "vt") {//texcord
				Vector2 texcoord;
				s >> texcoord.x >> texcoord.y;

				//原点変更
				texcoord.y = 1.0f - texcoord.y;

				texcoords.push_back(texcoord);
			}
			else if (identifier == "vn") {//法線
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
					iModelData.vertices = iVertices;//vectorで追加するmodelDataのvertexDataに導入
					vertexNum++;//カウントを進む
					iVertices.clear();//データをリセット
				}
				else {
					//最初の o はパス
					firstMaterial = true;
				}
			}
			else if (identifier == "usemtl") {
				std::string mtlName;
				s >> mtlName;
				MaterialData material;
				//マテリアルを読み取る
				material = LoadMaterialTemplateFile(directoryPath, filename + "/" + materialFilename, mtlName);
				iModelData.materialData = material;//vectorで追加するmodelDataのmaterialDataに導入
				materialNum++;
			}
			//マテリアル、頂点データともに読み取った時(両方とも0のときは発動しない)
			if (materialNum == vertexNum && materialNum > 0) {
				//値を導入
				modelData.Data.push_back(iModelData);
			}

		}

		iModelData.vertices = iVertices;

		//最後のマテリアル頂点情報を取得
		modelData.Data.push_back(iModelData);

		return modelData;
	}


}