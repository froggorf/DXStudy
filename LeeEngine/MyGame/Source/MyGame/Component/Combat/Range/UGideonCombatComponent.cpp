#include "CoreMinimal.h"
#include "UGideonCombatComponent.h"

#include "MyGame/Character/Player/AGideonCharacter.h"

UGideonCombatComponent::UGideonCombatComponent()
{
}

void UGideonCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	URangeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackData({
		"AM_Gideon_BasicAttack0",
		"AM_Gideon_BasicAttack1",
		"AM_Gideon_BasicAttack2",
	},
	{
		FAttackData{{10,10,10}, 1.0f, 0.0f, 10.0f, true},
		FAttackData{{20,20,20}, 1.25f, 0.0f, 10.0f, true},
		FAttackData{{30,30,30}, 1.5f, 0.0f, 15.0f, true},
	});
}

void UGideonCombatComponent::BasicAttack()
{
	URangeBaseComponent::BasicAttack();
}

bool UGideonCombatComponent::HeavyAttack()
{
	return URangeBaseComponent::HeavyAttack();
}

void UGideonCombatComponent::HeavyAttackMouseReleased()
{
	URangeBaseComponent::HeavyAttackMouseReleased();
}

void UGideonCombatComponent::ApplyBasicAttack0()
{
	if (AGideonCharacter* GideonCharacter = dynamic_cast<AGideonCharacter*>(MyGameCharacter))
	{
		// 소환 위치를 구하고
		USkeletalMeshComponent* GideonSkeletalMesh = GideonCharacter->GetSkeletalMeshComponent();
		if (!GideonSkeletalMesh)
		{
			return;
		}

		FTransform SpawnTransform = GideonSkeletalMesh->GetSocketTransform("hand_r");
		SpawnTransform.Scale3D = XMFLOAT3{1,1,1};
		SpawnTransform.Rotation = XMFLOAT4{0,0,0,1};

		XMFLOAT3 TargetPosition = {0,0,0};
		// 가까운 적을 구한다음에
		if (AActor* NearestEnemy = FindNearestEnemy(GideonCharacter->GetActorLocation(), FireBallThrowDistance, {}))
		{
			TargetPosition = NearestEnemy->GetActorLocation();
			// 적군이 있으면 해당 적군을 향해 던지고
			
		}
		else
		{
			// 아니면 전방을 향해 던짐
			XMFLOAT3 ForwardDir = GideonCharacter->GetActorForwardVector();
			ForwardDir.y = 0.0f;
			XMStoreFloat3(&ForwardDir, XMVectorScale(XMVector3NormalizeEst(XMLoadFloat3(&ForwardDir)), FireBallThrowDistance));
			TargetPosition = GideonCharacter->GetActorLocation() + ForwardDir;
		}
		GideonCharacter->SpawnFireBall(SpawnTransform, GetBasicAttackData(0), TargetPosition);
	}
	
}
