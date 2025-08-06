#include "IScene.h"

int IScene::sceneNo = Game;

IScene::~IScene(){}

int IScene::GetSceneNo() { return sceneNo; }