#include "Model_glTF.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace MyMath;

void Model_glTF::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& fileName, bool isAnimation, bool isSkinning) {
	this->modelCommon_ = modelCommon;

	//.gltf
	modelData_ = LoadModelFile(directorypath, fileName);
	if (isAnimation) {
		animation_ = LoadAnimationFile(directorypath, fileName,uint32_t(modelData_.indices.size()));
	}

	InitialData_ = modelData_;

	//vertex
	for (auto& vertices : modelData_.vertices) {
		D3D12_VERTEX_BUFFER_VIEW vertexB;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexR;

		vertexR = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * vertices.size());

		vertexB.BufferLocation = vertexR->GetGPUVirtualAddress();
		vertexB.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
		vertexB.StrideInBytes = sizeof(VertexData);

		vertexR->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
		std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

		vertexResource_.push_back(vertexR);
		vertexBufferView_.push_back(vertexB);

	}

	//index
	for (auto& indices : modelData_.indices) {
		D3D12_INDEX_BUFFER_VIEW indexB;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexR;

		indexR = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * indices.size());

		indexB.BufferLocation = indexR->GetGPUVirtualAddress();
		indexB.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
		indexB.Format= DXGI_FORMAT_R32_UINT;

		indexR->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
		std::memcpy(mappedIndex_, indices.data(), sizeof(uint32_t) * indices.size());

		indexBufferView_.push_back(indexB);
		indexResource_.push_back(indexR);
	}

	//Model用マテリアル
	//マテリアル用のリソース
	for (auto& material : modelData_.material) {
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		materialResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレス
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		//色の設定
		if (material.materialColor.s) {
			//baseColor設定
			materialData_->color = material.materialColor;
		}
		else {
			materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		materialData_->enableLighting = false;
		materialData_->uvTransform = MakeIdentity4x4();
		materialData_->shininess = 70;
		materialData_->environmentCoefficient = 0.0f;

		materialResources_.push_back(materialResource);
	}

	//テクスチャ読み込み
	for (auto& material : modelData_.material) {
		TextureManager::GetInstance()->LoadTexture(material.textureFilePath);
		material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(material.textureFilePath);
	}

	if (isSkinning) {
		for (auto& child : modelData_.rootNode.children) {
			Skeleton skeleton;
			skeleton = CreateSkeltion(child);
			skeletons_.push_back(skeleton);
		}


		SkinCluster skinCluster;
		skinCluster = CreateSkinCluster(skeletons_[1], modelData_);
		skinClusters_.push_back(skinCluster);
	}

	isAnimation_ = isAnimation;
	isSkinning_ = isSkinning;

	EnvironmentFile_ ="resource/rostock_laage_airport_4k.dds";

}

void Model_glTF::Draw() {
	//objファイルに元々あったテクスチャ
	modelData_ = InitialData_;
	vbvs_[0] = vertexBufferView_[multiMeshCount_];

	if (isSkinning_) {
		vbvs_[1] = skinClusters_[multiMeshCount_].influenceBufferView;
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs_);
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(8, skinClusters_[multiMeshCount_].paletteSrvHandle.second);//Skinning.VS t0
	}
	else {
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, vbvs_);
	}
	modelCommon_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView_[multiMeshCount_]);
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[multiMeshCount_]->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material[multiMeshCount_].textureFilePath));
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(EnvironmentFile_));
	modelCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices[multiMeshCount_].size()), 1, 0, 0, 0);
	multiMeshCount_++;
}


ModelData_glTF Model_glTF::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData_glTF modelData;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	const aiScene* kScene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(kScene->HasMeshes()); //メッシュがないのは対応なし

	std::vector<VertexData> vertices;

	//VertexDataを読み取る
	for (uint32_t meshIndex = 0; meshIndex < kScene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = kScene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線があるか

		vertices.resize(mesh->mNumVertices);//頂点数分のメモリ確保

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			vertices[vertexIndex].normal = { -normal.x,normal.y, normal.z, };

			//Texcordがあるか
			if (mesh->HasTextureCoords(0)) {
				vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			}
			else {
				//BaseColorは設定されていない
				vertices[vertexIndex].texcoord = { 1,1 };
			}
		}

		modelData.vertices.push_back(vertices);


		std::vector<uint32_t> indices;

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみ

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				indices.push_back(vertexIndex);
			}
		}

		modelData.indices.push_back(indices);

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

	//MaterialData
	for (uint32_t materialIndex = 0; materialIndex < kScene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = kScene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			MaterialData materialData;
			materialData.textureFilePath = directoryPath + "/Sprite/" + textureFilePath.C_Str();
			materialData.materialColor = { 1.0f,1.0f,1.0f,1.0f };
			modelData.material.push_back(materialData);
		}
		else {
			aiColor4D color;
			material->Get(AI_MATKEY_BASE_COLOR, color);
			//Blender初期のベースカラー
			MaterialData materialData;
			materialData.textureFilePath = directoryPath + "/Sprite/white.png";
			materialData.materialColor = { (float)color.r,(float)color.g,(float)color.b,(float)color.a };
			modelData.material.push_back(materialData);
		}

	}

	modelData.rootNode = ReadNode(kScene->mRootNode);

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


std::vector<Animation>  Model_glTF::LoadAnimationFile(const std::string& directoryPath, const std::string& filename, uint32_t Number) {
	Animation animation;
	std::vector<Animation> animations_;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがないとき

	for (uint32_t i = 0; i < Number; i++) {
		aiAnimation* animationAssimp = scene->mAnimations[i];//アニメーション数
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

		animations_.push_back(animation);
	}

	return animations_;
}

SkinCluster Model_glTF::CreateSkinCluster(const Skeleton& skeleton,const ModelData_glTF& modelData) {


	SkinCluster skinCluster;
	uint32_t IndexNum = 0;

	///t0
	// paletteResource確保
	skinCluster.paletteResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
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
	modelCommon_->GetDxCommon()->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);
	///

	///WEIGHT INDEXのやつ
	//influenceResource確保

	uint32_t all_vertex = 0;
	for (auto& v : modelData.vertices) {
		all_vertex += uint32_t(v.size());
	}

	skinCluster.influenceResource  = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexInfluence) * all_vertex);
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * all_vertex);
	skinCluster.mappedInfluence = { mappedInfluence,all_vertex };

	//InfluenceのVBV
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * all_vertex);
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
void Model_glTF::SetEnvironment(const std::string& mapFile) {
	EnvironmentFile_ = mapFile;
}