#include "Object_glTF.h"
#include "GLTFCommon.h"
#include "externals/imgui/imgui.h"
#include "TextureManager.h"

#include <fstream>
#include <sstream>
#include "ModelManager.h"
#include <numbers>
#include "Logger.h"

using namespace MyMath;

Object_glTF::Object_glTF(){}

Object_glTF::~Object_glTF(){
	for (auto it : debugSphere) {
		delete it;
	}
	debugSphere.clear();
}

void Object_glTF::Initialize() {
	this->object3dCommon = GLTFCommon::GetInstance();
	this->camera = object3dCommon->GetDefaultCamera();
	wvpResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));


	wvpData->World = MakeIdentity4x4();
	wvpData->WVP = MakeIdentity4x4();

	//ライト用のリソース
	directionalLightSphereResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightSphereResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData));
	//色の設定
	directionalLightSphereData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData->intensity = 0.5f;//明るすぎたため


	//Phong Reflection Model
	cameraResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	cameraData->worldPosition = { 0,0,0 };

	//ライト用のリソース
	pointLightResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(PointLight));
	//書き込むためのアドレス
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	//設定
	pointLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData->position = { 0.0f,2.0f,0.0f };
	pointLightData->intensity = 0.0f;
	pointLightData->radius = 5.0f;
	pointLightData->decay = 1.0f;


	//ライト用のリソース
	spotLightResource = object3dCommon->GetDirectXCommon()->CreateBufferResource(sizeof(SpotLight));
	//書き込むためのアドレス
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	//設定
	spotLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData->position = { 2.0f,1.25f,0.0f };
	spotLightData->distance = 70.0f;
	spotLightData->direction = Normalize({ -1.0f,-1.0f,0.0f });
	spotLightData->intensity = 0.0f;
	spotLightData->decay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);






	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	transformL = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };




}

void Object_glTF::Update(const WorldTransform& worldTransform) {

	//作るときはフレームレートを60FPSにする
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animation.duration);
	
	if (isChange) {
		changeTime += 1.0f / 60.0f;
		if (changeTime >= preAnimation.duration) {
			isChange = false;
			changeTime = 0;
		}
		else {	
			Interpolation(skeleton, preAnimation, animation, changeTime);
		}
	}
	else {
		ApplyAnimation(skeleton, animation, animationTime);
	}

	SkeletonUpdate(skeleton,worldTransform.matWorld_ * MakeTranslateMatrix(Vector3(0,0,-0.2f)));
	SkinClusterUpdate(skinCluster, skeleton);



	Matrix4x4 skaletonSpaceMatrix;
	Matrix4x4 WorldViewProjectionMatrix;
	if (camera) {
		Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = worldTransform.matWorld_ * projectionMatrix;
	}
	else {
		WorldViewProjectionMatrix = worldTransform.matWorld_;
	}
	Matrix4x4 JointWorldMatrix = skaletonSpaceMatrix * worldTransform.matWorld_;

	wvpData->World = JointWorldMatrix * worldTransform.matWorld_;
	wvpData->World = modelData.rootNode.localMatrix * worldTransform.matWorld_;
	//wvpData->World = worldMatrix;
	wvpData->WVP = WorldViewProjectionMatrix;

	directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);
}


void Object_glTF::Draw() {
	//モデル
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());
	if (model) {
		model->Draw();
	}

#ifdef _DEBUG
	DebugWireframes::GetInstance()->Command();

	for (auto it : debugSphere) {
		it->Draw();
	}

	GLTFCommon::GetInstance()->Command();
#endif // _DEBUG

}

void Object_glTF::Draw(const std::string& textureData) {
	//モデル
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	object3dCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());
	if (model) {
		model->Draw(textureData);
	}

#ifdef _DEBUG
	DebugWireframes::GetInstance()->Command();

	for (auto it : debugSphere) {
		it->Draw();
	}

	GLTFCommon::GetInstance()->Command();
#endif // _DEBUG

}

void Object_glTF::SetModelFile(const std::string& filePath) {

	model = ModelManager::GetInstance()->FindModel_gltf(filePath);
	material = model->GetMaterial();
	modelData = model->GetModelData();
	animation = model->GetAnimationData();
	skeleton = model->GetSkeleton();
	skinCluster = model->GetSkinCluster();

	//デバッグワイヤーフレーム
	//親ノード
	//SetWireframe();
	//子ノード
#ifdef _DEBUG
	for (uint32_t childIndex = 0; childIndex < skeleton.joints.size(); ++childIndex) {
		SetWireframe();
	}

#endif // _DEBUG
	SkeletonUpdate(skeleton);
	SkinClusterUpdate(skinCluster,skeleton);

}

void Object_glTF::LightSwitch(bool isLight) {
	if (model) {
		model->LightOn(isLight);
	}
}

//環境マップのファイルパス
void Object_glTF::SetEnvironment(const std::string& filePath) {
	if (model) {
		model->SetEnvironment(filePath);
	}
}

void Object_glTF::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		//jointにアニメーションがある場合
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime);
			joint.transform.rotate = CalculateValueQuaternion(rootNodeAnimation.rotate, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime);

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
		debugSphere[i]->SetColor(Vector4(1, 1, 0, 1));//わかりやすい色
#endif // _DEBUG
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;//Jointに親がいるとき(子)
#ifdef _DEBUG
			debugSphere[i]->Update(joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix * matWorld);
#endif // _DEBUG
		}
		else {
			joint.skeletonSpaceMatrix = joint.localMatrix;//jointに親がいない場合(親)
 #ifdef _DEBUG	
			debugSphere[i]->Update(joint.localMatrix * matWorld);
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
	SphereModel* sphere = new SphereModel();
	sphere->Initialize();

	debugSphere.push_back(sphere);
#endif // _DEBUG
}

void Object_glTF::ChangeAnimation(const std::string& filePath) {
	
	//モデルが同じならすぐにリターン
	if (model == ModelManager::GetInstance()->FindModel_gltf(filePath)) {
		return;
	}
	
	//変更前のアニメーションデータ
	preAnimation = animation;

	//変更先のアニメーションデータ
	model = ModelManager::GetInstance()->FindModel_gltf(filePath);
	modelData = model->GetModelData();
	animation = model->GetAnimationData();
	skeleton = model->GetSkeleton();
	skinCluster = model->GetSkinCluster();

	//animationTimeを1.0f/60.0fに
	//Sleapなどで0より小さい値を出さないようにする
	//はじめは少しカクつくが、アニメーション補間が終えた後がスムーズ
	changeTime += 1.0f / 60.0f;
	animationTime = changeTime;

	Interpolation(skeleton, animation, preAnimation,changeTime);
	SkeletonUpdate(skeleton);
	SkinClusterUpdate(skinCluster, skeleton);
	
	//アニメーション補間中に変更があった時
	if (isChange) {
		changeTime = 0.9f - changeTime;
	}
	
	isChange = true;

	//Sleapなどで1より大きい値を出さないようにする
	if (preAnimation.duration > 1.0f) {
		preAnimation.duration = 0.9f;
	}

}

void Object_glTF::Interpolation(Skeleton& skeleton, const Animation& animation, const Animation& nextAnimation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		//jointにアニメーションがある場合
		if (auto itA = animation.nodeAnimations.find(joint.name); itA != animation.nodeAnimations.end()) {
			if (auto itB = nextAnimation.nodeAnimations.find(joint.name); itB != nextAnimation.nodeAnimations.end()) {
				const NodeAnimation& rootNodeAnimation = (*itA).second;
				const NodeAnimation& nextRootNodeAnimation = (*itB).second;
				joint.transform.translate = InterpolationValue(nextRootNodeAnimation.translate,rootNodeAnimation.translate, animationTime);//nextと逆にする()
				joint.transform.rotate = InterpolationValueQuaternion(rootNodeAnimation.rotate, nextRootNodeAnimation.rotate, animationTime);
				joint.transform.scale = InterpolationValue(rootNodeAnimation.scale, nextRootNodeAnimation.scale, animationTime);

			}
		}
	}
}