/// ----------------------------------
///
/// パーティクルを作成するマネージャ
/// 
/// ----------------------------------
#include "ParticleManager.h"
#include "ModelManager.h"
#include <cassert>
#include <numbers>

#include "Camera.h"

#include <json.hpp>
#include <fstream>

#include "ImGuiManager.h"

using namespace MyMath;
using namespace Primitive;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
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

	D3D12_GPU_DESCRIPTOR_HANDLE  ParticleManager::GetSrvHandleGPU(const std::string& filePath) {
		assert(SrvManager::GetInstance().Max());

		ParticleGroup& particleG = particleGroups[filePath];
		return particleG.srvHandleGPU;
	}

	ModelData ParticleManager::GetModelData(const std::string& filePath) {
		assert(SrvManager::GetInstance().Max());

		ParticleGroup& particleG = particleGroups[filePath];
		return particleG.modelData;
	}

	std::string ParticleManager::GetTextureFile(const std::string& filePath) {
		assert(SrvManager::GetInstance().Max());

		ParticleGroup& particleG = particleGroups[filePath];
		return particleG.textureFile;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> ParticleManager::GetResource(const std::string& filePath) {
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

	void ParticleManager::Update(const std::string& filePath, ParticleForGPU* wvpData) {
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

	void ParticleManager::Emit(const std::string& filePath, const Emitter& emitter) {

		assert(SrvManager::GetInstance().Max());

		ParticleGroup& particleG = particleGroups[filePath];
		//
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());

		particleG.particles.splice(particleG.particles.end(), ParticleEmitter::GetInstance().MakeEmit(filePath, emitter, randomEngine));
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

		particle->SetParticleUpdater(parameters.particleData);

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

			particle.objectName = object["objectType"];
			particle.primitive = LoadObject(object["objectType"]);

			nlohmann::json parameter = object["parameter"];

			particle.count = parameter["count"]; //生成数を設定
			particle.frequency = parameter["frequency"]; //頻度 / 生存時間を設定

			Vector3 basicSize;
			basicSize.x = parameter["basicSize"][0];
			basicSize.y = parameter["basicSize"][1];
			basicSize.z = parameter["basicSize"][2];

			particle.basicSize = basicSize;//基本サイズを設定

			nlohmann::json emit = object["emit"];
			ParticleData particleData;

			particleData.velocityTransform.translate.x = emit["velocity"]["translate"][0];
			particleData.velocityTransform.translate.y = emit["velocity"]["translate"][1];
			particleData.velocityTransform.translate.z = emit["velocity"]["translate"][2];

			particleData.velocityTransform.rotate.x = emit["velocity"]["rotate"][0];
			particleData.velocityTransform.rotate.y = emit["velocity"]["rotate"][1];
			particleData.velocityTransform.rotate.z = emit["velocity"]["rotate"][2];

			particleData.velocityTransform.scale.x = emit["velocity"]["scale"][0];
			particleData.velocityTransform.scale.y = emit["velocity"]["scale"][1];
			particleData.velocityTransform.scale.z = emit["velocity"]["scale"][2];

			particleData.color.x = emit["color"][0];
			particleData.color.y = emit["color"][1];
			particleData.color.z = emit["color"][2];
			particleData.color.s = emit["color"][3];

			particleData.lifeTime = emit["lifeTime"];
			particleData.currentTime = emit["currentTime"];

			particle.particleData = particleData;

			nlohmann::json dist = object["dist"];
			particle.particleData.distTransformT.distMin = dist["transform"]["translate"][0];
			particle.particleData.distTransformT.distMax = dist["transform"]["translate"][1];

			particle.particleData.distTransformR.distMin = dist["transform"]["rotate"][0];
			particle.particleData.distTransformR.distMax = dist["transform"]["rotate"][1];

			particle.particleData.distTransformS.distMin = dist["transform"]["scale"][0];
			particle.particleData.distTransformS.distMax = dist["transform"]["scale"][1];


			particle.particleData.distVelocityT.distMin = dist["velocity"]["translate"][0];
			particle.particleData.distVelocityT.distMax = dist["velocity"]["translate"][1];

			particle.particleData.distVelocityR.distMin = dist["velocity"]["rotate"][0];
			particle.particleData.distVelocityR.distMax = dist["velocity"]["rotate"][1];

			particle.particleData.distVelocityS.distMin = dist["velocity"]["scale"][0];
			particle.particleData.distVelocityS.distMax = dist["velocity"]["scale"][1];


			particle.particleData.distColor.distMin = dist["color"][0];
			particle.particleData.distColor.distMax = dist["color"][1];

			particle.particleData.distLifeTime.distMin = dist["lifeTime"][0];
			particle.particleData.distLifeTime.distMax = dist["lifeTime"][1];

			particle.particleData.distCurrentTime.distMin = dist["currentTime"][0];
			particle.particleData.distCurrentTime.distMax = dist["currentTime"][1];
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

	void ParticleManager::ParameterImGui() {
#ifdef USE_IMGUI

		ImGui::Begin("ParticleParameter");

		ImGui::InputText("ParameterName", imGuiName.data(), size_t(50));

		if (ImGui::Button("newParticle")) {
			//新しいのを追加
			ParticleParameters& particle = particleParameters_[imGuiName.c_str()];
		}

		ParticleParameters changeParameter;

		for (auto& parameter : particleParameters_) {
			//該当する名前が存在しているなら
			if (imGuiName.c_str() == parameter.first) {
				//設定されたパラメータを読み込む
				int nowCount = parameter.second.count;
				//名前追加
				parameter.second.name = imGuiName.c_str();

				ImGui::InputInt("count", &nowCount);

				//ファイル名がないのならば
				if (parameter.second.textureFile == "") {
					ImGui::InputText("textureFile", textureFileName, IM_ARRAYSIZE(textureFileName));
					if (ImGui::Button("テクスチャ変更決定")) {
						parameter.second.textureFile = textureFileName;//入力したファイル名になる
					}
				}
				else {//ファイル名を変更したいとき
					if (ImGui::Button("ChangeTextureFile")) {
						parameter.second.textureFile = "";
					}
					ImGui::Text("%s", parameter.second.textureFile.c_str());//ファイル名
				}

				//オブジェクトタイプがないのならば
				if (parameter.second.objectName == "") {
					ImGui::InputText("objectName", objectName, IM_ARRAYSIZE(objectName));
					if (ImGui::Button("オブジェクトタイプ変更決定")) {
						parameter.second.objectName = objectName;//入力したオブジェクトタイプになる
					}
				}
				else {//オブジェクトタイプを変更したいとき
					if (ImGui::Button("ChangeObjectName")) {
						parameter.second.objectName = "";
					}
					ImGui::Text("%s", parameter.second.objectName.c_str());//オブジェクトタイプ
				}

				ImGui::InputFloat("frequency", &parameter.second.frequency);
				ImGui::InputFloat3("basicSize", &parameter.second.basicSize.x);

				ImGui::InputFloat3("emit_velocity_translate", &parameter.second.particleData.velocityTransform.translate.x);
				ImGui::InputFloat3("emit_velocity_rotate", &parameter.second.particleData.velocityTransform.rotate.x);
				ImGui::InputFloat3("emit_velocity_scale", &parameter.second.particleData.velocityTransform.scale.x);

				ImGui::InputFloat4("emit_color", &parameter.second.particleData.color.x);

				ImGui::InputFloat("emit_lifeTime", &parameter.second.particleData.lifeTime);

				ImGui::InputFloat("emit_currentTime", &parameter.second.particleData.currentTime);


				ImGui::InputFloat2("DistTransllate", &parameter.second.particleData.distTransformT.distMin);
				ImGui::InputFloat2("DistTransformT", &parameter.second.particleData.distTransformT.distMin);
				ImGui::InputFloat2("DistTransformT", &parameter.second.particleData.distTransformT.distMin);

				ImGui::InputFloat2("DistTransformR", &parameter.second.particleData.distTransformR.distMin);
				ImGui::InputFloat2("DistTransformS", &parameter.second.particleData.distTransformS.distMin);
				ImGui::InputFloat2("DistVelocityT", &parameter.second.particleData.distVelocityT.distMin);
				ImGui::InputFloat2("DistVelocityR", &parameter.second.particleData.distVelocityR.distMin);
				ImGui::InputFloat2("DistVelocityS", &parameter.second.particleData.distVelocityS.distMin);
				ImGui::InputFloat2("DistColor", &parameter.second.particleData.distColor.distMin);
				ImGui::InputFloat2("DistLifeTime", &parameter.second.particleData.distLifeTime.distMin);
				ImGui::InputFloat2("DistCurrentTime", &parameter.second.particleData.distCurrentTime.distMin);



				parameter.second.count = nowCount;
				changeParameter = parameter.second;//変更した値を挿入

				break;
			}
		}

		if (ImGui::Button("Change")) {

			nlohmann::json jsonFile;

			jsonFile["name"] = "Particle";

			uint32_t i = 0;
			for (auto& parameter : particleParameters_) {
				//imGuiNameで選択したパラメータを変更した値を上書き保存
				if (changeParameter.name == parameter.second.name) {
					jsonFile["particles"][i]["particleName"] = changeParameter.name;
					jsonFile["particles"][i]["textureFile"] = changeParameter.textureFile;
					jsonFile["particles"][i]["objectType"] = changeParameter.objectName;
					jsonFile["particles"][i]["parameter"]["count"] = changeParameter.count;

					jsonFile["particles"][i]["parameter"]["frequency"] = DecimalPointCut(changeParameter.frequency);
					jsonFile["particles"][i]["parameter"]["basicSize"][0] = DecimalPointCut(changeParameter.basicSize.x);
					jsonFile["particles"][i]["parameter"]["basicSize"][1] = DecimalPointCut(changeParameter.basicSize.y);
					jsonFile["particles"][i]["parameter"]["basicSize"][2] = DecimalPointCut(changeParameter.basicSize.z);


					jsonFile["particles"][i]["emit"]["velocity"]["translate"][0] = DecimalPointCut(changeParameter.particleData.velocityTransform.translate.x);
					jsonFile["particles"][i]["emit"]["velocity"]["translate"][1] = DecimalPointCut(changeParameter.particleData.velocityTransform.translate.y);
					jsonFile["particles"][i]["emit"]["velocity"]["translate"][2] = DecimalPointCut(changeParameter.particleData.velocityTransform.translate.z);

					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][0] = DecimalPointCut(changeParameter.particleData.velocityTransform.rotate.x);
					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][1] = DecimalPointCut(changeParameter.particleData.velocityTransform.rotate.y);
					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][2] = DecimalPointCut(changeParameter.particleData.velocityTransform.rotate.z);

					jsonFile["particles"][i]["emit"]["velocity"]["scale"][0] = DecimalPointCut(changeParameter.particleData.velocityTransform.scale.x);
					jsonFile["particles"][i]["emit"]["velocity"]["scale"][1] = DecimalPointCut(changeParameter.particleData.velocityTransform.scale.y);
					jsonFile["particles"][i]["emit"]["velocity"]["scale"][2] = DecimalPointCut(changeParameter.particleData.velocityTransform.scale.z);


					jsonFile["particles"][i]["emit"]["color"][0] = int(changeParameter.particleData.color.x);
					jsonFile["particles"][i]["emit"]["color"][1] = int(changeParameter.particleData.color.y);
					jsonFile["particles"][i]["emit"]["color"][2] = int(changeParameter.particleData.color.z);
					jsonFile["particles"][i]["emit"]["color"][3] = int(changeParameter.particleData.color.s);


					jsonFile["particles"][i]["emit"]["lifeTime"] = changeParameter.particleData.lifeTime;
					jsonFile["particles"][i]["emit"]["currentTime"] = changeParameter.particleData.currentTime;


					jsonFile["particles"][i]["dist"]["transform"]["translate"][0] = DecimalPointCut(changeParameter.particleData.distTransformT.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["translate"][1] = DecimalPointCut(changeParameter.particleData.distTransformT.distMax);
					jsonFile["particles"][i]["dist"]["transform"]["rotate"][0] = DecimalPointCut(changeParameter.particleData.distTransformR.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["rotate"][1] = DecimalPointCut(changeParameter.particleData.distTransformR.distMax);
					jsonFile["particles"][i]["dist"]["transform"]["scale"][0] = DecimalPointCut(changeParameter.particleData.distTransformS.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["scale"][1] = DecimalPointCut(changeParameter.particleData.distTransformS.distMax);

					jsonFile["particles"][i]["dist"]["velocity"]["translate"][0] = DecimalPointCut(changeParameter.particleData.distVelocityT.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["translate"][1] = DecimalPointCut(changeParameter.particleData.distVelocityT.distMax);
					jsonFile["particles"][i]["dist"]["velocity"]["rotate"][0] = DecimalPointCut(changeParameter.particleData.distVelocityR.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["rotate"][1] = DecimalPointCut(changeParameter.particleData.distVelocityR.distMax);
					jsonFile["particles"][i]["dist"]["velocity"]["scale"][0] = DecimalPointCut(changeParameter.particleData.distVelocityS.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["scale"][1] = DecimalPointCut(changeParameter.particleData.distVelocityS.distMax);


					jsonFile["particles"][i]["dist"]["color"][0] = DecimalPointCut(changeParameter.particleData.distColor.distMin);
					jsonFile["particles"][i]["dist"]["color"][1] = DecimalPointCut(changeParameter.particleData.distColor.distMax);

					jsonFile["particles"][i]["dist"]["lifeTime"][0] = DecimalPointCut(changeParameter.particleData.distLifeTime.distMin);
					jsonFile["particles"][i]["dist"]["lifeTime"][1] = DecimalPointCut(changeParameter.particleData.distLifeTime.distMax);

					jsonFile["particles"][i]["dist"]["currentTime"][0] = DecimalPointCut(changeParameter.particleData.distCurrentTime.distMin);
					jsonFile["particles"][i]["dist"]["currentTime"][1] = DecimalPointCut(changeParameter.particleData.distCurrentTime.distMax);


				}
				else {
					jsonFile["particles"][i]["particleName"] = parameter.second.name;
					jsonFile["particles"][i]["textureFile"] = parameter.second.textureFile;
					jsonFile["particles"][i]["objectType"] = parameter.second.objectName;
					jsonFile["particles"][i]["parameter"]["count"] = parameter.second.count;

					jsonFile["particles"][i]["parameter"]["frequency"] = DecimalPointCut(parameter.second.frequency);
					jsonFile["particles"][i]["parameter"]["basicSize"][0] = DecimalPointCut(parameter.second.basicSize.x);
					jsonFile["particles"][i]["parameter"]["basicSize"][1] = DecimalPointCut(parameter.second.basicSize.y);
					jsonFile["particles"][i]["parameter"]["basicSize"][2] = DecimalPointCut(parameter.second.basicSize.z);


					jsonFile["particles"][i]["emit"]["velocity"]["translate"][0] = DecimalPointCut(parameter.second.particleData.velocityTransform.translate.x);
					jsonFile["particles"][i]["emit"]["velocity"]["translate"][1] = DecimalPointCut(parameter.second.particleData.velocityTransform.translate.y);
					jsonFile["particles"][i]["emit"]["velocity"]["translate"][2] = DecimalPointCut(parameter.second.particleData.velocityTransform.translate.z);

					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][0] = DecimalPointCut(parameter.second.particleData.velocityTransform.rotate.x);
					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][1] = DecimalPointCut(parameter.second.particleData.velocityTransform.rotate.y);
					jsonFile["particles"][i]["emit"]["velocity"]["rotate"][2] = DecimalPointCut(parameter.second.particleData.velocityTransform.rotate.z);

					jsonFile["particles"][i]["emit"]["velocity"]["scale"][0] = DecimalPointCut(parameter.second.particleData.velocityTransform.scale.x);
					jsonFile["particles"][i]["emit"]["velocity"]["scale"][1] = DecimalPointCut(parameter.second.particleData.velocityTransform.scale.y);
					jsonFile["particles"][i]["emit"]["velocity"]["scale"][2] = DecimalPointCut(parameter.second.particleData.velocityTransform.scale.z);


					jsonFile["particles"][i]["emit"]["color"][0] = int(parameter.second.particleData.color.x);
					jsonFile["particles"][i]["emit"]["color"][1] = int(parameter.second.particleData.color.y);
					jsonFile["particles"][i]["emit"]["color"][2] = int(parameter.second.particleData.color.z);
					jsonFile["particles"][i]["emit"]["color"][3] = int(parameter.second.particleData.color.s);


					jsonFile["particles"][i]["emit"]["lifeTime"] = parameter.second.particleData.lifeTime;
					jsonFile["particles"][i]["emit"]["currentTime"] = parameter.second.particleData.currentTime;


					jsonFile["particles"][i]["dist"]["transform"]["translate"][0] = DecimalPointCut(parameter.second.particleData.distTransformT.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["translate"][1] = DecimalPointCut(parameter.second.particleData.distTransformT.distMax);
					jsonFile["particles"][i]["dist"]["transform"]["rotate"][0] = DecimalPointCut(parameter.second.particleData.distTransformR.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["rotate"][1] = DecimalPointCut(parameter.second.particleData.distTransformR.distMax);
					jsonFile["particles"][i]["dist"]["transform"]["scale"][0] = DecimalPointCut(parameter.second.particleData.distTransformS.distMin);
					jsonFile["particles"][i]["dist"]["transform"]["scale"][1] = DecimalPointCut(parameter.second.particleData.distTransformS.distMax);

					jsonFile["particles"][i]["dist"]["velocity"]["translate"][0] = DecimalPointCut(parameter.second.particleData.distVelocityT.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["translate"][1] = DecimalPointCut(parameter.second.particleData.distVelocityT.distMax);
					jsonFile["particles"][i]["dist"]["velocity"]["rotate"][0] = DecimalPointCut(parameter.second.particleData.distVelocityR.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["rotate"][1] = DecimalPointCut(parameter.second.particleData.distVelocityR.distMax);
					jsonFile["particles"][i]["dist"]["velocity"]["scale"][0] = DecimalPointCut(parameter.second.particleData.distVelocityS.distMin);
					jsonFile["particles"][i]["dist"]["velocity"]["scale"][1] = DecimalPointCut(parameter.second.particleData.distVelocityS.distMax);


					jsonFile["particles"][i]["dist"]["color"][0] = DecimalPointCut(parameter.second.particleData.distColor.distMin);
					jsonFile["particles"][i]["dist"]["color"][1] = DecimalPointCut(parameter.second.particleData.distColor.distMax);

					jsonFile["particles"][i]["dist"]["lifeTime"][0] = DecimalPointCut(parameter.second.particleData.distLifeTime.distMin);
					jsonFile["particles"][i]["dist"]["lifeTime"][1] = DecimalPointCut(parameter.second.particleData.distLifeTime.distMax);

					jsonFile["particles"][i]["dist"]["currentTime"][0] = DecimalPointCut(parameter.second.particleData.distCurrentTime.distMin);
					jsonFile["particles"][i]["dist"]["currentTime"][1] = DecimalPointCut(parameter.second.particleData.distCurrentTime.distMax);

				}
				i++;
			}

			std::ofstream file("resource/particle.json");

			if (file.is_open()) {
				file << jsonFile.dump(4);
				file.close();
			}
		}

		ImGui::End();

#endif // USE_IMGUI

	}

	double ParticleManager::DecimalPointCut(float value) {
		float answer = std::round(value * 1000.0f);
		return answer / 1000.0f;
	}
}