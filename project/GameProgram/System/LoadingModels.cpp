#include "LoadingModels.h"

std::shared_ptr<LoadingModels> LoadingModels::sInstance = nullptr;

std::shared_ptr<LoadingModels> LoadingModels::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = std::make_unique<LoadingModels>();
	}
	return sInstance;
}
void LoadingModels::Finalize() {
	sInstance.reset();
	sInstance = nullptr;
}

void LoadingModels::LoadObjects() {
	modelManager = ModelManager::GetInstance().get();

	LoadObj();
	LoadGltf();
}


void LoadingModels::LoadObj() {
	//objの設定
	modelManager->LoadModel("cannon", ".obj");
	modelManager->LoadModel("enemy", ".obj");
	modelManager->LoadModel("PlayerBullet", ".obj");
	modelManager->LoadModel("checkpoint", ".obj");
	modelManager->LoadModel("enemy_bomb", ".obj");
	modelManager->LoadModel("stage_0", ".obj");
	modelManager->LoadModel("stage_1", ".obj");
	modelManager->LoadModel("stage_2", ".obj");
	modelManager->LoadModel("stage_select", ".obj");
	modelManager->LoadModel("warpGate", ".obj");
	modelManager->LoadModel("goal", ".obj");

	modelManager->LoadModel("shadow", ".obj");
	modelManager->LoadModel("player_found_mark", ".obj");
	modelManager->LoadModel("player_lost_mark", ".obj");

}

void LoadingModels::LoadGltf() {
	//gltfの設定
	modelManager->LoadModel("player_standby", ".gltf",true);
	modelManager->LoadModel("player_move", ".gltf", true);
	modelManager->LoadModel("player_shield", ".gltf");
	modelManager->LoadModel("player_GameOver", ".gltf");
	modelManager->LoadModel("player_clear", ".gltf", true);


	modelManager->LoadModel("umbrella_Close", ".gltf");
	modelManager->LoadModel("umbrella_Open", ".gltf");

	modelManager->LoadModel("Select_Start", ".gltf");
	modelManager->LoadModel("Select_End", ".gltf");

	modelManager->LoadModel("Title_stage", ".gltf");
	modelManager->LoadModel("gameover_stage", ".gltf");
	modelManager->LoadModel("EventGate", ".gltf");

	modelManager->LoadModel("player_found_mark", ".gltf"); 
	modelManager->LoadModel("player_lost_mark", ".gltf");
}