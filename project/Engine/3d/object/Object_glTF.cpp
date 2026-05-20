/// -------------------------------
///
/// .gltfファイル型のオブジェクト
/// 
/// -------------------------------
#include "Object_glTF.h"
#include "GLTFCommon.h"
#include "TextureManager.h"

#include <fstream>
#include <sstream>
#include "ModelManager.h"
#include <numbers>
#include "Logger.h"

#include "ImGuiManager.h"
#include <Object3dCommon.h>

using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	Object_glTF::Object_glTF() {}

	Object_glTF::~Object_glTF() {
		for (auto it : debugSpheres_) {
			delete it;
		}
		debugSpheres_.clear();
	}

	void Object_glTF::Initialize() {
		camera_ = Object3dCommon::GetInstance().GetDefaultCamera();

		//カメラ初期化
		InitCamera();

		//ライト初期化処理まとめ
		InitLight();
	}

	void Object_glTF::Update(WorldTransform& worldTransform) {

		worldMatrix_ = worldTransform.GetMatWorld();

		//アニメーション更新
		AnimationUpdate();

		for (uint32_t i = 0; i < modelData_.Data.size(); i++) {
			if (model_->IsAnimation()) {
				wvpDatas_[i]->World = modelData_.rootNode.children[i].localMatrix * worldTransform.GetMatWorld();
			}
			else {
				wvpDatas_[i]->World = modelData_.rootNode.localMatrix * worldTransform.GetMatWorld();
			}
		}

		directionalLightData_->direction = Normalize(directionalLightData_->direction);
	}

	void Object_glTF::Update() {

		worldMatrix_ = MakeIdentity4x4();

		//アニメーション更新
		AnimationUpdate();

		for (uint32_t i = 0; i < modelData_.Data.size(); i++) {
			if (model_->IsAnimation()) {
				wvpDatas_[i]->World = localMatrices_[i] * worldMatrix_;
			}
			else {
				wvpDatas_[i]->World = modelData_.rootNode.localMatrix * worldMatrix_;
			}
		}

		directionalLightData_->direction = Normalize(directionalLightData_->direction);
	}

	void Object_glTF::AnimationUpdate() {
		//作るときはフレームレートを60FPSにする
		uint32_t i = 0;
		animationTime_ += animationTimeScale_;

		for (auto& animation_ : animations_) {
			//アニメーションが最大時間を超えたら巻き戻す
			animationTime_ = std::fmod(animationTime_, animation_.duration);
			//スキニング処理
			if (model_->IsSkinning()) {
				if (isChange_) {
					changeTime_ += animationTimeScale_;
					if (changeTime_ >= preAnimations_[i].duration) {
						isChange_ = false;
						changeTime_ = 0;
					}
					else {
						Interpolation(skeletons_[i], preAnimations_[i], animation_, changeTime_);
					}
				}
				else {
					ApplyAnimation(skeletons_[1], animation_, animationTime_);
				}
			}
		}
		//スキニング
		if (model_->IsSkinning()) {
			for (auto& skeleton : skeletons_) {
				SkeletonUpdate(skeleton, worldMatrix_ * MakeTranslateMatrix(Vector3(0, 0, -0.2f)));
				SkinClusterUpdate(skinClusters_[i], skeleton);
			}
		}
		//フレームアニメーション
		FrameAnimation();
	}

	void Object_glTF::FrameAnimation() {
		//スキニングではないアニメーションの場合
		if (!model_->IsSkinning() && model_->IsAnimation()) {
			//一度リセット
			localMatrices_.clear();

			for (uint32_t i = 0; i < modelData_.Data.size(); i++) {
				Matrix4x4 localMatrix;

				if (modelData_.rootNode.children.size() <= 1) {
					NodeAnimation& rootNodeAnimation = animations_[i].nodeAnimations[modelData_.rootNode.name];
					Vector3 translate = CalculateValue(rootNodeAnimation.translate, animationTime_);//nextと逆にする()
					Quaternion rotate = CalculateValueQuaternion(rootNodeAnimation.rotate, animationTime_);
					Vector3 scale = CalculateValue(rootNodeAnimation.scale, animationTime_);

					localMatrix = MakeAffineMatrix(scale, rotate, translate);
					localMatrices_.push_back(localMatrix);
				}
				else if (model_->IsAnimation()) {
					NodeAnimation& rootNodeAnimation = animations_[i].nodeAnimations[modelData_.rootNode.children[i].name];
					Vector3 translate = CalculateValue(rootNodeAnimation.translate, animationTime_);//nextと逆にする()
					Quaternion rotate = CalculateValueQuaternion(rootNodeAnimation.rotate, animationTime_);
					Vector3 scale = CalculateValue(rootNodeAnimation.scale, animationTime_);

					localMatrix = MakeAffineMatrix(scale, rotate, translate);
					localMatrices_.push_back(localMatrix);
				}
			}
		}
	}

	void Object_glTF::CameraUpdate() {
		if (camera_) {
			Matrix4x4 ProjectionMatrix = camera_->GetViewProjectionMatrix();
			WorldViewProjectionMatrix = worldMatrix_ * modelData_.rootNode.localMatrix * ProjectionMatrix;
			//通常のアニメーション
			if (!model_->IsSkinning() && model_->IsAnimation()) {
				WorldViewProjectionMatrix = worldMatrix_ * ProjectionMatrix;
			}
		}
		else {
			WorldViewProjectionMatrix = worldMatrix_;
		}
	}

	void Object_glTF::Draw() {
		//カメラ更新
		CameraUpdate();

		for (uint32_t i = 0; i < modelData_.Data.size(); i++) {
			if (model_->IsSkinning()) {
				//スキニングのアニメーションの場合
				wvpDatas_[i]->WVP = WorldViewProjectionMatrix;
			}
			else if (model_->IsAnimation()) {
				//フレームアニメーションの場合
				if (localMatrices_.empty()) return;
				wvpDatas_[i]->WVP = localMatrices_[i] * WorldViewProjectionMatrix;
			}
			else {
				//動かない場合
				wvpDatas_[i]->WVP = WorldViewProjectionMatrix;
			}
		}

		DrawCommand();
		//モデル
		for (uint32_t i = 0; i < modelData_.Data.size(); i++) {
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[i]->GetGPUVirtualAddress());
			if (model_) {
				model_->Draw();
			}
		}
		//マルチメッシュで使用するカウントをリセット
		model_->ResetMeshCount();

#ifdef _DEBUG
		DebugWireframes::GetInstance().Command();

		for (auto it : debugSpheres_) {
			//it->Draw();
		}

		GLTFCommon::GetInstance().Command();
#endif // _DEBUG
	}

	void Object_glTF::Draw(const std::string& textureData) {
		//モデル 

	}

	void Object_glTF::CreateWVP() {
		for (auto& i : modelData_.Data) {
			Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
			TransformationMatrix* wvpData;
			wvpResource = DirectXCommon::GetInstance().CreateBufferResource(sizeof(TransformationMatrix));
			wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));


			wvpData->World = MakeIdentity4x4();
			wvpData->WVP = MakeIdentity4x4();

			wvpResources_.push_back(wvpResource);
			wvpDatas_.push_back(wvpData);
		}
	}


	void Object_glTF::SetModelFile(const std::string& filePath) {

		model_ = ModelManager::GetInstance().FindModel_gltf(filePath);
		modelData_ = model_->GetModelData();

		//wvpを作成
		CreateWVP();

		if (model_->IsAnimation()) {
			animations_ = model_->GetAnimationData();
			if (model_->IsSkinning()) {
				skeletons_ = model_->GetSkeleton();
				skinClusters_ = model_->GetSkinCluster();

				for (auto& skeleton : skeletons_) {
					SkeletonUpdate(skeleton);
				}

#ifdef _DEBUG
				for (uint32_t i = 0; i < skeletons_.size(); i++) {
					for (uint32_t childIndex = 0; childIndex < skeletons_[i].joints.size(); ++childIndex) {
						//デバッグワイヤーフレーム
						SetWireframe();
					}
				}
#endif // _DEBUG	

				int i = 0;
				for (auto& skinCluster : skinClusters_) {
					SkinClusterUpdate(skinCluster, skeletons_[i]);
					i++;
				}
			}
		}

		//初期環境マップ
		TextureManager::GetInstance().LoadTexture("resource/rostock_laage_airport_4k.dds");
		model_->SetEnvironment("resource/rostock_laage_airport_4k.dds");
	}


	void Object_glTF::LightSwitch(bool isLight) {
		if (model_) {
			//ライト更新
			model_->LightOn(isLight);
		}
	}

	void Object_glTF::SetColor(const Vector4& color) {
		if (model_) {
			//色変更
			model_->SetColor(color);
		}
	}

	//環境マップのファイルパス
	void Object_glTF::SetEnvironment(const std::string& filePath) {
		if (model_) {
			//環境マップのテクスチャパス変更
			model_->SetEnvironment(filePath);
		}
	}

	void Object_glTF::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
		for (Joint& joint : skeleton.joints) {
			//jointにアニメーションがある場合
			if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
				const NodeAnimation& kRootNodeAnimation = (*it).second;
				joint.transform.translate = CalculateValue(kRootNodeAnimation.translate, animationTime);
				joint.transform.rotate = CalculateValueQuaternion(kRootNodeAnimation.rotate, animationTime);
				joint.transform.scale = CalculateValue(kRootNodeAnimation.scale, animationTime);

			}
		}
	}


	void Object_glTF::SkeletonUpdate(Skeleton& skeleton) {
		for (Joint& joint : skeleton.joints) {
			joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
			if (joint.parent) {
				joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;//Jointに親がいるとき(子)
			}
			else {
				joint.skeletonSpaceMatrix = joint.localMatrix;//jointに親がいない場合(親)
			}
		}
	}

	void Object_glTF::SkeletonUpdate(Skeleton& skeleton, const Matrix4x4& matWorld) {
		int i = 0;//一から順番に
		for (Joint& joint : skeleton.joints) {
			joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
#ifdef _DEBUG
			debugSpheres_[i]->SetColor(Vector4(1, 1, 0, 1));//わかりやすい色
#endif // _DEBUG
			if (joint.parent) {
				joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;//Jointに親がいるとき(子)
#ifdef _DEBUG
				debugSpheres_[i]->Update(joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix * matWorld);
#endif // _DEBUG
			}
			else {
				joint.skeletonSpaceMatrix = joint.localMatrix;//jointに親がいない場合(親)
#ifdef _DEBUG	
				debugSpheres_[i]->Update(joint.localMatrix * matWorld);
#endif // _DEBUG
			}
			i++;
		}
	}

	void Object_glTF::SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton) {
		for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
			assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
			skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
				skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
			skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
				Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
		}
	}

	void Object_glTF::SetWireframe() {
#ifdef _DEBUG
		std::unique_ptr<SphereModel> sphere = std::make_unique<SphereModel>();
		sphere->Initialize();

		debugSpheres_.push_back(std::move(sphere.get()));
#endif // _DEBUG
	}

	void Object_glTF::ChangeAnimation(const std::string& filePath) {

		//モデルが同じならすぐにリターン
		if (model_ == ModelManager::GetInstance().FindModel_gltf(filePath)) {
			return;
		}

		//変更前のアニメーションデータ
		preAnimations_ = animations_;

		//変更先のアニメーションデータ
		model_ = ModelManager::GetInstance().FindModel_gltf(filePath);
		modelData_ = model_->GetModelData();
		animations_ = model_->GetAnimationData();
		skeletons_ = model_->GetSkeleton();
		skinClusters_ = model_->GetSkinCluster();

		//WVPの再構築
		CreateWVP();

		if (model_->IsSkinning()) {
			//animationTimeを1.0f/60.0fに
			//SLeapなどで0より小さい値を出さないようにする
			//はじめは少しカクつくが、アニメーション補間が終えた後がスムーズ
			changeTime_ += 1.0f / 60.0f;
			animationTime_ = changeTime_;
			uint32_t i = 0;
			for (auto& skeleton : skeletons_) {
				Interpolation(skeleton, animations_[i], preAnimations_[i], changeTime_);
				SkeletonUpdate(skeleton);
				for (auto& skinCluster : skinClusters_) {
					SkinClusterUpdate(skinCluster, skeleton);
				}
			}
			//アニメーション補間中に変更があった時
			if (isChange_) {
				changeTime_ = 1.0f - changeTime_;
			}

			isChange_ = true;

			//SLeapなどで1より大きい値を出さないようにする
			for (auto& preAnimation : preAnimations_) {
				preAnimation.duration = max(preAnimation.duration, 1.0f);
			}
		}
		else {
			//アニメーションを初めから
			animationTime_ = 0.0f;
		}
		//初期環境マップ
		TextureManager::GetInstance().LoadTexture("resource/rostock_laage_airport_4k.dds");
		model_->SetEnvironment("resource/rostock_laage_airport_4k.dds");
	}

	void Object_glTF::Interpolation(Skeleton& skeleton, const Animation& animation, const Animation& nextAnimation, float animationTime) {
		for (Joint& joint : skeleton.joints) {
			//jointにアニメーションがある場合
			if (auto itA = animation.nodeAnimations.find(joint.name); itA != animation.nodeAnimations.end()) {
				if (auto itB = nextAnimation.nodeAnimations.find(joint.name); itB != nextAnimation.nodeAnimations.end()) {
					const NodeAnimation& kRootNodeAnimation = (*itA).second;
					const NodeAnimation& kNextRootNodeAnimation = (*itB).second;
					joint.transform.translate = InterpolationValue(kNextRootNodeAnimation.translate, kRootNodeAnimation.translate, animationTime);//nextと逆にする()
					joint.transform.rotate = InterpolationValueQuaternion(kRootNodeAnimation.rotate, kNextRootNodeAnimation.rotate, animationTime);
					joint.transform.scale = InterpolationValue(kRootNodeAnimation.scale, kNextRootNodeAnimation.scale, animationTime);

				}
			}
		}
	}
}