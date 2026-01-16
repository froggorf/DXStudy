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

	if (bUsePawnControlRotation && OwningCharacter)
	{
		XMFLOAT4 Rot = OwningCharacter->GetControlRotation();
		SetWorldRotation(XMLoadFloat4(&Rot));
	}

	const FTransform& Transform = GetComponentTransform();

	XMVECTOR ForwardVector = XMVectorSet(0,0,1,0);
	ForwardVector = XMVector3Rotate(ForwardVector, Transform.GetRotationQuat());
	ForwardVector *= TargetArmLength;
	XMFLOAT3 Offset;
	XMStoreFloat3(&Offset,XMVectorSubtract(XMLoadFloat3(&Transform.Translation) ,ForwardVector));
	TargetOffset = Offset;

	if (bCheckCollision)
	{
		std::vector<ECollisionChannel> Channels;
		Channels.reserve(static_cast<UINT>(ECollisionChannel::Count));
		for (size_t i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
		{
			Channels.emplace_back(static_cast<ECollisionChannel>(i));
		}

		FHitResult Result;
		XMFLOAT3 StartLocation = Transform.GetTranslation();
		XMFLOAT3 EndLocation = Offset;

		static constexpr float OwnerIgnoreStep = 5.0f;
		static constexpr int MaxOwnerIgnoreHits = 32;

		for (int Attempt = 0; Attempt < MaxOwnerIgnoreHits; ++Attempt)
		{
			XMVECTOR StartVec = XMLoadFloat3(&StartLocation);
			XMVECTOR EndVec = XMLoadFloat3(&EndLocation);
			XMVECTOR DirVec = XMVectorSubtract(EndVec, StartVec);
			float DirLen = XMVectorGetX(XMVector3Length(DirVec));
			if (DirLen <= FLT_EPSILON)
			{
				break;
			}

			if (!GPhysicsEngine->LineTraceSingleByChannel(StartLocation, EndLocation, Channels, Result))
			{
				break;
			}

			if (Result.HitActor == GetOwner())
			{
				XMFLOAT3 StepDir;
				XMStoreFloat3(&StepDir, XMVectorScale(DirVec, 1.0f / DirLen));
				StartLocation.x = Result.Location.x + StepDir.x * OwnerIgnoreStep;
				StartLocation.y = Result.Location.y + StepDir.y * OwnerIgnoreStep;
				StartLocation.z = Result.Location.z + StepDir.z * OwnerIgnoreStep;
				continue;
			}

			TargetOffset = Result.Location;
			break;
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

}

FTransform USpringArmComponent::GetSocketTransform(const std::string& InSocketName)
{
	FTransform CurTransform = GetComponentTransform();
	CurTransform.Translation = TargetOffset;
	return CurTransform;
}
