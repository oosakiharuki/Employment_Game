#pragma once
#include <memory>

/// <summary>
/// 全体の時間尺度
/// </summary>
class TimeScale {
public:

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static TimeScale& GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();


	void Update();

	/// <summary>
	/// getter_時間尺度
	/// </summary>
	/// <returns>現在の時間尺度</returns>
	float GetTimeScale() { return timeScale_; }

	/// <summary>
	/// getter_時間尺度の割合(時間尺度率)
	/// </summary>
	/// <returns>現在の時間尺度 / 時間尺度の基準値 (例: [1/60] / [1/60] = 1 , [1/300] / [1/60] = 1/5 )</returns>
	float GetTimeScaleFacto() { return timeScale_ / kDefaultValue_; }

	/// <summary>
	/// setter_時間尺度
	/// </summary>
	/// <param name="time">時間尺度の設定</param>
	void SetTimeScale(float time) { timeScale_ = time; }

private:
	//インスタンス
	static std::unique_ptr<TimeScale> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<TimeScale>;

	const float kDefaultValue_ = 1.0f / 60.0f;//基準値

	float timeScale_ = kDefaultValue_; //時間尺度(タイマースケール)

	//スロー状態
	float slowTime_ = 0.0f;
	const float kSlowTimeMax_ = 2.0f;	
	
	/// <summary>
	/// 時間尺度を基準値にリセット
	/// </summary>
	void ResetTimeScale() { timeScale_ = kDefaultValue_; }

};

