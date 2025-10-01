#include "CoreMinimal.h"
#include "UMotionWarpingComponent.h"
#include "MyGame/Character/AMyGameCharacterBase.h"

void UMotionWarpingComponent::Register()
{
	UActorComponent::Register();
	
	MyCharacter = dynamic_cast<AMyGameCharacterBase*>(GetOwner());
}

void UMotionWarpingComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);

	if (!bMotionWarping || !MyCharacter) 
	{
		return;
	}

	UCharacterMovementComponent* MovementComp = MyCharacter->GetCharacterMovement();
	if (!MovementComp)
	{
		return;
	}

	CurrentTime += DeltaSeconds;

	// 이동 적용
	if (MovementComp->PxCharacterController)
	{
		XMFLOAT3 CurrentLocation = MyCharacter->GetActorLocation();
		XMFLOAT3 WorldDeltaPos = (TargetLocation - CurrentLocation) * (CurrentTime / WarpingTime);
		physx::PxVec3 PxDelta(WorldDeltaPos.x, WorldDeltaPos.y, -WorldDeltaPos.z);
		MovementComp->PxCharacterController->move(
			PxDelta, 
			0.01f, 
			GEngine->GetDeltaSeconds(), 
			MovementComp->Filters
		);
	}

	if (CurrentTime >= WarpingTime)
	{
		bMotionWarping = false;
	}
}

void UMotionWarpingComponent::SetTargetLocation(XMFLOAT3 NewLocation, float NewTime)
{
	TargetLocation = NewLocation;
	WarpingTime = NewTime;
	CurrentTime = 0.0f;
}
