#pragma once
#include "MyMath.h"
#include "ParticleCommon.h"
#include <random>
#include "Primitive.h"

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct Particles {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct Emitter {
	Transform transform;
	uint32_t count; //発生数
	float frequency; //発生頻度
	float frequencyTime; //頻度時刻
};

struct AccelerationField {
	Vector3 acceleration;
	AABB area;
};

/// <summary>
/// Emitを発動する場面
/// </summary>
enum class BornParticle {
	TimerMode, //タイマーで出てくる
	MomentMode,//瞬間的に出てくる(その後stopに)
	Stop,//出さない
};

/// <summary>
/// Emitterで動きを変化させる
/// </summary>
enum class ParticleMosion {
	Normal,  //あちこち動く
	Fixed,   //固定
	Smaller, //小さくなって消える
	Spike,   //とげとげ (HitEffect)
	Exprosion//爆発っぽい感じ(ring)
};

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
	/// setter_
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { emitter.transform.scale = scale; }
	const Vector3& GetScale() const { return emitter.transform.scale; }

	const Vector3& GetTranslate()const { return emitter.transform.translate; }
	void SetTranslate(const Vector3& translate) { emitter.transform.translate = translate; }

	void SetRotate(const Vector3& rotate) { emitter.transform.rotate = rotate; }
	const Vector3& GetRotate() const { return emitter.transform.rotate; }

	void SetFrequency(const float time) { emitter.frequency = time; }

	void SetCamera(Camera* camera) { this->camera = camera; }

	/// <summary>
	/// パーティクルのモーション
	/// </summary>
	/// <param name="mosion"></param>
	void SetParticleMosion(ParticleMosion mosion) { particleMosion = mosion; }


	bool IsCollision(const AABB& aabb, const Vector3& point);
	/// <summary>
	/// パーティクル発動モード
	/// </summary>
	/// <param name="mode"></param>時間で出る/一度だけ/出さない
	void ChangeMode(BornParticle mode) { bornP = mode; }
	/// <summary>
	/// パーティクルの発生数
	/// </summary>
	/// <param name="countnum"></param>
	void SetParticleCount(uint32_t countnum) { emitter.count = countnum; }
	void Emit();

private:
	ParticleCommon* particleCommon = nullptr;

	
	std::string fileName;
	std::string textureFile;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;


	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;




	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	ParticleForGPU* wvpData = nullptr;

	//ライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource;


	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightSphereData = nullptr;


	static const uint32_t kNumMaxInstance = 100;

	std::list<Particles> particles;
	uint32_t numInstance = 0;

	Transform transformL;

	Camera* camera = nullptr;

	ModelData modelData;
	Emitter emitter{};


	AccelerationField accelerationField;

	BornParticle bornP = BornParticle::TimerMode;
	ParticleMosion particleMosion = ParticleMosion::Normal;

	uint32_t number = 0;

	//ワールド行列
	Matrix4x4 worldMatrix;

};