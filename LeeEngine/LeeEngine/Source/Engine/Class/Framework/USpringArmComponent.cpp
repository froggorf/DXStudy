#include "CoreMinimal.h"
#include "USpringArmComponent.h"

#include "Engine/Physics/UPhysicsEngine.h"

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

	
}

FTransform USpringArmComponent::GetSocketTransform(const std::string& InSocketName)
{
	FTransform CurTransform = GetComponentTransform();
	CurTransform.Translation = TargetOffset;
	//CurTransform.Translation.x += TargetOffset.x;
	//CurTransform.Translation.y += TargetOffset.y;
	//CurTransform.Translation.z += TargetOffset.z;
	return CurTransform;
}
