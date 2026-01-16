#pragma once

#include "BaseScene.h"
#include <string>

class AbstractSceneFactory
{
public:

	virtual ~AbstractSceneFactory() = default;
	/// <summary>
	/// シーンを生成
	/// </summary>
	/// <param name="sceneName">ベースシーン派生のシーンクラス名</param>
	/// <returns></returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};

