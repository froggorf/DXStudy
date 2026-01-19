#include "CoreMinimal.h"
#include "FAudioDevice.h"
#include "UEngine.h"
#include "AssetManager/AssetManager.h"
#include "World/UWorld.h"
#include "Class/Camera/APlayerCameraManager.h"

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
	const bool bLoop = AssetData.contains("bLoop") ? AssetData["bLoop"].get<bool>() : false;
	FMOD_MODE Mode = FMOD_DEFAULT | (bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	GAudioDevice->GetFMODSystem()->createSound((GEngine->GetDirectoryPath() + FilePath).c_str(), Mode, nullptr, &Sound);
	if (bLoop && Sound)
	{
		Sound->setLoopCount(-1);
	}
	SoundName = GetName();
}

std::shared_ptr<USoundBase> USoundBase::GetSoundAsset(const std::string& SoundAssetName)
{
	return std::dynamic_pointer_cast<USoundBase>(AssetManager::GetAssetCacheByName(SoundAssetName));
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
	if (CurrentChannel)
	{
		CurrentChannel->stop();
	}
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
		if (!ActiveSound->CurrentChannel)
		{
			bIsPlaying = false;
		}
		else
		{
			ActiveSound->CurrentChannel->isPlaying(&bIsPlaying);
		}
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
		if (PendingStopActiveSound == CurrentBgm)
		{
			CurrentBgm.reset();
			CurrentBgmName.clear();
		}
	}
	PendingStopActiveSounds.clear();
}

void FAudioDevice::PlaySoundAtLocation(USoundBase* SoundBase, UWorld* World, XMFLOAT3 Location)
{
	if (!SoundBase || !GAudioDevice)
	{
		return;
	}

	const std::shared_ptr<USoundBase> SharedSound = USoundBase::GetSoundAsset(SoundBase->GetName());
	if (!SharedSound || !SharedSound->Sound)
	{
		return;
	}

	XMFLOAT3 ListenerPosition = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 ListenerForward = {0.0f, 0.0f, 1.0f};
	XMFLOAT3 ListenerUp = {0.0f, 1.0f, 0.0f};
	if (World)
	{
		if (APlayerCameraManager* CameraManager = World->GetCameraManager())
		{
			const FViewMatrices ViewMatrices = CameraManager->GetViewMatrices();
			ListenerPosition = ViewMatrices.GetViewOrigin();

			const XMVECTOR CameraRotation = ViewMatrices.GetCameraRotQuat();
			XMVECTOR ForwardVec = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), CameraRotation);
			XMVECTOR UpVec = XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), CameraRotation);
			XMStoreFloat3(&ListenerForward, XMVector3Normalize(ForwardVec));
			XMStoreFloat3(&ListenerUp, XMVector3Normalize(UpVec));
		}
	}

	const std::shared_ptr<FActiveSound> NewActiveSound = std::make_shared<FActiveSound>(SharedSound);

	FAudioThread::ExecuteQueue.push([NewActiveSound, Location, ListenerPosition, ListenerForward, ListenerUp]()
	{
		if (!GAudioDevice || !NewActiveSound || !NewActiveSound->SoundBase || !NewActiveSound->SoundBase->Sound)
		{
			return;
		}

		FMOD::Channel* Channel = nullptr;
		GAudioDevice->GetFMODSystem()->playSound(NewActiveSound->SoundBase->Sound, nullptr, true, &Channel);
		if (!Channel)
		{
			return;
		}

		Channel->setMode(FMOD_3D);

		const FMOD_VECTOR SoundPosition = {Location.x, Location.y, Location.z};
		const FMOD_VECTOR SoundVelocity = {0.0f, 0.0f, 0.0f};
		const FMOD_VECTOR ListenerPos = {ListenerPosition.x, ListenerPosition.y, ListenerPosition.z};
		const FMOD_VECTOR ListenerVel = {0.0f, 0.0f, 0.0f};
		const FMOD_VECTOR ListenerForwardVec = {ListenerForward.x, ListenerForward.y, ListenerForward.z};
		const FMOD_VECTOR ListenerUpVec = {ListenerUp.x, ListenerUp.y, ListenerUp.z};

		GAudioDevice->GetFMODSystem()->set3DListenerAttributes(0, &ListenerPos, &ListenerVel, &ListenerForwardVec, &ListenerUpVec);
		Channel->set3DAttributes(&SoundPosition, &SoundVelocity);
		Channel->set3DMinMaxDistance(200.0f, 5000.0f);
		Channel->setPaused(false);

		NewActiveSound->CurrentChannel = Channel;
		GAudioDevice->ActiveSounds.emplace_back(NewActiveSound);
	});
}

void FAudioDevice::PlaySound2D(const std::shared_ptr<USoundBase>& SoundBase) 
{
	const std::shared_ptr<FActiveSound>& NewActiveSound = std::make_shared<FActiveSound>(SoundBase);
	AddNewActiveSound(NewActiveSound);
}

void FAudioDevice::PlayBgmByName(const char* SoundName)
{
	if (!SoundName || SoundName[0] == '\0')
	{
		return;
	}

	if (const std::shared_ptr<USoundBase>& Sound = USoundBase::GetSoundAsset(SoundName))
	{
		PlayBgm(Sound);
	}
}

void FAudioDevice::PlayBgm(const std::shared_ptr<USoundBase>& SoundBase)
{
	if (!SoundBase)
	{
		return;
	}

	const std::string SoundName = SoundBase->GetName();
	FAudioThread::ExecuteQueue.push([SoundBase, SoundName]()
	{
		if (!GAudioDevice || !SoundBase || !SoundBase->Sound)
		{
			return;
		}

		if (GAudioDevice->CurrentBgm && GAudioDevice->CurrentBgmName == SoundName)
		{
			return;
		}

		if (GAudioDevice->CurrentBgm)
		{
			GAudioDevice->CurrentBgm->Stop();
			auto Iter = std::ranges::find(GAudioDevice->ActiveSounds, GAudioDevice->CurrentBgm);
			if (Iter != GAudioDevice->ActiveSounds.end())
			{
				GAudioDevice->ActiveSounds.erase(Iter);
			}
			GAudioDevice->CurrentBgm.reset();
			GAudioDevice->CurrentBgmName.clear();
		}

		const std::shared_ptr<FActiveSound> NewBgm = std::make_shared<FActiveSound>(SoundBase);
		NewBgm->Play();
		GAudioDevice->ActiveSounds.emplace_back(NewBgm);
		GAudioDevice->CurrentBgm = NewBgm;
		GAudioDevice->CurrentBgmName = SoundName;
	});
}

void FAudioDevice::StopSound(const std::shared_ptr<FActiveSound>& ActiveSound)
{
	if (!ActiveSound)
	{
		return;
	}

	FAudioThread::ExecuteQueue.push([ActiveSound]()
	{
		ActiveSound->Stop();
		GAudioDevice->PendingStopActiveSounds.emplace_back(ActiveSound);
	});
}

void FAudioDevice::StopBgm()
{
	FAudioThread::ExecuteQueue.push([]()
	{
		if (!GAudioDevice || !GAudioDevice->CurrentBgm)
		{
			return;
		}

		GAudioDevice->CurrentBgm->Stop();
		auto Iter = std::ranges::find(GAudioDevice->ActiveSounds, GAudioDevice->CurrentBgm);
		if (Iter != GAudioDevice->ActiveSounds.end())
		{
			GAudioDevice->ActiveSounds.erase(Iter);
		}
		GAudioDevice->CurrentBgm.reset();
		GAudioDevice->CurrentBgmName.clear();
	});
}

void FAudioDevice::GameKill()
{
	FAudioThread::bIsGameRunning = false;
}

void FAudioThread::Execute()
{
	std::function<void()> ExecuteFunc;
	while (true)
	{
		if (!bIsGameRunning)
		{
			break;
		}
		if (ExecuteQueue.try_pop(ExecuteFunc))
		{
			ExecuteFunc();
		}
		else
		{
			std::this_thread::yield();
		}
	}
}

// =========================
concurrency::concurrent_queue<std::function<void()>> FAudioThread::ExecuteQueue;
std::atomic<bool>                                    FAudioThread::bIsGameRunning = true;
