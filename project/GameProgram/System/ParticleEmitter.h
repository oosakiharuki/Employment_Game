#pragma once
#include "MyMath.h"
#include "Particle.h"

/// <summary>
/// パーティクルの発生部分
/// </summary>
class ParticleEmitter{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static ParticleEmitter& GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="particleName">パーティクルの名前</param>
	/// <param name="emitter">使っているエミッター</param>
	/// <param name="randomEngine">ランダムエンジン</param>
	/// <returns></returns>
	std::list<ParticleData> MakeEmit(const std::string& particleName, const Emitter& emitter, std::mt19937& randomEngine);

private:
	//インスタンス
	static std::unique_ptr<ParticleEmitter> sInstance_;

	///パーティクル生成方法一覧
	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter);//通常
	ParticleData MakeNewParticleFixed(const Emitter& emitter);//固定
	ParticleData MakeNewParticleSmaller(std::mt19937& randomEngine, const Emitter& emitter);//だんだん小さく消える
	ParticleData MakeNewParticleSpike(std::mt19937& randomEngine, const Emitter& emitter);//トゲトゲしい
	ParticleData MakeNewParticleExplosion(std::mt19937& randomEngine, const Emitter& emitter);//爆発っぽい
	ParticleData MakeNewParticleFanfare(std::mt19937& randomEngine, const Emitter& emitter);//紙吹雪っぽい
};