#include "CoreMinimal.h"
#include "FAudioDevice.h"

#include "UEngine.h"

std::shared_ptr<FAudioDevice> GAudioDevice = nullptr;

FAudioDevice::FAudioDevice()
{
	FMOD::System_Create(&FMODSystem);
	FMODSystem->init(512,FMOD_INIT_NORMAL,nullptr);
	FMODSystem->createSound((GEngine->GetDirectoryPath() + "/Content/Resource/Sound/FFXIV_IshgardNight.wav").c_str(), FMOD_DEFAULT, nullptr, &Sound);
}

FAudioDevice::~FAudioDevice()
{
	Sound->release();
	FMODSystem->release();
}

void FAudioDevice::AudioThread_Update()
{
	FMODSystem->playSound(Sound, nullptr, false, &FMODChannel);
	while(bIsGameRunning)
	{
		FMODSystem->update();
	}

}

void FAudioDevice::GameKill()
{
	bIsGameRunning = false;
}
