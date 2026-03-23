#include "ParticleManager.h"
#include "ModelManager.h"
#include <cassert>
#include <numbers>

#include "Camera.h"

#include <json.hpp>
#include <fstream>


using namespace MyMath;
using namespace Primitive;

std::unique_ptr<ParticleManager> ParticleManager::sInstance_ = nullptr;

ParticleManager& ParticleManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ParticleManager>();
	}
	return *sInstance_;
}

void ParticleManager::Finalize() {
	sInstance_.reset();
}


void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilePath, const ModelData& modelData) {

	assert(SrvManager::GetInstance().Max());
	
	if (particleGroups.contains(name)) {
		return;
	}
	ParticleGroup& particleG = particleGroups[name];


	particleG.textureFile = textureFilePath;

	particleG.modelData = modelData;

	//テクスチャ読み込み
	particleG.modelData.materialData.textureFilePath = textureFilePath;
	particleG.resource = DirectXCommon::GetInstance().CreateBufferResource(sizeof(ParticleForGPU) * particleG.kNumInstance);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = particleG.kNumInstance;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	particleG.srvIndex = SrvManager::GetInstance().Allocate();
	particleG.srvHandleCPU = SrvManager::GetInstance().GetCPUDescriptorHandle(particleG.srvIndex);
	particleG.srvHandleGPU = SrvManager::GetInstance().GetGPUDescriptorHandle(particleG.srvIndex);
	

	//SRVの生成
	DirectXCommon::GetInstance().GetDevice()->CreateShaderResourceView(particleG.resource.Get(), &srvDesc, particleG.srvHandleCPU);
	
	particleG.numInstance = 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE  ParticleManager::GetSrvHandleGPU( const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.srvHandleGPU;
}

ModelData ParticleManager::GetModelData( const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.modelData;
}

std::string ParticleManager::GetTextureFile( const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.textureFile;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ParticleManager::GetResource( const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.resource;
}

Particle* ParticleManager::GetParticle(const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	Particle* particleG = particles_[filePath].get();
	return particleG;
}

ParticleParameters ParticleManager::GetParticleParameter(const std::string& filePath) {
	return particleParameters_[filePath];
}

void ParticleManager::Update( const std::string& filePath, ParticleForGPU* wvpData) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];

	//複数ある場合、出ていない部分は描画されないように
	particleG.numInstance = 0;

	//filePath名ですでに更新している場合
	if (particleG.Updated) { return; }
	particleG.Updated = true;

	for (std::list<ParticleData>::iterator particleIterator = particleG.particles.begin(); particleIterator != particleG.particles.end(); ) {
		//生存時間を過ぎた or スケール(x,y,z)のいずれかが0以下の場合
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime || ((*particleIterator).transform.scale.x < 0 || (*particleIterator).transform.scale.y < 0 || (*particleIterator).transform.scale.z < 0)) {
			particleIterator = particleG.particles.erase(particleIterator);
			continue;
		}
		//時間
		Timer(*particleIterator);
		//移動
		VelocityMove(*particleIterator);

		if (filePath == "clear_fanfare") {
			(*particleIterator).velocityTransform.translate.y -= 0.1f;
		}

		Matrix4x4 worldMatrix = CreateWorldMatrix(*particleIterator);

		// wvpDataのnullチェック
		if (wvpData) {
			wvpData[particleG.numInstance].World = worldMatrix;

			wvpData[particleG.numInstance].color = (*particleIterator).color;
			wvpData[particleG.numInstance].color.s = alpha;

			//パーティクルカウンター
			if (particleG.numInstance < kNumMaxInstance) {
				++particleG.numInstance;
			}
		}
		++particleIterator;
	}
}

void ParticleManager::VelocityMove(ParticleData& particleData) {
	particleData.transform.translate += particleData.velocityTransform.translate * kDeltaTime;

	particleData.transform.rotate += particleData.velocityTransform.rotate;

	if (particleData.transform.scale.x > 0) {
		particleData.transform.scale.x += particleData.velocityTransform.scale.x * kDeltaTime;
	}
	if (particleData.transform.scale.y > 0) {
		particleData.transform.scale.y += particleData.velocityTransform.scale.y * kDeltaTime;
	}
	if (particleData.transform.scale.z > 0) {
		particleData.transform.scale.z += particleData.velocityTransform.scale.z * kDeltaTime;
	}
}

Matrix4x4 ParticleManager::CreateWorldMatrix(ParticleData& particleData) {
	//スケール行列
	Matrix4x4 scaleMatrix = MakeScaleMatrix(particleData.transform.scale);

	//回転行列
	Matrix4x4 rotateX = MakeRotateXMatrix(particleData.transform.rotate.x * (float(M_PI) / 180.0f));
	Matrix4x4 rotateY = MakeRotateYMatrix(particleData.transform.rotate.y * (float(M_PI) / 180.0f));
	Matrix4x4 rotateZ = MakeRotateZMatrix(particleData.transform.rotate.z * (float(M_PI) / 180.0f));
	//全てまとめた
	Matrix4x4 rotateXYZ = Multiply(Multiply(rotateX, rotateY), rotateZ);
	
	//変換座標
	Matrix4x4 translateMatrix = MakeTranslateMatrix(particleData.transform.translate);

	Matrix4x4 result;
	if (false) {
		//ビルボード
		result = CreateBillBoardMatrix(scaleMatrix, rotateXYZ, translateMatrix);
	}
	else {
		//アフィン変換
		result = MakeAffineMatrix(particleData.transform.scale, particleData.transform.rotate, particleData.transform.translate);
	}

	return result;
}

Matrix4x4 ParticleManager::CreateBillBoardMatrix(const Matrix4x4& scaleMatrix, const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix) {

	//ビルボード
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

	Matrix4x4 billboardMatrix = Multiply(Multiply(backToFrontMatrix, rotateMatrix), camera->GetWorldMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	//全て結合
	Matrix4x4 result = Multiply(scaleMatrix, Multiply(billboardMatrix, translateMatrix));

	return result;
}

void ParticleManager::Timer(ParticleData& particleData) {
	alpha = 1.0f - (particleData.currentTime / particleData.lifeTime);
	particleData.currentTime += kDeltaTime;
}

void ParticleManager::Emit( const std::string& filePath, const Emitter& emitter) {

	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	//
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	particleG.particles.splice(particleG.particles.end(), ParticleEmitter::GetInstance().MakeEmit(filePath ,emitter, randomEngine));
}

uint32_t& ParticleManager::GetNum(const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.numInstance;
}

void ParticleManager::ResetNum(const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	particleG.Updated = false;
}

void ParticleManager::ResetParticle(const std::string& filePath) {
	assert(SrvManager::GetInstance().Max());

	ParticleGroup& particleG = particleGroups[filePath];
	particleG.particles.clear();
}

std::unique_ptr<Particle> ParticleManager::InitParticle(const std::string& name) {
	//パラメータに設定されてないものの場合
	if (!particleParameters_.contains(name)) {
		return nullptr;
	}

	//使用するパーティクルを選ぶ
	std::unique_ptr<Particle>& particle = particles_[name];
	//パーティクルを設定
	particle = std::make_unique<Particle>();

	//設定されたパラメータを読み込む
	ParticleParameters& parameters = particleParameters_[name];

	//初期化(名前,テクスチャファイル,モデルの形)
	particle->Initialize(parameters.name, parameters.textureFile, parameters.primitive);
	particle->SetParticleCount(parameters.count); //生成数を設定
	particle->SetFrequency(parameters.frequency); //頻度 / 生存時間を設定
	particle->SetScale(parameters.basicSize);     //基本サイズを設定

	return std::move(particle);
}

void ParticleManager::InitializeParameter() {

	//Json文字列から解凍したデータ
	nlohmann::json deserialized;

	//ファイルストリーム
	std::ifstream file;

	//読み取れない場合
	file.open("resource/particle.json");

	if (file.fail()) {
		assert(0);
	}

	//解凍処理
	file >> deserialized;

	assert(deserialized.is_object());//オブジェクトがあるか
	assert(deserialized.contains("name"));//名前があるか
	assert(deserialized["name"].is_string());//stringであるか

	//["name"]文字列として取得
	std::string name = deserialized["name"].get<std::string>();

	//正しいレベルデータファイルなのか
	assert(name.compare("Particle") == 0);

	for (nlohmann::json& object : deserialized["particles"]) {
		assert(object.contains("particleName"));

		//使用するパーティクルを選ぶ
		ParticleParameters& particle = particleParameters_[object["particleName"]];
		particle.name = object["particleName"];
		particle.textureFile = object["textureFile"];
		particle.primitive = LoadObject(object["objectType"]);

		nlohmann::json parameter = object["parameter"];

		particle.count = parameter["count"]; //生成数を設定
		particle.frequency = parameter["frequency"]; //頻度 / 生存時間を設定

		Vector3 basicSize;
		basicSize.x = parameter["basicSize"][0];
		basicSize.y = parameter["basicSize"][1];
		basicSize.z = parameter["basicSize"][2];

		particle.basicSize = basicSize;//基本サイズを設定
	}
}

ModelData ParticleManager::LoadObject(const std::string& primitiveName) {
	ModelData result;
	
	if (primitiveName == "Box") {
		result = Primitive::CreateBox();
	}
	else if (primitiveName == "Cone") {
		result = Primitive::CreateCone();
	}
	else if (primitiveName == "Ring") {
		result = Primitive::CreateRing();
	}
	else if (primitiveName == "Sphere") {
		result = Primitive::CreateSphere();
	}
	else if (primitiveName == "Beam") {
		result = Primitive::CreateBeam();
	}
	else if (primitiveName == "Plane") {
		result = Primitive::CreatePlane();
	}
	else if (primitiveName == "Cylineder") {
		result = Primitive::CreateCylinder();
	}
	return result;
}
