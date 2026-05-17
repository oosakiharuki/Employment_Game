#pragma once
#include "MyMath.h"
#include "ParticleCommon.h"
#include <random>
#include "Primitive.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// パーティクル用のGPU
	/// </summary>
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct RandomDist {
		float distMin = 0.0f;
		float distMax = 0.0f;
	};

	/// <summary>
	/// パーティクルのパラメータ
	/// </summary>
	struct ParticleData {
		Transform transform;        //発生場所(ゲームの方から代入される)
		Transform velocityTransform;//追加移動
		Vector4 color;              //色
		float lifeTime;             //生存時間
		float currentTime;          //生存用タイマー

		RandomDist distTransformT;
		RandomDist distTransformR;
		RandomDist distTransformS;

		RandomDist distVelocityT;
		RandomDist distVelocityR;
		RandomDist distVelocityS;

		RandomDist distColor;
		RandomDist distLifeTime;
		RandomDist distCurrentTime;
	};

	/// <summary>
	/// エミッター
	/// </summary>
	struct Emitter {
		//発生用処理
		Transform transform;//発生場所
		uint32_t count;     //発生数
		float frequency;    //発生頻度
		float frequencyTime;//頻度時刻
		////発生後の動き
		ParticleData particleData;
	};

	/// <summary>
	/// Emitを発動する場面
	/// </summary>
	enum class ParticleBorn {
		TimerMode, //タイマーで出てくる
		MomentMode,//瞬間的に出てくる(その後stopに)
		Stop,//出さない
	};

	/// <summary>
	/// パーティクルのパラメータ構造体
	/// </summary>
	struct ParticleParameters {
		std::string name;        //パーティクルの名前
		std::string textureFile; //テクスチャファイル名
		std::string objectName;
		ModelData primitive; //パーティクルの形(プリミティブ)
		uint32_t count;          //生成数
		float frequency;         //頻度 / 生存時間
		Vector3 basicSize;       //基本サイズ

		ParticleData particleData;
	};


	/// <summary>
	/// パーティクル
	/// </summary>
	class Particle {
	public:
		~Particle(); // デストラクタを追加
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="particleName">パーティクルの名前</param>
		/// <param name="textureFile">使用するテクスチャ名</param>
		/// <param name="type">形(Primitive)</param>
		void Initialize(const std::string& particleName, const std::string& textureFile, const ModelData& modelData);
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw();

		/// <summary>
		/// setter_拡大縮小
		/// </summary>
		/// <param name="scale">エミッタで指定する大きさ</param>
		void SetScale(const Vector3& scale) { emitter_.transform.scale = scale; }
		/// <summary>
		/// getter_拡大縮小
		/// </summary>
		/// <returns>現在の大きさ</returns>
		const Vector3& GetScale() const { return emitter_.transform.scale; }
		/// <summary>
		/// setter_座標位置
		/// </summary>
		/// <param name="translate">エミッタで指定できる座標位置</param>
		void SetTranslate(const Vector3& translate) { emitter_.transform.translate = translate; }
		/// <summary>
		/// getter_座標位置
		/// </summary>
		/// <returns>現在の位置</returns>
		const Vector3& GetTranslate()const { return emitter_.transform.translate; }
		/// <summary>
		/// setter_回転
		/// </summary>
		/// <param name="rotate">エミッタで指定できる回転角度</param>
		void SetRotate(const Vector3& rotate) { emitter_.transform.rotate = rotate; }
		/// <summary>
		/// getter_回転
		/// </summary>
		/// <returns>現在の回転角度</returns>
		const Vector3& GetRotate() const { return emitter_.transform.rotate; }
		/// <summary>
		/// setter_発生時間
		/// </summary>
		/// <param name="time">発生させる時間</param>
		void SetFrequency(float time) {
			emitter_.frequency = time;
			emitter_.frequencyTime = time;
		}
		/// <summary>
		/// getter_発生時間
		/// </summary>
		/// <returns>設定された発生時間</returns>
		float GetFrequency() { return emitter_.frequency; }

		/// <summary>
		/// setter_カメラ
		/// </summary>
		/// <param name="camera">現在使っているカメラ</param>
		void SetCamera(Camera* camera) { camera_ = camera; }

		/// <summary>
		/// パーティクル発動モード
		/// </summary>
		/// <param name="mode">時間で出る/一度だけ/出さない</param>
		void SetParticleBorn(ParticleBorn mode) { particleBorn_ = mode; }
		/// <summary>
		/// パーティクルの発生数
		/// </summary>
		/// <param name="count">発生数を決める</param>
		void SetParticleCount(uint32_t count) { emitter_.count = count; }

		void SetParticleUpdater(ParticleData particleUpdater) { emitter_.particleData = particleUpdater; }

		/// <summary>
		/// setter_色(あとから設定)
		/// </summary>
		/// <param name="color">RGBAの配色</param>
		void SetColor(const Vector4& color) { emitter_.particleData.color = color; }

	private:

		std::string fileName_;
		std::string textureFile_;

		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;


		VertexData* vertexData_ = nullptr;
		Material* materialData_ = nullptr;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;




		Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
		ParticleForGPU* wvpData_ = nullptr;

		//ライト用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource_;


		//マテリアルにデータを書き込む
		DirectionalLight* directionalLightSphereData_ = nullptr;


		static const uint32_t kNumMaxInstance_ = 100;

		std::list<ParticleData> particles_;
		uint32_t numInstance_ = 0;

		EngineLayer::Camera* camera_ = nullptr;

		ModelData modelData_;
		Emitter emitter_{};

		ParticleBorn particleBorn_ = ParticleBorn::TimerMode;

		uint32_t number_ = 0;

		//ワールド行列
		Matrix4x4 worldMatrix_;

		const float kDeltaTime = 1.0f / 60.0f;

		/// <summary>
		/// 座標
		/// </summary>
		void InitVertex();


		/// <summary>
		/// マテリアル作成
		/// </summary>
		void InitMaterial();

		/// <summary>
		/// パラメータの初期化
		/// </summary>
		void InitParameter();
	};
}