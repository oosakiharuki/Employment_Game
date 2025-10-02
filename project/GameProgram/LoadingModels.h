#pragma once
#include "ModelManager.h"

class LoadingModels {
public:
	static LoadingModels* GetInstance();
	void LoadObjects();
	void Finalize();
private:

	ModelManager* modelManager;

	static LoadingModels* instance;

	void LoadObj();
	void LoadGltf();


	LoadingModels() = default;
	~LoadingModels() = default;
	LoadingModels(LoadingModels&) = default;
	LoadingModels& operator=(LoadingModels&) = default;
};