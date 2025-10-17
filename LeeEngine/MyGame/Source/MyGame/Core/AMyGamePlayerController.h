#pragma once
#include "Engine/Class/Framework/APlayerController.h"

class AMyGamePlayerController : public APlayerController
{
	MY_GENERATE_BODY(AMyGamePlayerController)

public:
	void SpawnFloatingDamage(const FTransform& SpawnTransform, const XMFLOAT4& Color, UINT Value, float DigitScale = 1.0f, const std::shared_ptr<UTexture>& DigitTexture = nullptr);


	void SetMonochromeCenterComp(const std::shared_ptr<USceneComponent>& TargetComp) { MonochromeCenterComp = TargetComp;}
	XMFLOAT3 GetMonochromeCenterPos();
	void SetMonochromeDistance(float NewDistance) {MonochromeDistance = NewDistance;}
	float GetMonochromeDistance() const {return MonochromeDistance;}
protected:
private:

	std::weak_ptr<USceneComponent> MonochromeCenterComp;
	float MonochromeDistance = 0.0f;
};