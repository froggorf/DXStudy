#include "CoreMinimal.h"
#include "USpringArmComponent.h"

#include "ACharacter.h"
#include "Engine/Physics/UPhysicsEngine.h"

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

	std::vector<ECollisionChannel> Channel(static_cast<UINT>(ECollisionChannel::Count));
	for (size_t i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		Channel.emplace_back(static_cast<ECollisionChannel>(i));	
	}

	FHitResult Result;
	if (gPhysicsEngine->LineTraceSingleByChannel(Transform.GetTranslation(),Offset, Channel, Result))
	{
		TargetOffset = Result.Location;	
	}
	else
	{
		TargetOffset = Offset;
	}

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
