#pragma once
#include <memory>
#include "IScene.h"

struct SceneGroup {
	std::shared_ptr<IScene> iscene;
};

class SceneFactory {
public:

	void AddScene();

	SceneGroup GetSceneGroup(std::string name);
private:

	std::unordered_map<std::string, SceneGroup> sceneG;
};