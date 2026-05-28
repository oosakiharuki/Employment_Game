/// ----------------------------------
///
/// ステージオブジェクトの基盤クラス
/// 
/// ----------------------------------
#include "IStageObject.h"

using namespace MyMath;

IStageObject::IStageObject(){}

IStageObject::~IStageObject() {}

void IStageObject::ParticleDraw() {
	//パーティクル
	for (auto& particle : particles_) {
		if (particle.second)
			particle.second->Draw();
	}
	
}
