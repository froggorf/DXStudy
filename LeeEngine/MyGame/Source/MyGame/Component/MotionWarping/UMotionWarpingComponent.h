#pragma once
#include "Engine/Components/UActorComponent.h"

class UMotionWarpingComponent : public UActorComponent
{
	MY_GENERATE_BODY(UMotionWarpingComponent)

	UMotionWarpingComponent() = default;
	~UMotionWarpingComponent() override = default;
	void Register() override;
	void TickComponent(float DeltaSeconds) override;
	void SetTargetLocation(XMFLOAT3 NewLocation, float NewTime);

	void SetMotionWarping(bool NewMotionWarping) { bMotionWarping = NewMotionWarping;} 
protected:
	bool bMotionWarping;

	XMFLOAT3 TargetLocation;
	float WarpingTime;
	float CurrentTime;

	class AMyGameCharacterBase* MyCharacter;
};
