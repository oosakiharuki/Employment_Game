#include "LoadingModels.h"

LoadingModels* LoadingModels::instance = nullptr;

LoadingModels* LoadingModels::GetInstance() {
	if (instance == nullptr) {
		instance = new LoadingModels;
	}
	return instance;
}
void LoadingModels::Finalize() {
	delete instance;
	instance = nullptr;
}

void LoadingModels::LoadObjects() {
	modelManager = ModelManager::GetInstance();

	LoadObj();
	LoadGltf();
}


void LoadingModels::LoadObj() {
	modelManager->LoadModel("cannon", ".obj");
	modelManager->LoadModel("playerHead", ".obj");
	modelManager->LoadModel("enemy", ".obj");
	modelManager->LoadModel("stage_proto", ".obj");
	modelManager->LoadModel("PlayerBullet", ".obj");
	modelManager->LoadModel("umbrella_Open", ".obj");
	modelManager->LoadModel("umbrella_Close", ".obj");
	modelManager->LoadModel("checkpoint", ".obj");
	modelManager->LoadModel("sphere", ".obj");
	modelManager->LoadModel("stage_0", ".obj");
	modelManager->LoadModel("stage_1", ".obj");
	modelManager->LoadModel("warpGate", ".obj");

	modelManager->LoadModel("shadow", ".obj");

	modelManager->LoadModel("Title", ".obj");
}

void LoadingModels::LoadGltf() {
	modelManager->LoadModel("walk", ".gltf");
	modelManager->LoadModel("sneakWalk", ".gltf");
	modelManager->LoadModel("stop", ".gltf");
}