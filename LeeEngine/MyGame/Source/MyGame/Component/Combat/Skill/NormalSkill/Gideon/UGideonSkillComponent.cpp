#include "CoreMinimal.h"
#include "UGideonSkillComponent.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"

void UGideonSkillComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	USkillBaseComponent::Initialize(MyCharacter);

	SetSkillAttackData({"AM_Gideon_Skill"}, 
	{FAttackData{XMFLOAT3{}, 1.0f, 0.0f, 10.0f}},
	15.0f);
	SetUseCastSound(false);
}

void UGideonSkillComponent::ApplySkillAttack()
{
	AGideonCharacter* GideonCharacter = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
	if (!GideonCharacter)
	{
		return;
	}

	USkillBaseComponent::ApplySkillAttack();

	const XMFLOAT3& StartLocation = GideonCharacter->GetActorLocation();
	const XMFLOAT3& ForwardDirection = GideonCharacter->GetActorForwardVector();
	XMFLOAT3 Start = StartLocation + ForwardDirection*100;
	XMFLOAT3 End = Start - XMFLOAT3{0.0f, 5000.0f, 0.0f};
	FHitResult Result;
	bool bIsHit = GPhysicsEngine->LineTraceSingleByChannel(Start,End, {ECollisionChannel::WorldStatic, ECollisionChannel::WorldDynamic}, Result);

	FTransform SpawnTransform;
	SpawnTransform.Translation = bIsHit ? Result.Location +XMFLOAT3{0.0f, 5.0f, 0.0f} : GideonCharacter->GetActorLocation();
	SpawnTransform.Rotation = GideonCharacter->GetActorRotation();
	GideonCharacter->SpawnIceStorm(SpawnTransform, SkillAttackData[0]);
}

