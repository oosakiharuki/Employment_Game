#pragma once
#include "MyMath.h"
#include "ParticleCommon.h"
#include <random>
#include "Primitive.h"

/// <summary>
/// パーティクル用のGPU
/// </summary>
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};
/// <summary>
/// Transform移動/回転/スケールそれぞれの速度の構造体
/// </summary>
struct VelocityTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

/// <summary>
/// パーティクルのパラメータ
/// </summary>
struct ParticleData {
	Transform transform;
	VelocityTransform velocityTransform;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

/// <summary>
/// エミッター
/// </summary>
struct Emitter {
	Transform transform;
	uint32_t count; //発生数
	float frequency; //発生頻度
	float frequencyTime; //頻度時刻
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
/// パーティクル
/// </summary>
class Particle{
public:
	~Particle(); // デストラクタを追加
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="particleName"></param>パーティクルの名前
	/// <param name="textureFile"></param>使用するテクスチャ名
	/// <param name="type"></param>形(Primitive)
	void Initialize(const std::string& particleName,std::string textureFile, PrimitiveType type);
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
	/// <param name="scale"></param>emitterで
	void SetScale(const Vector3& scale) { emitter_.transform.scale = scale; }
	/// <summary>
	/// getter_拡大縮小
	/// </summary>
	/// <returns></returns>
	const Vector3& GetScale() const { return emitter_.transform.scale; }	
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Vector3& translate) { emitter_.transform.translate = translate; }
	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>
	const Vector3& GetTranslate()const { return emitter_.transform.translate; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const Vector3& rotate) { emitter_.transform.rotate = rotate; }
	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRotate() const { return emitter_.transform.rotate; }
	/// <summary>
	/// setter_発生時間
	/// </summary>
	/// <param name="time"></param>発生させる時間
	void SetFrequency(const float time) { 
		emitter_.frequency = time;
		emitter_.frequencyTime = time;
	}
	/// <summary>
	/// setter_カメラ
	/// </summary>
	/// <param name="camera"></param>現在使っているカメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// パーティクル発動モード
	/// </summary>
	/// <param name="mode"></param>時間で出る/一度だけ/出さない
	void SetParticleBorn(ParticleBorn mode) { particleBorn_ = mode; }
	/// <summary>
	/// パーティクルの発生数
	/// </summary>
	/// <param name="countnum"></param>
	void SetParticleCount(uint32_t countnum) { emitter_.count = countnum; }

private:
	ParticleCommon* particleCommon_ = nullptr;

	
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

	Camera* camera_ = nullptr;

	ModelData modelData_;
	Emitter emitter_{};

	ParticleBorn particleBorn_ = ParticleBorn::TimerMode;

	uint32_t number_ = 0;

	//ワールド行列
	Matrix4x4 worldMatrix_;

};