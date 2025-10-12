#pragma once
#include "Engine/Class/Framework/APlayerController.h"

class AMyGamePlayerController : public APlayerController
{
	MY_GENERATE_BODY(AMyGamePlayerController)

public:
	void SpawnFloatingDamage(const FTransform& SpawnTransform, const XMFLOAT4& Color, UINT Value, float DigitScale = 1.0f, const std::shared_ptr<UTexture>& DigitTexture = nullptr);
protected:
private:
};