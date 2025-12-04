#pragma once
#include <memory>
#include "BaseScene.h"

class SceneFactory {
public:
	/// <summary>
	/// 追加するシーンを設定
	/// </summary>
	void MakeScene(const std::string& preScene);

	/// <summary>
	/// 使用するシーン
	/// </summary>
	/// <param name="name"></param>使用するシーン名
	/// <returns></returns>
	std::unique_ptr<BaseScene> GetSceneGroup(std::string name);
private:

	std::unordered_map<std::string, std::unique_ptr<BaseScene>> sceneG_;
};