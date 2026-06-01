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
	EngineLayer::ModelManager::GetInstance().LoadModel("PlayerBullet", ".obj");
	//ステージ
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_0", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_1", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_2", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_3", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_4", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_boss",".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("stage_select", ".obj");
	//ステージオブジェクト
	EngineLayer::ModelManager::GetInstance().LoadModel("checkpoint", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("warpGate", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("goal", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("moveGround", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("energy", ".obj");
	//敵の捜索 「!」「?」
	EngineLayer::ModelManager::GetInstance().LoadModel("player_found_mark", ".obj");
	EngineLayer::ModelManager::GetInstance().LoadModel("player_lost_mark", ".obj");
	//ガイド
	EngineLayer::ModelManager::GetInstance().LoadModel("Guide", ".obj");
}

void LoadingModels::LoadGltf() {
	//gltfの設定
	//プレイヤー アニメーション
	EngineLayer::ModelManager::GetInstance().LoadModel("player_standby", ".gltf",true);
	EngineLayer::ModelManager::GetInstance().LoadModel("player_move", ".gltf", true);
	EngineLayer::ModelManager::GetInstance().LoadModel("player_shield", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("player_GameOver", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("player_clear", ".gltf", true);
	EngineLayer::ModelManager::GetInstance().LoadModel("player_appearance", ".gltf", true);
	// -- 傘 --
	EngineLayer::ModelManager::GetInstance().LoadModel("umbrella_Close", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("umbrella_Open", ".gltf");

	EngineLayer::ModelManager::GetInstance().LoadModel("longUmbrella_Close", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("longUmbrella_Open", ".gltf");	
	
	EngineLayer::ModelManager::GetInstance().LoadModel("foldingUmbrella_Close", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("foldingUmbrella_Open", ".gltf");
	// --------

	EngineLayer::ModelManager::GetInstance().LoadModel("Select_Start", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("Select_End", ".gltf");

	EngineLayer::ModelManager::GetInstance().LoadModel("Title_stage", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("ResultStage", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("EventGate", ".gltf");

	//敵
	EngineLayer::ModelManager::GetInstance().LoadModel("soldier", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("turret", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("bomb", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("rusher", ".gltf", true);
	EngineLayer::ModelManager::GetInstance().LoadModel("Boss", ".gltf", true);

	EngineLayer::ModelManager::GetInstance().LoadModel("shadow", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("skybox", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("backGroundObject", ".gltf");

	EngineLayer::ModelManager::GetInstance().LoadModel("Box", ".gltf");
	EngineLayer::ModelManager::GetInstance().LoadModel("BrokenBox", ".gltf", true);
	EngineLayer::ModelManager::GetInstance().LoadModel("needle", ".gltf");
}