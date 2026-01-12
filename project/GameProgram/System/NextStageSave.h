#pragma once
#include "MyMath.h"
#include <memory>

/// <summary>
/// 移行する前に保存するデータ
/// </summary>
struct SceneSaveData {
	uint32_t playerMaxHp;      //プレイヤーの体力
	uint32_t playerHp;         //プレイヤーの体力
	uint32_t playerZanki;       //残り残機
	std::string nextStageFile; //次のステージ名
};

class NextStageSave
{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<NextStageSave> GetInstance();
	/// <summary>
	/// getter_前に受け取ったデータを渡す
	/// </summary>
	/// <returns></returns> 前に受け取ったデータ
	SceneSaveData GetNextStageSaveData() { return sceneSaveData_; }
	/// <summary>
	/// setter_次のステージの全体層
	/// </summary>
	/// <param name="nextStageFile"></param> 次のステージの全体層のファイルパス
	void SetNextStageFile(const std::string& nextStageFile) { sceneSaveData_.nextStageFile = nextStageFile; }
	
	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="playerHp"></param> 
	void SetPlayerHp(uint32_t playerHp);

	/// <summary>
	/// setter_残機
	/// </summary>
	/// <param name="zanki"></param>
	void SetPlayerZanki(uint32_t zanki);

	/// <summary>
	/// setter_最大体力
	/// </summary>
	/// <param name="maxHp"></param>
	void SetPlayerMaxHp(uint32_t maxHp);
private:

	SceneSaveData sceneSaveData_ = {
		3,3,2,"stage_0" //初期設定
	};

	static std::shared_ptr<NextStageSave> sInstance_;
};

