#include "ParticleEmitter.h"
#include <numbers>

using namespace MyMath;

std::shared_ptr<ParticleEmitter> ParticleEmitter::sInstance_ = nullptr;

std::shared_ptr<ParticleEmitter> ParticleEmitter::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ParticleEmitter>();
	}
	return sInstance_;
}

ParticleData ParticleEmitter::MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//座標位置用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//色用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);//生存時間用

	ParticleData particle;
	particle.transform.scale = emitter.transform.scale;
	particle.transform.rotate = emitter.transform.rotate;

	Vector3 randomTranslate{ distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.transform.translate = emitter.transform.translate + randomTranslate;

	particle.velocityTransform.translate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.velocityTransform.rotate = { 0,0,0 };
	particle.velocityTransform.scale = { 0,0,0 };
	
	particle.color = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine),1.0f };
	particle.color = { 1,1,1,1 };

	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

ParticleData ParticleEmitter::MakeNewParticleFixed(const Emitter& emitter) {

	ParticleData particle;
	particle.transform.scale = emitter.transform.scale;
	particle.transform.rotate = emitter.transform.rotate;
	particle.transform.translate = emitter.transform.translate;

	//固定のため動かない
	particle.velocityTransform.translate = { 0,0,0 };
	particle.velocityTransform.rotate = { 0,0,0 };
	particle.velocityTransform.scale = { 0,0,0 };

	particle.color = { 1,1,1,1 };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}


ParticleData ParticleEmitter::MakeNewParticleSmaller(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//座標位置用
	std::uniform_real_distribution<float> distScale(0.1f, 1.0f);//大きさ用
	std::uniform_real_distribution<float> distRotate(0.0f, 360.0f);//回転用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);//生存時間用

	//xyzが同じ大きさにするように
	float scaleValue = distScale(randomEngine);

	ParticleData particle;
	particle.transform.scale = emitter.transform.scale * Vector3(scaleValue, scaleValue, scaleValue);
	particle.transform.rotate = emitter.transform.rotate + Vector3(distRotate(randomEngine), distRotate(randomEngine),distRotate(randomEngine));
	particle.transform.translate = emitter.transform.translate;

	//移動をランダムな方向に
	particle.velocityTransform.translate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.velocityTransform.rotate = { 0,0,0 };
	//どんどん小さくする
	particle.velocityTransform.scale = { -0.5f,-0.5f,-0.5f };

	particle.color = { 1,1,1,1 };

	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

ParticleData ParticleEmitter::MakeNewParticleSpike(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//座標位置用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//色用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);//生存時間用

	//0度 ～ 360度
	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);//回転用
	std::uniform_real_distribution<float> distScale(4.5f, 6.5f);//大きさ用
	
	//細くなるように(y軸のみランダムに)
	Vector3 spikeScale = { 0.05f,distScale(randomEngine),1.0f };
	ParticleData particle;
	particle.transform.scale = emitter.transform.scale * spikeScale;
	//回転させ全方向に
	particle.transform.rotate = { 0.0f,0.0f,distRotate(randomEngine) };
	particle.transform.translate = emitter.transform.translate;

	//ちりばめるとになるとスパイクに見えないため動かさない
	particle.velocityTransform.translate = { 0,0,0 };
	particle.velocityTransform.rotate = { 0,0,0 };
	particle.velocityTransform.scale = { 0,0,0 };
	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}

ParticleData ParticleEmitter::MakeNewParticleExprosion(std::mt19937& randomEngine, const Emitter& emitter) {
	//random
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);//座標位置用
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//色用
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);//生存時間用


	std::uniform_real_distribution<float> distRotate(0.0f,360.0f);
	std::uniform_real_distribution<float> distScale(2.0f, 4.0f);
	const float kScale = 0.5f;

	ParticleData particle;
	Vector3 scale = { kScale,distScale(randomEngine),kScale };
	particle.transform.scale = emitter.transform.scale * scale;
	//回転させ全方向に
	particle.transform.rotate = { distRotate(randomEngine),distRotate(randomEngine),distRotate(randomEngine) };
	particle.transform.translate = emitter.transform.translate;
	
	//ちりばめるとになると爆発に見えないため動かさない
	particle.velocityTransform.translate = { 0,0,0 };
	particle.velocityTransform.rotate = { 0,0,0 };
	//どんどん大きくする
	particle.velocityTransform.scale = { 0.5f,5.0f,0.5f };

	particle.color = { 1.0f,1.0f,1.0f,1.0f };

	particle.lifeTime = emitter.frequency;
	particle.currentTime = 0;

	return particle;
}

ParticleData ParticleEmitter::MakeNewParticleFanfare(std::mt19937& randomEngine, const Emitter& emitter) {
	
	
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);//色用
	
	std::uniform_real_distribution<float> distRotate(0.0f, 360.0f);//回転用
	std::uniform_real_distribution<float> distScale(0.1f, 0.3f);//大きさ用

	std::uniform_real_distribution<float> distWidth(-1.5f, 1.5f);//幅用
	std::uniform_real_distribution<float> distLifeTime(1.0f, 2.0f);//生存時間用
	
	ParticleData gParticle;
	gParticle.transform.scale = emitter.transform.scale * distScale(randomEngine);
	
	gParticle.transform.translate = emitter.transform.translate;
	gParticle.transform.rotate = emitter.transform.rotate;
	gParticle.transform.rotate += distRotate(randomEngine);
	
	gParticle.color = { distColor(randomEngine), distColor(randomEngine) ,distColor(randomEngine) ,distColor(randomEngine) };
	
	const float kFly = 5.0f;
	//真上に飛ばす
	gParticle.velocityTransform.translate = { 0,kFly,0 };
	//散らばるように
	gParticle.velocityTransform.translate += Vector3(distWidth(randomEngine), distWidth(randomEngine), distWidth(randomEngine));
	gParticle.velocityTransform.rotate = { distWidth(randomEngine), distWidth(randomEngine), distWidth(randomEngine) };
	gParticle.velocityTransform.scale = { 0,0,0 };
	
	
	//消える時間を少し伸ばす(1～2倍)
	gParticle.lifeTime = emitter.frequency * distLifeTime(randomEngine);
	gParticle.currentTime = 0.0f;

	return gParticle;
}

std::list<ParticleData> ParticleEmitter::MakeEmit(const std::string& particleName, const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<ParticleData> particles;

	for (uint32_t count = 0; count < emitter.count; ++count) {
		if (particleName == "player_walk") {
			particles.push_back(MakeNewParticleSmaller(randomEngine, emitter));
		}
		else if (particleName == "player_dead") {
			particles.push_back(MakeNewParticleSmaller(randomEngine, emitter));
		}
		else if (particleName == "player_fire") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "player_brink") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "player_pari") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "enemy_fire") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "tullet_laser") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "player_damage") {
			particles.push_back(MakeNewParticleExprosion(randomEngine, emitter));
		}
		else if(particleName == "enemy_damage"){
			particles.push_back(MakeNewParticleExprosion(randomEngine, emitter));
		}
		else if (particleName == "enemies_summon") {
			particles.push_back(MakeNewParticleExprosion(randomEngine, emitter));
		}
		else if (particleName == "title_bullet") {
			particles.push_back(MakeNewParticleFixed(emitter));
		}
		else if (particleName == "clear_fanfare") {
			particles.push_back(MakeNewParticleFanfare(randomEngine, emitter));
		}
	}

	return particles;
}
