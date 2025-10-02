#include "ParticleEmitter.h"
#include <numbers>

using namespace MyMath;

ParticleEmitter* ParticleEmitter::instance = nullptr;

uint32_t ParticleEmitter::kSRVIndexTop = 1;

ParticleEmitter* ParticleEmitter::GetInstance() {
	if (instance == nullptr) {
		instance = new ParticleEmitter();
	}
	return instance;
}

Particles ParticleEmitter::MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//color用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	Particles particle;
	particle.transform.scale = emitter.transform.scale;
	particle.transform.rotate = emitter.transform.rotate;

	Vector3 randomTranslate{ distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.transform.translate = emitter.transform.translate + randomTranslate;

	particle.velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.color = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine),1.0f };
	particle.color = { 1,1,1,1 };

	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

Particles ParticleEmitter::MakeNewParticleFixed(const Emitter& emitter) {

	Particles particle;
	particle.transform.scale = emitter.transform.scale;
	particle.transform.rotate = emitter.transform.rotate;
	particle.transform.translate = emitter.transform.translate;

	particle.velocity = { 0,0,0 };
	particle.color = { 1,1,1,1 };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}


Particles ParticleEmitter::MakeNewParticleSmaller(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> scaleDist(0.1f, 1.0f);//大きさ用
	std::uniform_real_distribution<float> rotateDist(0.0f, 360.0f);//回転用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	float scaleValue = scaleDist(randomEngine);

	Particles particle;
	particle.transform.scale = emitter.transform.scale * Vector3(scaleValue, scaleValue, scaleValue);
	particle.transform.rotate = emitter.transform.rotate + Vector3(rotateDist(randomEngine), rotateDist(randomEngine),rotateDist(randomEngine));
	particle.transform.translate = emitter.transform.translate;

	particle.velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.color = { 1,1,1,1 };

	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

Particles ParticleEmitter::MakeNewParticleSpike(std::mt19937& randomEngine, const Vector3& translate) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//color用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	//0度 ～ 360度
	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);
	std::uniform_real_distribution<float> distScale(4.5f, 6.5f);

	Particles particle;
	particle.transform.scale = { 0.05f,distScale(randomEngine),1.0f };
	particle.transform.rotate = { 0.0f,0.0f,distRotate(randomEngine) };

	particle.transform.translate = translate;

	particle.velocity = { 0.0f,0.0f,0.0f };
	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = 1.0f;
	particle.currentTime = 0;

	return particle;
}

Particles ParticleEmitter::MakeNewParticleExprosion(std::mt19937& randomEngine, const Vector3& translate) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//color用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);


	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);
	std::uniform_real_distribution<float> distScale(4.5f, 6.5f);

	Particles particle;
	particle.transform.scale = { 0.5f,distScale(randomEngine),0.5f };
	particle.transform.rotate = { distRotate(randomEngine),distRotate(randomEngine),distRotate(randomEngine) };

	particle.transform.translate = translate;

	particle.velocity = { 0.0f,0.0f,0.0f };
	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = 1.0f;
	particle.currentTime = 0;

	return particle;
}

std::list<Particles> ParticleEmitter::MakeEmit(const Emitter& emitter, std::mt19937& randomEngine, ParticleMosion mosion) {
	std::list<Particles> particles;

	switch (mosion)
	{
	case ParticleMosion::Normal:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticle(randomEngine, emitter));
		}
		break;
	case ParticleMosion::Fixed:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		break;
	case ParticleMosion::Smaller:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleSmaller(randomEngine, emitter));
		}
		break;
	case ParticleMosion::Spike:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleSpike(randomEngine, emitter.transform.translate));
		}
		break;
	case ParticleMosion::Exprosion:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleExprosion(randomEngine, emitter.transform.translate));
		}
		break;
	default:
		break;
	}


	return particles;
}
