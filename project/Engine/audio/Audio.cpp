#include "Audio.h"

#include <mfapi.h>
#pragma comment(lib,"mfplat.lib")

#include <mfobjects.h>
#include <mfidl.h>

#include <mfreadwrite.h>
#pragma comment(lib, "Mfreadwrite.lib")

#pragma comment(lib, "mfuuid.lib")

#include "StringUtility.h"

using namespace StringUtility;

std::unique_ptr<Audio> Audio::sInstance_ = nullptr;

Audio& Audio::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Audio>();
	}
	return *sInstance_;
}

void Audio::Finalize() {
	xAudio2_.Reset();
	//SoundUnload(&*soundData_);

	// Windows Media Foundation 終了
	result_ = MFShutdown();
	assert(SUCCEEDED(result_));

	sInstance_.reset();
}

void Audio::Initialize() {
	result_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result_ = xAudio2_->CreateMasteringVoice(&masterVoice_);

	// Windows Media Foundation 初期化
	result_ = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result_));
}

std::unique_ptr<SoundData> Audio::LoadWave(const char* filename) {
	soundData_ = SoundLoadFile(filename);
	return std::move(soundData_);
}

std::unique_ptr<SoundData> Audio::SoundLoadFile(const std::string& fileName) {
	std::wstring filePathW = ConvertString(fileName);

	Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
	result_ = MFCreateSourceReaderFromURL(filePathW.c_str(),nullptr,&pReader);
	assert(SUCCEEDED(result_));

	//PCM形式にフォーマット指定する
	Microsoft::WRL::ComPtr<IMFMediaType> pPCMType;
	MFCreateMediaType(&pPCMType);
	pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result_ = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPCMType.Get());
	assert(SUCCEEDED(result_));

	//セットされたメディアタイプを取得
	Microsoft::WRL::ComPtr<IMFMediaType> pOutType;
	pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,&pOutType);

	//Waveフォーマットを取得
	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);
	
	//コンテナに格納する音声データ
	std::unique_ptr<SoundData> soundData = std::make_unique<SoundData>();
	soundData->wfex = *waveFormat;
	
	result_ = xAudio2_.Get()->CreateSourceVoice(&soundData->pSourceVoice, &soundData->wfex);
	assert(SUCCEEDED(result_));

	//生成したWaveフォーマット解放
	CoTaskMemFree(waveFormat);

	while (true) {
		Microsoft::WRL::ComPtr<IMFSample> pSample;
		DWORD streamIndex = 0;
		DWORD flags = 0;
		LONGLONG LongLongTimeStamp = 0;

		result_ = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &LongLongTimeStamp, &pSample);
		//ストリームの最後尾に達した場合ループから解放
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

		if (pSample) {
			Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
			// サンプルに含まれるサウンドデータのバッファを一繋ぎにして取得
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;
			DWORD maxLength = 0;
			DWORD currentLength = 0;
			// バッファ読み込み用にロック
			pBuffer->Lock(&pData, &maxLength, &currentLength);
			// バッファの最後尾にデータを追加
			soundData->buffer.insert(soundData->buffer.end(), pData, pData + currentLength);
			pBuffer->Unlock();//ロック解除
		}
	}

	return soundData;
}

void Audio::SoundPlayWave(const SoundData& soundData, float volume, bool isLoop) {

	XAUDIO2_VOICE_STATE state;
	soundData.pSourceVoice->GetState(&state);

	//波状データを読み込む
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//ループさせる処理
	if (isLoop) {
		buf.LoopBegin = 0;
		buf.LoopCount = XAUDIO2_MAX_LOOP_COUNT;
		buf.LoopLength = 0;
	}

	result_ = soundData.pSourceVoice->SetVolume(volume);//音量調節
	result_ = soundData.pSourceVoice->SubmitSourceBuffer(&buf);
	result_ = soundData.pSourceVoice->Start();

}

bool Audio::IsPlayingSound(const SoundData& soundData) {
	XAUDIO2_VOICE_STATE state;
	soundData.pSourceVoice->GetState(&state);

	// BuffersQueued が 0 なら再生は終了している
	if (state.BuffersQueued > 0) {
		return true;
	}
	
	return false;
}


void Audio::SoundUnload(SoundData* soundData) {
	soundData->buffer.clear();
	soundData->wfex = {};
}

void Audio::StopWave(const SoundData& soundData) {
	result_ = soundData.pSourceVoice->Stop(); //音源を止める
	result_ = soundData.pSourceVoice->FlushSourceBuffers(); //音源のリセット
}

void Audio::ControlVolume(const SoundData& soundData, float volume) {
	//音量調節
	result_ = soundData.pSourceVoice->SetVolume(volume);
}