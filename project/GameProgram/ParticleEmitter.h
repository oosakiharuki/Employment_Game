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
	std::list<ParticleData> MakeEmit(const std::string& particleName, const Emitter& emitter, std::mt19937& randomEngine);

private:
	static ParticleEmitter* sInstance_;

	///パーティクル生成方法一覧
	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter);//通常
	ParticleData MakeNewParticleFixed(const Emitter& emitter);//固定
	ParticleData MakeNewParticleSmaller(std::mt19937& randomEngine, const Emitter& emitter);//だんだん小さく消える
	ParticleData MakeNewParticleSpike(std::mt19937& randomEngine, const Emitter& emitter);//トゲトゲしい
	ParticleData MakeNewParticleExprosion(std::mt19937& randomEngine, const Emitter& emitter);//爆発っぽい
	ParticleData MakeNewParticleFanfare(std::mt19937& randomEngine, const Emitter& emitter);//紙吹雪っぽい
	
	static uint32_t sSRVIndexTop_;

	ParticleEmitter() = default;
	~ParticleEmitter() = default;
	ParticleEmitter(ParticleEmitter&) = delete;
	ParticleEmitter& operator=(ParticleEmitter&) = delete;
};