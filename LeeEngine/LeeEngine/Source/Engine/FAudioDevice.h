// 04.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"

#include <concurrent_queue.h>

#include "Engine/MyEngineUtils.h"
#include "UObject/UObject.h"

class USoundBase : public UObject, public std::enable_shared_from_this<USoundBase>
{
	MY_GENERATED_BODY(USoundBase)
public:
	USoundBase() = default;
	~USoundBase() override;
	FMOD::Sound* Sound;
	std::string SoundName;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	bool operator==(const USoundBase& Other) const
	{
		return SoundName == Other.SoundName;
	}

	static std::shared_ptr<USoundBase> GetSoundAsset(const std::string& SoundAssetName)
	{
		auto SoundAssetCacheMap = GetSoundAssetCacheMap();
		if(SoundAssetCacheMap.contains(SoundAssetName))
		{
			return SoundAssetCacheMap[SoundAssetName];
		}
		return nullptr;
	}
protected:
private:
public:
protected:
private:
	static std::map<std::string, std::shared_ptr<USoundBase>>& GetSoundAssetCacheMap()
	{
		static std::map<std::string, std::shared_ptr<USoundBase>> SoundAssetCacheMap;
		return SoundAssetCacheMap;
	}
};

class FActiveSound
{
public:
	FActiveSound(const std::shared_ptr<USoundBase>& InSoundBase) : SoundBase(InSoundBase){}
	void Play();
	void Stop();
public:
	std::shared_ptr<USoundBase> SoundBase;
	FMOD::Channel* CurrentChannel = nullptr;

};

class FAudioDevice 
{
public:
	FAudioDevice();
	~FAudioDevice();

	FMOD::System* GetFMODSystem() const {return FMODSystem;}

	void AddNewActiveSound(const std::shared_ptr<FActiveSound>& NewActiveSound);
	void GameThread_AudioUpdate();
	void AudioThread_Update();
	void PlaySoundAtLocation(USoundBase* SoundBase, UWorld* World, XMFLOAT3 Location);
	void GameKill();
protected:
	FMOD::System* FMODSystem;

	std::vector<std::shared_ptr<FActiveSound>> ActiveSounds;
	std::vector<std::shared_ptr<FActiveSound>> PendingAddedActiveSounds;
	std::vector<std::shared_ptr<FActiveSound>> PendingStopActiveSounds;

private:


};

class FAudioThread
{
public:
	static std::atomic<bool> bIsGameRunning;
	static void Execute()
	{
		std::function<void()> ExecuteFunc;
		while(true)
		{
			if(!bIsGameRunning)
			{
				break;
			}
			if(ExecuteQueue.try_pop(ExecuteFunc))
			{
				ExecuteFunc();
			}else
			{
				std::this_thread::yield();
			}
		}
	}
public:
	static concurrency::concurrent_queue<std::function<void()>> ExecuteQueue;

};

extern std::shared_ptr<FAudioDevice> GAudioDevice;