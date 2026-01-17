// 04.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "UObject/UObject.h"

class USoundBase : public UObject
{
	MY_GENERATE_BODY(USoundBase)

	USoundBase() = default;
	~USoundBase() override;
	FMOD::Sound* Sound;
	std::string  SoundName;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	bool operator==(const USoundBase& Other) const
	{
		return SoundName == Other.SoundName;
	}

	static std::shared_ptr<USoundBase> GetSoundAsset(const std::string& SoundAssetName);
};


class FActiveSound
{
public:
	FActiveSound(const std::shared_ptr<USoundBase>& InSoundBase)
		: SoundBase(InSoundBase)
	{
	}

	void                        Play();
	void                        Stop();
	std::shared_ptr<USoundBase> SoundBase;
	FMOD::Channel*              CurrentChannel = nullptr;
};

class FAudioDevice
{
public:
	FAudioDevice();
	virtual ~FAudioDevice();

	FMOD::System* GetFMODSystem() const
	{
		return FMODSystem;
	}

	void AddNewActiveSound(const std::shared_ptr<FActiveSound>& NewActiveSound);
	void GameThread_AudioUpdate();
	void AudioThread_Update();
	void PlaySoundAtLocation(USoundBase* SoundBase, UWorld* World, XMFLOAT3 Location);
	void PlaySound2D(const std::shared_ptr<USoundBase>& SoundBase);
	void StopSound(const std::shared_ptr<FActiveSound>& ActiveSound);
	void GameKill();

protected:
	FMOD::System* FMODSystem;

	std::vector<std::shared_ptr<FActiveSound>> ActiveSounds;
	std::vector<std::shared_ptr<FActiveSound>> PendingAddedActiveSounds;
	std::vector<std::shared_ptr<FActiveSound>> PendingStopActiveSounds;
};

class FAudioThread
{
public:
	static std::atomic<bool> bIsGameRunning;

	static void Execute();

	static concurrency::concurrent_queue<std::function<void()>> ExecuteQueue;
};

extern std::shared_ptr<FAudioDevice> GAudioDevice;

inline void PlaySoundAtLocationByName(const std::shared_ptr<UWorld>& World, const XMFLOAT3& Location, const char* SoundName)
{
	if (!GAudioDevice || !World || !SoundName || SoundName[0] == '\0')
	{
		return;
	}

	if (const std::shared_ptr<USoundBase>& Sound = USoundBase::GetSoundAsset(SoundName))
	{
		GAudioDevice->PlaySoundAtLocation(Sound.get(), World.get(), Location);
	}
}

inline void PlaySound2DByName(const std::shared_ptr<UWorld>& World, const char* SoundName)
{
	if (!GAudioDevice || !World || !SoundName || SoundName[0] == '\0')
	{
		return;
	}

	if (const std::shared_ptr<USoundBase>& Sound = USoundBase::GetSoundAsset(SoundName))
	{
		GAudioDevice->PlaySound2D(Sound);
	}
}

inline void PlaySoundAtLocationRandom(const std::shared_ptr<UWorld>& World, const XMFLOAT3& Location, std::initializer_list<const char*> SoundNames)
{
	if (!GAudioDevice || !World || SoundNames.size() == 0)
	{
		return;
	}

	const int Index = MyMath::RandRange(0, static_cast<int>(SoundNames.size() - 1));
	auto Iter = SoundNames.begin();
	std::advance(Iter, Index);
	if (const std::shared_ptr<USoundBase>& Sound = USoundBase::GetSoundAsset(*Iter))
	{
		GAudioDevice->PlaySoundAtLocation(Sound.get(), World.get(), Location);
	}
}