#include "CoreMinimal.h"
#include "UGideonUltimateComponent.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

bool UGideonUltimateComponent::TrySkill()
{
	if (!UUltimateBaseComponent::TrySkill())
	{
		return false;
	}

	// 카메라가 캐릭터 뒷면을 향할 수 있도록 조정해줘야함
	APlayerController* PC = GetWorld()->GetPlayerController();
	if (PC)
	{
		PC->SetActorRotation(MyGameCharacter->GetActorRotation());
		MyGameCharacter->GetSpringArm()->TickComponent(0.0f);
		AGideonCharacter* Gideon = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
		if (Gideon && Gideon->IsAimMode())
		{
			Gideon->ToNormalMode();
		}
	}

	return true;
}

void UGideonUltimateComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UUltimateBaseComponent::Initialize(MyCharacter);
	
	SetMaxUltimateGauge(150.0f);

	SetSkillAttackData({"AM_Gideon_Ult_Start"}, 
		{FAttackData{XMFLOAT3{ 150,150,150},3.0f, -0.0f,5.0f}}
	, 30.0f);
}

void UGideonUltimateComponent::ApplyUltimateAttack()
{
	UUltimateBaseComponent::ApplyUltimateAttack();
}
