#include "CoreMinimal.h"
#include "FAudioDevice.h"

#include "UEngine.h"

std::shared_ptr<FAudioDevice> GAudioDevice = nullptr;

void FAudioDevice::AudioThread_Update()
{
	FMOD::System_Create(&FMODSystem);
	FMODSystem->init(512,FMOD_INIT_NORMAL,nullptr);
	FMODSystem->createSound((GEngine->GetDirectoryPath() + "/Content/Resource/Sound/FFXIV_IshgardNight.wav").c_str(), FMOD_DEFAULT, nullptr, &Sound);
	FMODSystem->playSound(Sound,nullptr, false,&FMODChannel);
	bool isPlaying = true;
	while(isPlaying)
	{
		if(!bIsGameRunning)
		{
			break;
		}
		FMODSystem->update();
		FMODChannel->isPlaying(&isPlaying);
	}

	Sound->release();
	FMODSystem->release();

}

void FAudioDevice::GameKill()
{
	bIsGameRunning = false;
}
