/// -----------------------------------
///
/// パーティクルエミッタ 
/// 
/// -----------------------------------
#include "ParticleEmitter.h"
#include <numbers>

using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
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
		std::uniform_real_distribution<float> distTransformT(emitter.particleData.distTransformT.distMin, emitter.particleData.distTransformT.distMax);
		std::uniform_real_distribution<float> distTransformR(emitter.particleData.distTransformR.distMin, emitter.particleData.distTransformR.distMax);
		std::uniform_real_distribution<float> distTransformS(emitter.particleData.distTransformS.distMin, emitter.particleData.distTransformS.distMax);

		std::uniform_real_distribution<float> distVelocityT(emitter.particleData.distVelocityT.distMin, emitter.particleData.distVelocityT.distMax);
		std::uniform_real_distribution<float> distVelocityR(emitter.particleData.distVelocityR.distMin, emitter.particleData.distVelocityR.distMax);
		std::uniform_real_distribution<float> distVelocityS(emitter.particleData.distVelocityS.distMin, emitter.particleData.distVelocityS.distMax);

		std::uniform_real_distribution<float> distColor(emitter.particleData.distColor.distMin, emitter.particleData.distColor.distMax);//色用
		std::uniform_real_distribution<float> distLifeTime(emitter.particleData.distLifeTime.distMin, emitter.particleData.distLifeTime.distMax);//生存時間用
		std::uniform_real_distribution<float> distCurrentTime(emitter.particleData.distCurrentTime.distMin, emitter.particleData.distCurrentTime.distMax);//生存時間用

		ParticleData particle;

		Vector3 translate = { distTransformT(randomEngine),distTransformT(randomEngine) ,distTransformT(randomEngine) };
		Vector3 rotate = { distTransformR(randomEngine),distTransformR(randomEngine) ,distTransformR(randomEngine) };
		Vector3 scale = { distTransformS(randomEngine),distTransformS(randomEngine) ,distTransformS(randomEngine) };

		particle.transform.translate = emitter.transform.translate + translate;
		particle.transform.scale = emitter.transform.scale + scale;
		particle.transform.rotate = emitter.transform.rotate + rotate;

		Vector3 velocityTranslate = { distVelocityT(randomEngine),distVelocityT(randomEngine) ,distVelocityT(randomEngine) };
		Vector3 velocityRotate = { distVelocityR(randomEngine),distVelocityR(randomEngine) ,distVelocityR(randomEngine) };
		Vector3 velocityScale = { distVelocityS(randomEngine),distVelocityS(randomEngine) ,distVelocityS(randomEngine) };

		particle.velocityTransform.translate = emitter.particleData.velocityTransform.translate + velocityTranslate;
		particle.velocityTransform.rotate = emitter.particleData.velocityTransform.rotate + velocityRotate;
		particle.velocityTransform.scale = emitter.particleData.velocityTransform.scale + velocityScale;

		Vector4 color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine),distColor(randomEngine) };
		particle.color = emitter.particleData.color + color;

		particle.lifeTime = emitter.particleData.lifeTime + distLifeTime(randomEngine);

		particle.currentTime = emitter.particleData.currentTime + distCurrentTime(randomEngine);

		return particle;
	}

	std::list<ParticleData> ParticleEmitter::MakeEmit(const std::string& particleName, const Emitter& emitter, std::mt19937& randomEngine) {
		std::list<ParticleData> particles;

		for (uint32_t count = 0; count < emitter.count; ++count) {
			particles.push_back(MakeNewParticle(randomEngine, emitter));
		}

		return particles;
	}
}