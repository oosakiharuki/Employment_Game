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

#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>

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
		particleG.resource = D3D12CreateResourceManager::GetInstance().CreateBufferResource(sizeof(ParticleForGPU) * particleG.kNumInstance);

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

		selectName_.clear();
		particleParameters_.clear();

		for (nlohmann::json& object : deserialized["particles"]) {
			assert(object.contains("particleName"));

			//名前を追加
			selectName_.push_back(object["particleName"]);

			//使用するパーティクルを選ぶ
			ParticleParameters& particle = particleParameters_[object["particleName"]];
			particle.name = object["particleName"];
			particle.textureFile = object["textureFile"];

			particle.objectName = object["objectType"];
			particle.primitive = LoadObject(object["objectType"]);

			nlohmann::json parameter = object["parameter"];

			particle.count = parameter["count"]; //生成数を設定
			particle.frequency = parameter["frequency"]; //頻度 / 生存時間を設定

			AssignVector3(particle.basicSize, parameter["basicSize"]);//基本サイズを設定

			nlohmann::json emit = object["emit"];
			ParticleData particleData;

			AssignTransform(particleData.velocityTransform, emit["velocity"]);

			particleData.color.x = emit["color"][0];
			particleData.color.y = emit["color"][1];
			particleData.color.z = emit["color"][2];
			particleData.color.s = emit["color"][3];

			particleData.lifeTime = emit["lifeTime"];
			particleData.currentTime = emit["currentTime"];

			particle.particleData = particleData;

			nlohmann::json dist = object["dist"];

			AssignDistTF(particle.particleData.randomDistTransform, dist["transform"]);
			AssignDistTF(particle.particleData.randomDistVelocity, dist["velocity"]);

			AssignDist(particle.particleData.distColor, dist["color"]);
			AssignDist(particle.particleData.distLifeTime, dist["lifeTime"]);
			AssignDist(particle.particleData.distCurrentTime, dist["currentTime"]);
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

		if (ImGui::BeginCombo("パーティクル一覧", selectName_[selectCount_].c_str())) {
			for (int n = 0; n < selectName_.size(); n++) {
				bool is_selected = (selectCount_ == n);
				if (ImGui::Selectable(selectName_[n].c_str(), is_selected)) {
					selectCount_ = n;
				}
				// キーボード操作などでフォーカスを合わせる
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}


		if (ImGui::Button("新しくパーティクルを作る")) {
			isAddParticle_ = true;
			imGuiName = "";//空に
		}
		
		if (isAddParticle_) {
			size_t sizeCount = 50;
			
			ImGui::InputText("名前", imGuiName.data(), sizeCount);
			if (size_t(imGuiName.data()) == sizeCount) {
				imGuiName.erase(sizeCount);
			}

			if (ImGui::Button("決定")) {
				//追加
				ParticleParameters& parameter = particleParameters_[imGuiName.c_str()];//パラメータ追加
				selectName_.push_back(imGuiName);//名前追加
				isAddParticle_ = false;
			}
			else if (ImGui::Button("追加キャンセル")) {
				isAddParticle_ = false;
			}
		}		
		else {
			//既存の名前
			imGuiName = selectName_[selectCount_];
		}

		ParticleParameters changeParameter;

		for (auto& parameter : particleParameters_) {
			//該当する名前が存在しているなら
			if (imGuiName.c_str() == parameter.first) {
				//設定されたパラメータを読み込む
				int nowCount = parameter.second.count;
				//名前追加
				parameter.second.name = imGuiName.c_str();

				ImGui::InputInt("パーティクル発生数", &nowCount);

				if (ImGui::TreeNode("描画モデル / テクスチャ")) {
					//ファイル名がないのならば
					if (parameter.second.textureFile == "") {
						ImGui::InputText("textureFile", textureFileName, IM_ARRAYSIZE(textureFileName));
						if (ImGui::Button("テクスチャ変更決定")) {
							parameter.second.textureFile = textureFileName;//入力したファイル名になる
						}
					}
					else {//ファイル名を変更したいとき
						if (ImGui::Button("テクスチャを変更する")) {
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
						if (ImGui::Button("オブジェクトタイプを変更する")) {
							parameter.second.objectName = "";
						}
						ImGui::Text("%s", parameter.second.objectName.c_str());//オブジェクトタイプ
					}

					ImGui::TreePop();
				}
			

				ImGui::InputFloat("発生頻度", &parameter.second.frequency);
				ImGui::InputFloat3("基本サイズ", &parameter.second.basicSize.x);

				ImGui::InputFloat3("移動速度", &parameter.second.particleData.velocityTransform.translate.x);
				ImGui::InputFloat3("回転速度", &parameter.second.particleData.velocityTransform.rotate.x);
				ImGui::InputFloat3("拡大縮小", &parameter.second.particleData.velocityTransform.scale.x);

				ImGui::InputFloat4("色", &parameter.second.particleData.color.x);

				ImGui::InputFloat("生存時間", &parameter.second.particleData.lifeTime);

				ImGui::InputFloat("現在時間", &parameter.second.particleData.currentTime);

				if (ImGui::TreeNode("ランダム追加")) {
					ImGui::InputFloat2("座標位置範囲", &parameter.second.particleData.randomDistTransform.translate.distMin);
					ImGui::InputFloat2("回転", &parameter.second.particleData.randomDistTransform.rotate.distMin);
					ImGui::InputFloat2("サイズ", &parameter.second.particleData.randomDistTransform.scale.distMin);
					ImGui::InputFloat2("移動速度", &parameter.second.particleData.randomDistVelocity.translate.distMin);
					ImGui::InputFloat2("回転速度", &parameter.second.particleData.randomDistVelocity.rotate.distMin);
					ImGui::InputFloat2("拡縮", &parameter.second.particleData.randomDistVelocity.scale.distMin);
					ImGui::InputFloat2("カラー", &parameter.second.particleData.distColor.distMin);
					ImGui::InputFloat2("描画時間", &parameter.second.particleData.distLifeTime.distMin);
					ImGui::InputFloat2("現在時刻の追加", &parameter.second.particleData.distCurrentTime.distMin);
					ImGui::TreePop();
				}

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
				//選択したパラメータを変更した値を上書き保存(変えてないならparticleParameters_内のデータからまた読み込む)
				if (changeParameter.name == parameter.second.name) {
					jsonFile["particles"][i]["particleName"] = changeParameter.name;
					jsonFile["particles"][i]["textureFile"] = changeParameter.textureFile;
					jsonFile["particles"][i]["objectType"] = changeParameter.objectName;
					jsonFile["particles"][i]["parameter"]["count"] = changeParameter.count;

					jsonFile["particles"][i]["parameter"]["frequency"] = DecimalPointCut(changeParameter.frequency);

					JsonSaveVector3(changeParameter.basicSize, jsonFile["particles"][i]["parameter"]["basicSize"]);

					JsonSaveTransform(changeParameter.particleData.velocityTransform, jsonFile["particles"][i]["emit"]["velocity"]);

					jsonFile["particles"][i]["emit"]["color"][0] = int(changeParameter.particleData.color.x);
					jsonFile["particles"][i]["emit"]["color"][1] = int(changeParameter.particleData.color.y);
					jsonFile["particles"][i]["emit"]["color"][2] = int(changeParameter.particleData.color.z);
					jsonFile["particles"][i]["emit"]["color"][3] = int(changeParameter.particleData.color.s);


					jsonFile["particles"][i]["emit"]["lifeTime"] = changeParameter.particleData.lifeTime;
					jsonFile["particles"][i]["emit"]["currentTime"] = changeParameter.particleData.currentTime;

					JsonSaveDistTF(changeParameter.particleData.randomDistTransform, jsonFile["particles"][i]["dist"]["transform"]);
					JsonSaveDistTF(changeParameter.particleData.randomDistVelocity, jsonFile["particles"][i]["dist"]["velocity"]);

					JsonSaveDist(changeParameter.particleData.distColor, jsonFile["particles"][i]["dist"]["color"]);
					JsonSaveDist(changeParameter.particleData.distLifeTime, jsonFile["particles"][i]["dist"]["lifeTime"]);
					JsonSaveDist(changeParameter.particleData.distCurrentTime, jsonFile["particles"][i]["dist"]["currentTime"]);
				}
				else {
					jsonFile["particles"][i]["particleName"] = parameter.second.name;
					jsonFile["particles"][i]["textureFile"] = parameter.second.textureFile;
					jsonFile["particles"][i]["objectType"] = parameter.second.objectName;
					jsonFile["particles"][i]["parameter"]["count"] = parameter.second.count;

					jsonFile["particles"][i]["parameter"]["frequency"] = DecimalPointCut(parameter.second.frequency);

					JsonSaveVector3(parameter.second.basicSize, jsonFile["particles"][i]["parameter"]["basicSize"]);

					JsonSaveTransform(parameter.second.particleData.velocityTransform, jsonFile["particles"][i]["emit"]["velocity"]);


					jsonFile["particles"][i]["emit"]["color"][0] = int(parameter.second.particleData.color.x);
					jsonFile["particles"][i]["emit"]["color"][1] = int(parameter.second.particleData.color.y);
					jsonFile["particles"][i]["emit"]["color"][2] = int(parameter.second.particleData.color.z);
					jsonFile["particles"][i]["emit"]["color"][3] = int(parameter.second.particleData.color.s);


					jsonFile["particles"][i]["emit"]["lifeTime"] = parameter.second.particleData.lifeTime;
					jsonFile["particles"][i]["emit"]["currentTime"] = parameter.second.particleData.currentTime;

					JsonSaveDistTF(parameter.second.particleData.randomDistTransform, jsonFile["particles"][i]["dist"]["transform"]);
					JsonSaveDistTF(parameter.second.particleData.randomDistVelocity, jsonFile["particles"][i]["dist"]["velocity"]);

					JsonSaveDist(parameter.second.particleData.distColor, jsonFile["particles"][i]["dist"]["color"]);

					JsonSaveDist(parameter.second.particleData.distLifeTime, jsonFile["particles"][i]["dist"]["lifeTime"]);

					JsonSaveDist(parameter.second.particleData.distCurrentTime, jsonFile["particles"][i]["dist"]["currentTime"]);
				}
				i++;
			}

			std::ofstream file("resource/particle.json");

			if (file.is_open()) {
				file << jsonFile.dump(4);
				file.close();
			}

			selectName_.clear();

			InitializeParameter();
		}

		ImGui::End();

#endif // USE_IMGUI

	}

	double ParticleManager::DecimalPointCut(float value) {
		float answer = std::round(value * 1000.0f);
		return answer / 1000.0f;
	}


	void ParticleManager::AssignDist(RandomDist& dist, const nlohmann::json& object) {
		dist.distMin = object[0];
		dist.distMax = object[1];
	}

	void ParticleManager::JsonSaveDist(const RandomDist& dist, nlohmann::json& object) {
		object[0] = DecimalPointCut(dist.distMax);
		object[1] = DecimalPointCut(dist.distMin);
	}

	void ParticleManager::AssignDistTF(RandomDistTF& dist, const nlohmann::json& object) {
		AssignDist(dist.translate, object["translate"]);
		AssignDist(dist.rotate, object["rotate"]);
		AssignDist(dist.scale, object["scale"]);
	}

	void ParticleManager::JsonSaveDistTF(const RandomDistTF& dist, nlohmann::json& object) {
		JsonSaveDist(dist.translate, object["translate"]);
		JsonSaveDist(dist.rotate, object["rotate"]);
		JsonSaveDist(dist.scale, object["scale"]);
	}

	void ParticleManager::AssignVector3(Vector3& vector, const nlohmann::json& object) {
		vector.x = object[0];
		vector.y = object[1];
		vector.z = object[2];
	}

	void ParticleManager::JsonSaveVector3(const Vector3& vector, nlohmann::json& object) {
		object[0] = DecimalPointCut(vector.x);
		object[1] = DecimalPointCut(vector.y);
		object[2] = DecimalPointCut(vector.z);
	}

	void ParticleManager::AssignTransform(Transform& transform, const nlohmann::json& object) {
		AssignVector3(transform.translate, object["translate"]);
		AssignVector3(transform.rotate, object["rotate"]);
		AssignVector3(transform.scale, object["scale"]);
	}

	void ParticleManager::JsonSaveTransform(const Transform& transform, nlohmann::json& object) {
		JsonSaveVector3(transform.translate, object["translate"]);
		JsonSaveVector3(transform.rotate, object["rotate"]);
		JsonSaveVector3(transform.scale, object["scale"]);
	}

}