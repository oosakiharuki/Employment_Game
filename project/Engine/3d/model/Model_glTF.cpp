#include "Model_glTF.h"
/// -----------------------------------------------------------
///
/// .gltfファイル型の3Dモデル(アニメーション、スキニングあり)
/// 
/// -----------------------------------------------------------
#include "TextureManager.h"
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Model_glTF::Initialize(const std::string& directoryPath, const std::string& fileName) {
		//.gltf
		modelData_ = LoadModelFile(directoryPath, fileName);

		InitialData_ = modelData_;

		//vertex
		for (auto& modelData : modelData_.Data) {

			InitVertexResource(modelData);

			//index
			InitIndexResource(modelData);

			//Model用マテリアル
			InitMaterialResource(modelData);
		}

		//環境マップ
		EnvironmentFile_ = "resource/rostock_laage_airport_4k.dds";
	}

	void Model_glTF::InitAnimation(const std::string& directoryPath, const std::string& fileName, bool isAnimation, bool isSkinning) {
		isAnimation_ = isAnimation;
		isSkinning_ = isSkinning;

		if (isAnimation_) {
			animation_ = LoadAnimationFile(directoryPath, fileName, uint32_t(modelData_.Data.size()));
		}

		if (isSkinning_) {
			for (auto& child : modelData_.rootNode.children) {
				Skeleton skeleton;
				skeleton = CreateSkelton(child);
				skeletons_.push_back(skeleton);
			}

			SkinCluster skinCluster;
			skinCluster = CreateSkinCluster(skeletons_[1], modelData_);
			skinClusters_.push_back(skinCluster);
		}
	}

	void Model_glTF::InitVertexResource(ModelData modelData) {
		D3D12_VERTEX_BUFFER_VIEW vertexB{};
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexR;

		vertexR = DirectXCommon::GetInstance().CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

		vertexB.BufferLocation = vertexR->GetGPUVirtualAddress();
		vertexB.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
		vertexB.StrideInBytes = sizeof(VertexData);

		vertexR->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
		std::memcpy(vertexData_, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

		vertexResource_.push_back(vertexR);
		vertexBufferView_.push_back(vertexB);
	}

	void Model_glTF::InitMaterialResource(ModelData modelData) {

		//マテリアル用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		materialResource = DirectXCommon::GetInstance().CreateBufferResource(sizeof(Material));
		//書き込むためのアドレス
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
		//色の設定
		if (modelData.materialData.materialColor.s) {
			//baseColor設定
			materialData_->color = modelData.materialData.materialColor;
		}
		else {
			materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		materialData_->enableLighting = true;
		materialData_->uvTransform = modelData.materialData.uvTransform;

		materialData_->shininess = 70;
		materialData_->environmentCoefficient = 0.0f;

		materialResources_.push_back(materialResource);

		//テクスチャ読み込み
		TextureManager::GetInstance().LoadTexture(modelData.materialData.textureFilePath);
		modelData.materialData.textureIndex = TextureManager::GetInstance().GetSrvIndex(modelData.materialData.textureFilePath);
	}

	void Model_glTF::InitIndexResource(ModelData modelData) {

		Microsoft::WRL::ComPtr<ID3D12Resource> indexR;
		D3D12_INDEX_BUFFER_VIEW indexB{};

		indexR = DirectXCommon::GetInstance().CreateBufferResource(sizeof(uint32_t) * modelData.indices.size());

		indexB.BufferLocation = indexR->GetGPUVirtualAddress();
		indexB.SizeInBytes = UINT(sizeof(uint32_t) * modelData.indices.size());
		indexB.Format = DXGI_FORMAT_R32_UINT;

		indexR->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
		std::memcpy(mappedIndex_, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());

		indexResource_.push_back(indexR);
		indexBufferView_.push_back(indexB);
	}


	void Model_glTF::Draw() {
		//objファイルに元々あったテクスチャ
		//modelData_ = InitialData_;
		vbvs_[0] = vertexBufferView_[multiMeshCount_];


		if (isSkinning_) {
			vbvs_[1] = skinClusters_[multiMeshCount_].influenceBufferView;
			DirectXCommon::GetInstance().GetCommandList()->IASetVertexBuffers(0, 2, vbvs_);
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(9, skinClusters_[multiMeshCount_].paletteSrvHandle.second);//Skinning.VS t0
		}
		else {
			DirectXCommon::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, vbvs_);
		}
		DirectXCommon::GetInstance().GetCommandList()->IASetIndexBuffer(&indexBufferView_[multiMeshCount_]);
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[multiMeshCount_]->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance().GetSrvHandleGPU(modelData_.Data[multiMeshCount_].materialData.textureFilePath));
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(8, TextureManager::GetInstance().GetSrvHandleGPU(EnvironmentFile_));
		DirectXCommon::GetInstance().GetCommandList()->DrawIndexedInstanced(UINT(modelData_.Data[multiMeshCount_].indices.size()), 1, 0, 0, 0);
		multiMeshCount_++;

	}


	ModelDataMulti Model_glTF::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
		ModelDataMulti modelData;

		ModelData iModelData;

		Assimp::Importer importer;
		std::string filePath = directoryPath + "/Object/" + filename;

		const aiScene* kScene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
		assert(kScene->HasMeshes()); //メッシュがないのは対応なし

		//使用するマテリアル番号
		uint32_t materialNum = 0;
		std::vector<MaterialData> materialDatas;

		//MaterialData
		for (uint32_t materialIndex = 0; materialIndex < kScene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = kScene->mMaterials[materialIndex];
			MaterialData materialData;

			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

				//テクスチャUVのトランスフォーム
				aiUVTransform uvTransform;
				materialData.uvTransform = MakeIdentity4x4();//通常行列

				//マッピングがあるとき
				if (material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), uvTransform) == AI_SUCCESS) {
					//座標位置の変更
					Vector3 uvOffset = { uvTransform.mTranslation.x ,uvTransform.mTranslation.y ,0.0f };//位置を設定
					materialData.uvTransform = MakeTranslateMatrix(uvOffset);//行列にして移行
					//通常行列にスケールをかける
					Vector3 uvScale = { uvTransform.mScaling.x,uvTransform.mScaling.y,1.0f };//大きさを設定
					materialData.uvTransform = materialData.uvTransform * MakeScaleMatrix(uvScale);//行列にしてかける
				}

				size_t pos1;
				std::string texture = textureFilePath.C_Str();//stringに変更

				std::string extension;

				//最後の'.'を読み込む  ○○'.'png
				pos1 = texture.rfind('.');

				//拡張子がない
				if (pos1 != std::wstring::npos) {
					extension = texture.substr(0, pos1);//.pngを抜いた文字列
				}


				materialData.textureFilePath = directoryPath + "/Sprite/" + extension + ".dds";
				materialData.materialColor = { 1.0f,1.0f,1.0f,1.0f };
			}
			else {
				aiColor4D color;
				material->Get(AI_MATKEY_BASE_COLOR, color);
				//Blender初期のベースカラー
				materialData.textureFilePath = directoryPath + "/Sprite/white.dds";
				materialData.materialColor = { (float)color.r,(float)color.g,(float)color.b,(float)color.a };
			}
			//マテリアルデータを導入
			materialDatas.push_back(materialData);
		}



		//VertexDataを読み取る
		for (uint32_t meshIndex = 0; meshIndex < kScene->mNumMeshes; ++meshIndex) {
			aiMesh* mesh = kScene->mMeshes[meshIndex];
			assert(mesh->HasNormals());//法線があるか

			//頂点データ
			std::vector<VertexData> vertices;

			vertices.resize(mesh->mNumVertices);//頂点数分のメモリ確保

			for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

				vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
				vertices[vertexIndex].normal = { -normal.x,normal.y, normal.z, };

				//Texcoordがあるか
				if (mesh->HasTextureCoords(0)) {
					vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
				}
				else {
					//BaseColorは設定されていない
					vertices[vertexIndex].texcoord = { 1,1 };
				}
			}

			//頂点データの塊を導入
			iModelData.vertices = vertices;


			std::vector<uint32_t> indices;

			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
				aiFace& face = mesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);//三角形のみ

				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					uint32_t vertexIndex = face.mIndices[element];
					indices.push_back(vertexIndex);
				}
			}

			//頂点インデックスの塊を導入
			iModelData.indices = indices;

			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

				//jointごとの格納領域作成
				aiBone* bone = mesh->mBones[boneIndex]; // Bone(骨) = joint
				std::string jointName = bone->mName.C_Str();
				JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

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

			//使用するマテリアルを導入
			iModelData.materialData = materialDatas[mesh->mMaterialIndex];

			modelData.Data.push_back(iModelData);
			materialNum++;
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
		std::string filePath = directoryPath + "/Object/" + filename;
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

	SkinCluster Model_glTF::CreateSkinCluster(const Skeleton& skeleton, const ModelDataMulti& modelData) {


		SkinCluster skinCluster;
		uint32_t IndexNum = 0;

		///t0
		// paletteResource確保
		skinCluster.paletteResource = DirectXCommon::GetInstance().CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
		WellForGPU* mappedPalette = nullptr;
		skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
		skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//sponのサイズ設定

		IndexNum = SrvManager::GetInstance().Allocate();
		skinCluster.paletteSrvHandle.first = SrvManager::GetInstance().GetCPUDescriptorHandle(IndexNum);
		skinCluster.paletteSrvHandle.second = SrvManager::GetInstance().GetGPUDescriptorHandle(IndexNum);

		//palette用のsrvを作成 StructuredBufferでアクセスできるようにする
		D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
		paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		paletteSrvDesc.Buffer.FirstElement = 0;
		paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
		paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
		DirectXCommon::GetInstance().GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);
		///

		///WEIGHT INDEXのやつ
		//influenceResource確保

		uint32_t all_vertex = 0;
		for (auto& v : modelData.Data) {
			all_vertex += uint32_t(v.vertices.size());
		}

		skinCluster.influenceResource = DirectXCommon::GetInstance().CreateBufferResource(sizeof(VertexInfluence) * all_vertex);
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
		std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);
		///

		for (const auto& jointWeight : modelData.skinClusterData) {
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


	Skeleton Model_glTF::CreateSkelton(const Node& rootNode) {
		Skeleton skeleton;
		skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

		for (const Joint& joint : skeleton.joints) {
			skeleton.jointMap.emplace(joint.name, joint.index);
		}
		return skeleton;
	}

	int32_t Model_glTF::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
		Joint joint;
		joint.name = node.name;
		joint.localMatrix = node.localMatrix;
		joint.skeletonSpaceMatrix = MakeIdentity4x4();
		joint.transform = node.transform;
		joint.index = int32_t(joints.size());
		joint.parent = parent;
		joints.push_back(joint);

		//再帰関数
		for (const Node& child : node.children) {
			int32_t childIndex = CreateJoint(child, joint.index, joints);
			joints[joint.index].children.push_back(childIndex);
		}

		return joint.index;
	}
}