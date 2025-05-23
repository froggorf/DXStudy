#include "CoreMinimal.h"
#include "AnimNotify.h"

void UAnimNotify::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);
}

void UAnimNotify::Notify()
{
}

// =======================================

void UAnimNotify_PlaySound::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimNotify::LoadDataFromFileData(AssetData);

	if (AssetData.contains("SoundName"))
	{
		std::string SoundName = AssetData["SoundName"];
		SoundBase             = USoundBase::GetSoundAsset(SoundName);
	}
}

void UAnimNotify_PlaySound::Notify()
{
	if (SoundBase)
	{
		auto NewActiveSound = std::make_shared<FActiveSound>(SoundBase);
		GAudioDevice->AddNewActiveSound(NewActiveSound);
	}
}
