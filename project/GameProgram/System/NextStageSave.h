#pragma once
#include "MyMath.h"
#include <memory>

/// <summary>
/// 移行する前に保存するデータ
/// </summary>
struct SceneSaveData {
	uint32_t playerMaxHp;      //プレイヤーの体力
	uint32_t playerHp;         //プレイヤーの体力
	uint32_t playerRemain;     //残り残機
	std::string nextStageFile; //次のステージ名
	Vector3 checkPoint;        //チェックポイント
};

class NextStageSave
{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static NextStageSave& GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

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
	/// <param name="remain"></param>
	void SetPlayerRemain(uint32_t remain);

	/// <summary>
	/// setter_最大体力
	/// </summary>
	/// <param name="maxHp"></param>
	void SetPlayerMaxHp(uint32_t maxHp);
	
	/// <summary>
	/// setter_チェックポイントの位置
	/// </summary>
	/// <param name="checkPoint">座標位置を設定</param>
	void SetCheckPoint(const Vector3& chekcPoint);
private:

	SceneSaveData sceneSaveData_ = {
		3,3,3,"stage_0" //初期設定
	};
	//インスタンス
	static std::unique_ptr<NextStageSave> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<NextStageSave>;
};

