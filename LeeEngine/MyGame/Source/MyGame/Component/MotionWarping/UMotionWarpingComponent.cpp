#include "CoreMinimal.h"
#include "UMotionWarpingComponent.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

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
	if (CurrentTime >= WarpingTime)
	{
		CurrentTime = WarpingTime;
	}

	// 이동 적용
	if (MovementComp->PxCharacterController)
	{
		if (bIsSetPosition)
		{
			XMFLOAT3 NewPos = MyMath::Lerp(StartLocation, TargetLocation, (CurrentTime/ WarpingTime));
			NewPos.z *= -1;

			MovementComp->PxCharacterController->setPosition(physx::PxExtendedVec3{NewPos.x,NewPos.y,NewPos.z});
		}
		else
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
	
	}

	if (!bMaintainMotionWarping && CurrentTime >= WarpingTime)
	{
		bMotionWarping = false;
	}
}

void UMotionWarpingComponent::SetTargetLocation(XMFLOAT3 NewLocation, float NewTime, bool bMaintainMotionWarping)
{
	TargetLocation = NewLocation;
	StartLocation = GetOwner()->GetActorLocation();
	WarpingTime = NewTime;
	CurrentTime = 0.0f;
	this->bMaintainMotionWarping = bMaintainMotionWarping;
}
