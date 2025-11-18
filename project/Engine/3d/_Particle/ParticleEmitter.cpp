#include "ParticleEmitter.h"
#include <numbers>

using namespace MyMath;

ParticleEmitter* ParticleEmitter::sInstance = nullptr;

uint32_t ParticleEmitter::sSRVIndexTop = 1;

ParticleEmitter* ParticleEmitter::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = new ParticleEmitter();
	}
	return sInstance;
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

Particles ParticleEmitter::MakeNewParticleSpike(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//color用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	//0度 ～ 360度
	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);
	std::uniform_real_distribution<float> distScale(4.5f, 6.5f);

	Particles particle;
	Vector3 spikeScale = { 0.05f,distScale(randomEngine),1.0f };
	particle.transform.scale = emitter.transform.scale * spikeScale;
	particle.transform.rotate = { 0.0f,0.0f,distRotate(randomEngine) };

	particle.transform.translate = emitter.transform.translate;

	particle.velocity = { 0.0f,0.0f,0.0f };
	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}

Particles ParticleEmitter::MakeNewParticleExprosion(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//position用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//color用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);


	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);
	std::uniform_real_distribution<float> distScale(4.5f, 6.5f);

	Particles particle;
	Vector3 scale = { 0.5f,distScale(randomEngine),0.5f };
	particle.transform.scale = emitter.transform.scale * scale;

	particle.transform.rotate = { distRotate(randomEngine),distRotate(randomEngine),distRotate(randomEngine) };

	particle.transform.translate = emitter.transform.translate;

	particle.velocity = { 0.0f,0.0f,0.0f };
	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}

Particles ParticleEmitter::MakeNewParticleFanfare(std::mt19937& randomEngine, const Emitter& emitter) {
	
	
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	
	std::uniform_real_distribution<float> distTranslate(0.0f,5.0f);
	std::uniform_real_distribution<float> distRotate(0.0f, 360.0f);
	std::uniform_real_distribution<float> distScale(0.1f, 0.3f);//大きさ用

	std::uniform_real_distribution<float> distWidth(-1.5f, 1.5f);//幅
	std::uniform_real_distribution<float> distLifeTime(1.0f, 2.0f);//消える時間
	
	Particles gParticle;
	gParticle.transform.scale = emitter.transform.scale * distScale(randomEngine);
	
	gParticle.transform.translate = emitter.transform.translate;
	//gParticle.transform.translate.y += distTranslate(randomEngine);

	gParticle.transform.rotate = emitter.transform.rotate;
	gParticle.transform.rotate += distRotate(randomEngine);
	
	gParticle.color = { distColor(randomEngine), distColor(randomEngine) ,distColor(randomEngine) ,distColor(randomEngine) };
	
	gParticle.velocity = { 0,5.0f,0 };
	gParticle.velocity.x += distWidth(randomEngine);
	gParticle.velocity.z += distWidth(randomEngine);
	gParticle.velocity.y += distWidth(randomEngine);


	gParticle.lifeTime = emitter.frequency * distLifeTime(randomEngine);
	gParticle.currentTime = 0.0f;

	return gParticle;
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
			particles.push_back(MakeNewParticleSpike(randomEngine, emitter));
		}
		break;
	case ParticleMosion::Exprosion:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleExprosion(randomEngine, emitter));
		}
		break;
	case ParticleMosion::Fanfare:
		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticleFanfare(randomEngine, emitter));
		}
		break;
	default:
		break;
	}


	return particles;
}
