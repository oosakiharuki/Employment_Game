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
	static ParticleEmitter* GetInstance();


	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="emitter"></param>
	/// <param name="randomEngine"></param>
	/// <param name="mosion"></param>
	/// <returns></returns>
	std::list<Particles> MakeEmit(const Emitter& emitter, std::mt19937& randomEngine, ParticleMosion mosion);

private:
	static ParticleEmitter* sInstance_;

	///パーティクル生成方法一覧
	Particles MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter);//通常
	Particles MakeNewParticleFixed(const Emitter& emitter);//固定
	Particles MakeNewParticleSmaller(std::mt19937& randomEngine, const Emitter& emitter);//だんだん小さく消える
	Particles MakeNewParticleSpike(std::mt19937& randomEngine, const Emitter& emitter);//トゲトゲしい
	Particles MakeNewParticleExprosion(std::mt19937& randomEngine, const Emitter& emitter);//爆発っぽい
	Particles MakeNewParticleFanfare(std::mt19937& randomEngine, const Emitter& emitter);//紙吹雪っぽい
	
	static uint32_t sSRVIndexTop_;

	ParticleEmitter() = default;
	~ParticleEmitter() = default;
	ParticleEmitter(ParticleEmitter&) = delete;
	ParticleEmitter& operator=(ParticleEmitter&) = delete;
};