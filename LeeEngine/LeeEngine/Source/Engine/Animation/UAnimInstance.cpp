#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include <stack>

#include "UAnimSequence.h"
#include "Bone.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/RenderCore/renderingthread.h"

UAnimInstance::UAnimInstance()
{
	DeltaTime = 0.0f;
}


void UAnimInstance::BeginPlay()
{
	UObject::BeginPlay();

	NativeInitializeAnimation();
}

void UAnimInstance::NativeInitializeAnimation()
{
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
}

void UAnimInstance::UpdateAnimation(float dt)
{

}

void UAnimInstance::Tick(float DeltaSeconds)
{
	DeltaTime = DeltaSeconds;
	CurrentTime = CurrentTime + DeltaSeconds * 15;

	static float TicksPerSecondTime = 1.0f/30*15;
	if(LatestUpdateTime + TicksPerSecondTime < CurrentTime)
	{
		LatestUpdateTime = CurrentTime;

		NativeUpdateAnimation(DeltaSeconds);
		UpdateAnimation(DeltaSeconds);
	}

}



AActor* UAnimInstance::TryGetPawnOwner() const
{
	if (CurrentSkeletalMeshComponent)
	{
		return CurrentSkeletalMeshComponent->GetOwner();
	}
	return nullptr;
}