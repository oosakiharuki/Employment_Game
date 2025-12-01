#pragma once

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>

//ComPtr
#include <wrl.h>
#include <cassert>
/// <summary>
/// チャンク
/// </summary>
struct ChunkHeader {
	char id[4];//チャンク毎ID
	int32_t size;//サイズ
};
/// <summary>
/// Wave読み取りパラメータ
/// </summary>
struct RiffHeader {
	ChunkHeader chunk;//RIFF
	char type[4];//WAVE
};

struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};
/// <summary>
/// サウンドデータ
/// </summary>
struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファ先頭
	BYTE* pBuffer;
	//サイズ
	unsigned int byfferSize;
	//ソースボイス
	IXAudio2SourceVoice* pSourceVoice = nullptr;
};
/// <summary>
/// サウンド
/// </summary>
class Audio {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static Audio* GetInstance();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 音声をロードする
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData LoadWave(const char* filename);

	/// <summary>
	/// 音声を再生
	/// </summary>
	/// <param name="soundData"></param>流したい音声データ
	/// <param name="volume"></param>音量
	/// <param name="isLoop"></param>ループするか
	void SoundPlayWave(SoundData soundData, float volume, bool isLoop = false);
	/// <summary>
	/// 音声を停止
	/// </summary>
	/// <param name="soundData"></param>止めたい音声データ
	void StopWave(SoundData soundData);
	/// <summary>
	/// 音声のボリューム
	/// </summary>
	/// <param name="soundData"></param>変えたい音声データ
	/// <param name="volume"></param>音量
	void ControlVolume(SoundData soundData, float volume);
private:

	static Audio* sInstance;

	Audio() = default;
	~Audio() = default;
	Audio(Audio&) = default;
	Audio& operator=(Audio&) = default;
	/// <summary>
	/// 
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData SoundLoadWave(const char* filename);//string?

	//音声データの解放 delete
	void SoundUnload(SoundData* soundData);

	SoundData soundData_;

	//audio
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;

	IXAudio2MasteringVoice* masterVoice_;

	HRESULT result_;
};