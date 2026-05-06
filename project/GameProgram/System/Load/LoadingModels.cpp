#include "LoadingModels.h"

std::unique_ptr<LoadingModels> LoadingModels::sInstance_ = nullptr;

LoadingModels& LoadingModels::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<LoadingModels>();
	}
	return *sInstance_;
}
void LoadingModels::Finalize() {
	sInstance_.reset();
}

void LoadingModels::LoadObjects() {
	LoadObj();
	LoadGltf();
}


void LoadingModels::LoadObj() {
	//objの設定
	//プレイヤー弾丸
	ModelManager::GetInstance().LoadModel("PlayerBullet", ".obj");
	//ステージ
	ModelManager::GetInstance().LoadModel("stage_0", ".obj");
	ModelManager::GetInstance().LoadModel("stage_1", ".obj");
	ModelManager::GetInstance().LoadModel("stage_2", ".obj");
	ModelManager::GetInstance().LoadModel("stage_3", ".obj");
	ModelManager::GetInstance().LoadModel("stage_4", ".obj");
	ModelManager::GetInstance().LoadModel("stage_boss",".obj");
	ModelManager::GetInstance().LoadModel("stage_select", ".obj");
	//ステージオブジェクト
	ModelManager::GetInstance().LoadModel("checkpoint", ".obj");
	ModelManager::GetInstance().LoadModel("warpGate", ".obj");
	ModelManager::GetInstance().LoadModel("goal", ".obj");
	ModelManager::GetInstance().LoadModel("moveGround", ".obj");
	ModelManager::GetInstance().LoadModel("energy", ".obj");
	//敵の捜索 「!」「?」
	ModelManager::GetInstance().LoadModel("player_found_mark", ".obj");
	ModelManager::GetInstance().LoadModel("player_lost_mark", ".obj");
	//ガイド
	ModelManager::GetInstance().LoadModel("Guide", ".obj");
}

void LoadingModels::LoadGltf() {
	//gltfの設定
	//プレイヤー アニメーション
	ModelManager::GetInstance().LoadModel("player_standby", ".gltf",true);
	ModelManager::GetInstance().LoadModel("player_move", ".gltf", true);
	ModelManager::GetInstance().LoadModel("player_shield", ".gltf");
	ModelManager::GetInstance().LoadModel("player_GameOver", ".gltf");
	ModelManager::GetInstance().LoadModel("player_clear", ".gltf", true);
	ModelManager::GetInstance().LoadModel("player_appearance", ".gltf", true);
	// -- 傘 --
	ModelManager::GetInstance().LoadModel("umbrella_Close", ".gltf");
	ModelManager::GetInstance().LoadModel("umbrella_Open", ".gltf");

	ModelManager::GetInstance().LoadModel("longUmbrella_Close", ".gltf");
	ModelManager::GetInstance().LoadModel("longUmbrella_Open", ".gltf");	
	
	ModelManager::GetInstance().LoadModel("foldingUmbrella_Close", ".gltf");
	ModelManager::GetInstance().LoadModel("foldingUmbrella_Open", ".gltf");
	// --------

	ModelManager::GetInstance().LoadModel("Select_Start", ".gltf");
	ModelManager::GetInstance().LoadModel("Select_End", ".gltf");

	ModelManager::GetInstance().LoadModel("Title_stage", ".gltf");
	ModelManager::GetInstance().LoadModel("ResultStage", ".gltf");
	ModelManager::GetInstance().LoadModel("EventGate", ".gltf");

	//敵
	ModelManager::GetInstance().LoadModel("soldier", ".gltf");
	ModelManager::GetInstance().LoadModel("turret", ".gltf");
	ModelManager::GetInstance().LoadModel("bomb", ".gltf");
	ModelManager::GetInstance().LoadModel("rusher", ".gltf", true);
	ModelManager::GetInstance().LoadModel("Boss", ".gltf", true);

	ModelManager::GetInstance().LoadModel("shadow", ".gltf");
	ModelManager::GetInstance().LoadModel("skybox", ".gltf");
	ModelManager::GetInstance().LoadModel("backGroundObject", ".gltf");

	ModelManager::GetInstance().LoadModel("Box", ".gltf");
	ModelManager::GetInstance().LoadModel("BrokenBox", ".gltf", true);
	ModelManager::GetInstance().LoadModel("needle", ".gltf");
}