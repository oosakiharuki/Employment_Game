#include "Model_glTF.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace MyMath;

void Model_glTF::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName) {
	this->modelCommon = modelCommon;

	//.gltf
	modelData = LoadModelFile(directorypath, fileName);
	animation = LoadAnimationFile(directorypath, fileName);

	InitialData = modelData;

	//vertex
	vertexResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	//index
	indexResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * modelData.indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * modelData.indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;


	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());


	//Model用マテリアル
	//マテリアル用のリソース
	materialResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = 70;
	materialData->environmentCoefficient = 0.3f;

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);

	skeleton = CreateSkeltion(modelData.rootNode);
	skinCluster = CreateSkinCluster(skeleton,modelData);

	vbvs[0] = vertexBufferView;
	vbvs[1] = skinCluster.influenceBufferView;

}

void Model_glTF::Draw() {
	//objファイルに元々あったテクスチャ
	modelData = InitialData;
	
	modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	//modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	modelCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(7, skinCluster.paletteSrvHandle.second);//Skinning.VS t0
	
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(8, TextureManager::GetInstance()->GetSrvHandleGPU(EnvironmentFile));
	//modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	modelCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);

}

void Model_glTF::Draw(const std::string& textureFilePath) {

	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	modelData.material.textureFilePath = textureFilePath;
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	
	modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	//modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	modelCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(7, skinCluster.paletteSrvHandle.second);//Skinning.VS t0
	//modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	modelCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);

}


MaterialData Model_glTF::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//ファイルを開く
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/Sprite/" + textureFilename;
		}
	}
	return materialData;
};


//ModelData Model_glTF::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//	ModelData modelData;
//
//	Assimp::Importer importer;
//	std::string filePath = directoryPath + "/" + filename;
//
//	const aiScene* scene = importer.ReadFile(filePath.c_str(),aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
//	assert(scene->HasMeshes()); //メッシュがないのは対応なし
//
//	//VertexDataを読み取る
//	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
//		aiMesh* mesh = scene->mMeshes[meshIndex];
//		assert(mesh->HasNormals());//法線があるか
//		assert(mesh->HasTextureCoords(0));//Texcordがあるか
//
//		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
//			aiFace& face = mesh->mFaces[faceIndex];
//			assert(face.mNumIndices == 3);//三角形のみ
//			
//			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
//				uint32_t vertexIndex = face.mIndices[element];
//				aiVector3D& position = mesh->mVertices[vertexIndex];
//				aiVector3D& normal = mesh->mNormals[vertexIndex];
//				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
//
//				VertexData vertex;
//				vertex.position = { position.x,position.y,position.z,1.0f };
//				vertex.normal = { normal.x,normal.y, normal.z, };
//				vertex.texcoord = { texcoord.x,texcoord.y };
//
//				//aiProcess_MakeleftHandleなので z *= -1,右手→左手(x *= -1)に変換する
//				vertex.position.x *= -1.0f;
//				vertex.normal.x *= -1.0f;
//
//				modelData.vertices.push_back(vertex);
//			}
//		}
//
//		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
//			aiFace& face = mesh->mFaces[faceIndex];
//			assert(face.mNumIndices == 3);//三角形のみ
//
//			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
//				uint32_t vertexIndex = face.mIndices[element];
//				modelData.indices.push_back(vertexIndex);
//			}
//		}
//
//
//	}	
//	//MaterialData
//	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
//		aiMaterial* material = scene->mMaterials[materialIndex];
//		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
//			aiString textureFilePath;
//			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
//			modelData.material.textureFilePath = directoryPath + "/Sprite/" + textureFilePath.C_Str();		
//		}
//
//	}	
//	//オブジェクトには親子ノードがないので単品
//	Node result;
//	result.localMatrix = MakeIdentity4x4();
//	modelData.rootNode = result;
//
//	return modelData;
//}

ModelData_glTF Model_glTF::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData_glTF modelData;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); //メッシュがないのは対応なし

	//VertexDataを読み取る
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線があるか
		assert(mesh->HasTextureCoords(0));//Texcordがあるか
		modelData.vertices.resize(mesh->mNumVertices);//頂点数分のメモリ確保

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y, normal.z, };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみ

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}

		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			
			//jointごとの格納領域作成
			aiBone* bone = mesh->mBones[boneIndex]; // Bone(骨) = joint
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterDeta[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();//BindPoseMatrixに戻す
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);//成分
			//右手系ベクトルのため回転、X座標を変える
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
				{ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z }
			);
			//BindPoseMatrix作成
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	modelData.material.textureFilePath = directoryPath + "/Sprite/uvChecker.png";
	//MaterialData
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/Sprite/" + textureFilePath.C_Str();
		}

	}
	
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

Node Model_glTF::ReadNode(aiNode* node) {
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale = { scale.x ,scale.y ,scale.z };
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translate = { -translate.x,translate.y ,translate.z };
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);


	result.name = node->mName.C_Str();//nodeの名前
	result.children.resize(node->mNumChildren);//子供の数
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰関数で階級構造
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}


Animation  Model_glTF::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	Animation animation;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがないとき
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションのみ。複数はまだ
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間単位を秒に

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			keyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };//xはマイナス
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			keyframeQuatarnion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			//y,zを右手から左手に変更するため" - "に
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y ,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			keyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}


	return animation;
}

SkinCluster Model_glTF::CreateSkinCluster(const Skeleton& skeleton,const ModelData_glTF& modelData) {


	SkinCluster skinCluster;
	uint32_t IndexNum = 0;

	///t0
	// paletteResource確保
	skinCluster.paletteResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//sponのサイズ設定
	
	IndexNum = SrvManager::GetInstance()->Allocate();
	skinCluster.paletteSrvHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(IndexNum);
	skinCluster.paletteSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(IndexNum);

	//palette用のsrvを作成 StructuredBufferでアクセスできるようにする
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	modelCommon->GetDxCommon()->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);
	///

	///WEIGHT INDEXのやつ
	//influenceResource確保
	skinCluster.influenceResource  = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

	//InfluenceのVBV
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//inverseBindPoseMatrixを格納場所、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	//beginからendまでMakeIdentity4x4の値を与える
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(),MakeIdentity4x4);
	///

	for (const auto& jointWeight : modelData.skinClusterDeta) {
		auto it = skeleton.jointMap.find(jointWeight.first);
		if (it == skeleton.jointMap.end()) {
			continue;
		}

		//
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}


	return skinCluster;
}

//環境マップのテクスチャをもらう
void Model_glTF::SetEnvironment(const std::string mapFile) {
	EnvironmentFile = mapFile;
}