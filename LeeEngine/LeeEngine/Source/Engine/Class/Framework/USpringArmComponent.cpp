#include "CoreMinimal.h"
#include "USpringArmComponent.h"

#include "ACharacter.h"
#include "Engine/Physics/UPhysicsEngine.h"

USpringArmComponent::USpringArmComponent()
{
	Rename("SpringArm" + std::to_string(ComponentID));
}

void USpringArmComponent::BeginPlay()
{
	USceneComponent::BeginPlay();

	OwningCharacter = dynamic_cast<ACharacter*>(GetOwner());
}

void USpringArmComponent::TickComponent(float DeltaSeconds)
{
	USceneComponent::TickComponent(DeltaSeconds);

	

	const FTransform& Transform = GetComponentTransform();

	XMVECTOR ForwardVector = XMVectorSet(0,0,1,0);
	ForwardVector = XMVector3Rotate(ForwardVector, Transform.GetRotationQuat());
	ForwardVector *= TargetArmLength;
	XMFLOAT3 Offset;
	XMStoreFloat3(&Offset,XMVectorSubtract(XMLoadFloat3(&Transform.Translation) ,ForwardVector));
	TargetOffset = Offset;

	if (bCheckCollision)
	{
		std::vector<ECollisionChannel> Channel(static_cast<UINT>(ECollisionChannel::Count));
		for (size_t i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
		{
			Channel.emplace_back(static_cast<ECollisionChannel>(i));	
		}

		FHitResult Result;
		bool bHitOwner = false;
		XMFLOAT3 StartLocation = Transform.GetTranslation();
		int BreakCount = 0;
		while (true)
		{
			if (GPhysicsEngine->LineTraceSingleByChannel(StartLocation, Offset, Channel, Result))
			{
				if (Result.HitActor == GetOwner())
				{
					if (BreakCount++ > 1'000) break;
					XMFLOAT3 GapDirection;
					float Gap = 0.2f;
					XMStoreFloat3(&GapDirection, XMVectorScale(XMVectorSubtract(XMLoadFloat3(&Offset), XMLoadFloat3(&StartLocation)), Gap));

					StartLocation.x = Result.Location.x + GapDirection.x;
					StartLocation.y = Result.Location.y + GapDirection.y;
					StartLocation.z = Result.Location.z + GapDirection.z;
				}
				else
				{
					TargetOffset = Result.Location;
					break;
				}
			}
			else
			{
				TargetOffset = Offset;
				break;
			}

		}
	}
	
	//if (gPhysicsEngine->LineTraceSingleByChannel(Transform.GetTranslation(),Offset, Channel, Result))
	//{
	//	TargetOffset = Result.Location;	
	//}
	//else
	//{
	//	TargetOffset = Offset;
	//}

	if (bUsePawnControlRotation && OwningCharacter)
	{
		XMFLOAT4 Rot = OwningCharacter->GetControlRotation();
		SetWorldRotation(XMLoadFloat4(&Rot));
	}
}

FTransform USpringArmComponent::GetSocketTransform(const std::string& InSocketName)
{
	FTransform CurTransform = GetComponentTransform();
	CurTransform.Translation = TargetOffset;
	return CurTransform;
}
