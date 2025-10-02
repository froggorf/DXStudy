#include "CoreMinimal.h"
#include "UCombatBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/AMyGameCharacterBase.h"

UCombatBaseComponent::UCombatBaseComponent()
{
	
}

void UCombatBaseComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	SetFightMode(false);
}

void UCombatBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	OwnerCharacter = MyCharacter;
}

void UCombatBaseComponent::BasicAttack()
{
	if (bIsBasicAttacking)
	{
		LastBasicAttackClickedTime = GEngine->GetTimeSeconds();
		return;
	}

	// 진행중이 아니면 1타 공격부터 시작하고
	// MontageEnd를 통해서 공격을 계속 진행
	if (const std::shared_ptr<UAnimInstance>& AnimInstance =  OwnerCharacter->GetAnimInstance())
	{
		AnimInstance->Montage_Play(BasicAttackMontages[0], 0, {this, &UCombatBaseComponent::BasicAttackEnded});
	}
	bIsBasicAttacking = true;
}

void UCombatBaseComponent::BasicAttackEnded()
{
	float CurrentTime = GEngine->GetTimeSeconds();
	if (LastBasicAttackClickedTime + BasicAttackApplyTime >= CurrentTime)
	{
		CurrentBasicAttackCombo = (CurrentBasicAttackCombo + 1) % static_cast<UINT>(BasicAttackMontages.size());
		if (const std::shared_ptr<UAnimInstance>& AnimInstance =  OwnerCharacter->GetAnimInstance())
		{
			AnimInstance->Montage_Play(BasicAttackMontages[CurrentBasicAttackCombo], 0, Delegate<>{},Delegate<>{},{this, &UCombatBaseComponent::BasicAttackEnded});
		}
	}
	else
	{
		CurrentBasicAttackCombo = 0;
		bIsBasicAttacking = false;
	}
}

void UCombatBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Started, this, &UCombatBaseComponent::BasicAttack);
}

void UCombatBaseComponent::SetBasicAttackMontages(const std::vector<std::string>& NewMontagesName)
{
	size_t Size = NewMontagesName.size();
	BasicAttackMontages.clear();
	BasicAttackMontages.resize(NewMontagesName.size());
	for (size_t Index = 0; Index < Size; ++Index)
	{
		AssetManager::GetAsyncAssetCache(NewMontagesName[Index],[this, Index](std::shared_ptr<UObject> Object)
		{
			BasicAttackMontages[Index] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!BasicAttackMontages[Index])
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", BasicAttackMontages (Index) Montage Not exist - "+ std::to_string(Index));
#ifdef WITH_EDITOR
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist BasicAttackMontages");
#endif
			}
		});
	}
	
}

void UCombatBaseComponent::SetFightMode(bool NewMode)
{
	if (bIsFightMode == NewMode)
	{
		return;
	}
	bIsFightMode = NewMode;

	const std::shared_ptr<FTimerManager>& TimerManager = GEngine->GetTimerManager();
	//TimerManager->ClearTimer(FindEnemyHandle);

	// 전투중일땐 0.25f마다 적을 찾고 평시 모드일땐 1.0f 마다 적을 찾음
	const float RepeatTime = bIsFightMode ? EnemyFindTime_OnFight : EnemyFindTime_NoFight;
	TimerManager->SetTimer(FindEnemyHandle,Delegate{ this, &UCombatBaseComponent::FindNearbyEnemy},0.5f,true,RepeatTime);
}

void UCombatBaseComponent::FindNearbyEnemy()
{
	XMFLOAT3 SpherePos = OwnerCharacter->GetActorLocation();
	std::vector<AActor*> OverlapActors;
	GPhysicsEngine->SphereOverlapComponents(SpherePos, EnemyFindRadius, {ECollisionChannel::Enemy}, {}, OverlapActors);
	MY_LOG("LOG", EDebugLogLevel::DLL_Error, std::to_string(OverlapActors.size()));

	SetFightMode(!OverlapActors.empty());

}
