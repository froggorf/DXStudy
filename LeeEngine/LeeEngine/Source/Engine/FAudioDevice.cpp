#include "CoreMinimal.h"
#include "FAudioDevice.h"

#include "UEngine.h"
#include "RenderCore/EditorScene.h"

std::shared_ptr<FAudioDevice> GAudioDevice = nullptr;

USoundBase::~USoundBase()
{
	//if(Sound)
	//{
	//	Sound->release();
	//}
}

void USoundBase::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	std::string FilePath = AssetData["SoundFilePath"];
	GAudioDevice->GetFMODSystem()->createSound((GEngine->GetDirectoryPath() + FilePath).c_str(),FMOD_DEFAULT, nullptr,
												&Sound);
	SoundName                          = GetName();
	GetSoundAssetCacheMap()[GetName()] = shared_from_this();
}

// ================
void FActiveSound::Play()
{
	if (SoundBase && SoundBase->Sound)
	{
		GAudioDevice->GetFMODSystem()->playSound(SoundBase->Sound, nullptr, false, &CurrentChannel);
	}
	else
	{
		MY_LOG("FActiveSound", EDebugLogLevel::DLL_Error, "Play Error");
	}
}

void FActiveSound::Stop()
{
	CurrentChannel->stop();
}

// ===============================================
FAudioDevice::FAudioDevice()
{
	System_Create(&FMODSystem);
	FMODSystem->init(512,FMOD_INIT_NORMAL, nullptr);
}

FAudioDevice::~FAudioDevice()
{
	FMODSystem->release();
}

void FAudioDevice::AddNewActiveSound(const std::shared_ptr<FActiveSound>& NewActiveSound)
{
	FAudioThread::ExecuteQueue.push([NewActiveSound]()
	{
		GAudioDevice->PendingAddedActiveSounds.emplace_back(NewActiveSound);
	});
}

void FAudioDevice::GameThread_AudioUpdate()
{
	//FMODSystem->playSound(Sound, nullptr, false, &FMODChannel);

	FAudioThread::ExecuteQueue.push([]()
	{
		GAudioDevice->AudioThread_Update();
	});
}

void FAudioDevice::AudioThread_Update()
{
	for (const auto& PendingAddActiveSound : PendingAddedActiveSounds)
	{
		PendingAddActiveSound->Play();
		ActiveSounds.emplace_back(PendingAddActiveSound);
	}
	PendingAddedActiveSounds.clear();

	FMODSystem->update();

	for (const auto& ActiveSound : ActiveSounds)
	{
		bool bIsPlaying;
		ActiveSound->CurrentChannel->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			PendingStopActiveSounds.emplace_back(ActiveSound);
		}
	}

	for (const auto& PendingStopActiveSound : PendingStopActiveSounds)
	{
		auto TargetIter = std::ranges::find(ActiveSounds, PendingStopActiveSound);
		if (TargetIter != ActiveSounds.end())
		{
			(*TargetIter)->Stop();
			ActiveSounds.erase(TargetIter);
		}
	}
	PendingStopActiveSounds.clear();
}

void FAudioDevice::GameKill()
{
	FAudioThread::bIsGameRunning = false;
}

// =========================
concurrency::concurrent_queue<std::function<void()>> FAudioThread::ExecuteQueue;
std::atomic<bool>                                    FAudioThread::bIsGameRunning = true;
