#include "ParticleEmitter.h"
#include <numbers>

using namespace MyMath;

std::unique_ptr<ParticleEmitter> ParticleEmitter::sInstance_ = nullptr;

ParticleEmitter& ParticleEmitter::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ParticleEmitter>();
	}
	return *sInstance_;
}

void ParticleEmitter::Finalize() {
	sInstance_.reset();
}

ParticleData ParticleEmitter::MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	//std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//座標位置用
	//std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//色用
	//std::uniform_real_distribution<float> distTime(1.0f, 3.0f);//生存時間用

	ParticleData particle;
	particle.transform.translate = emitter.transform.translate;
	particle.transform.scale = emitter.transform.scale;
	particle.transform.rotate = emitter.transform.rotate;

	particle.velocityTransform.translate = emitter.particleData.velocityTransform.translate;
	particle.velocityTransform.rotate = emitter.particleData.velocityTransform.rotate;
	particle.velocityTransform.scale = emitter.particleData.velocityTransform.scale;
	
	particle.color = emitter.particleData.color;

	particle.lifeTime = emitter.particleData.lifeTime;// distTime(randomEngine);
	particle.currentTime = emitter.particleData.currentTime;

	return particle;
}

std::list<ParticleData> ParticleEmitter::MakeEmit(const std::string& particleName, const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<ParticleData> particles;

	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine, emitter));
	}

	return particles;
}
