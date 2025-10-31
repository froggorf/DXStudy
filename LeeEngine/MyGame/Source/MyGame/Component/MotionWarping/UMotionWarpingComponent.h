#pragma once
#include "Engine/Components/UActorComponent.h"

class UMotionWarpingComponent : public UActorComponent
{
	MY_GENERATE_BODY(UMotionWarpingComponent)

	UMotionWarpingComponent() = default;
	~UMotionWarpingComponent() override = default;
	void Register() override;
	void TickComponent(float DeltaSeconds) override;
	void SetTargetLocation(XMFLOAT3 NewLocation, float NewTime, bool bMaintainMotionWarping = false);

	void SetMotionWarping(bool NewMotionWarping) { bMotionWarping = NewMotionWarping; bIsSetPosition = false;}
	void SetbIsSetPosition(bool NewState) {bIsSetPosition = NewState;}
protected:
	bool bMotionWarping;
	bool bIsSetPosition = false;

	XMFLOAT3 TargetLocation;
	XMFLOAT3 StartLocation;
	float WarpingTime;
	float CurrentTime;
	bool bMaintainMotionWarping = false;

	class AMyGameCharacterBase* MyCharacter;
};
