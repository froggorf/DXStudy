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
	// 로드가 안됐 을 경우 기본공격 막기
	for (size_t i = 0; i < BasicAttackMontages.size(); ++i)
	{
		if (!BasicAttackMontages[i])
		{
			MY_LOG("No Resource" , EDebugLogLevel::DLL_Warning, GetFunctionName + "-> No Montage Yet");
			return;
		}
	}

	if (bIsBasicAttacking)
	{
		LastBasicAttackClickedTime = GEngine->GetTimeSeconds();
		return;
	}

	// 진행중이 아니면 1타 공격부터 시작하고
	// MontageEnd를 통해서 공격을 계속 진행
	if (const std::shared_ptr<UAnimInstance>& AnimInstance =  OwnerCharacter->GetAnimInstance())
	{
		AnimInstance->Montage_Play(BasicAttackMontages[0], 0, Delegate<>(), Delegate<>(), {this, &UCombatBaseComponent::BasicAttackEnded});
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
}

bool UCombatBaseComponent::FindNearbyEnemy(const XMFLOAT3& SpherePos, float EnemyFindRadius,const std::vector<AActor*>& IgnoreActors , std::vector<AActor*>& OverlapActors)
{
	GPhysicsEngine->SphereOverlapComponents(SpherePos, EnemyFindRadius, {ECollisionChannel::Enemy}, {}, OverlapActors);

	return OverlapActors.size() > 0;
}

AActor* UCombatBaseComponent::FindNearestEnemy(const XMFLOAT3& SpherePos, float EnemyFindRadius, const std::vector<AActor*>& IgnoreActors)
{
	std::vector<AActor*> OverlapEnemies;
	if (FindNearbyEnemy(SpherePos, EnemyFindRadius, IgnoreActors, OverlapEnemies))
	{
		float MinDistance = FLT_MAX;
		AActor* NearestEnemy = nullptr;

		for (AActor* Enemy : OverlapEnemies)
		{
			float CurrentDistance = MyMath::GetDistance(SpherePos, Enemy->GetActorLocation());
			if (MinDistance > CurrentDistance)
			{
				MinDistance = CurrentDistance;
				NearestEnemy = Enemy;
			}
		}

		return NearestEnemy;
	}
	else
	{
		return nullptr;
	}

}

float UCombatBaseComponent::GetBasicAttackMoveDistance(size_t Index)
{
	if (Index < 0 || Index >= BasicAttackMoveDistance.size())
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "Wrong index");
		return 100;
	}

	return BasicAttackMoveDistance[Index];
}

const std::shared_ptr<UAnimMontage>& UCombatBaseComponent::GetBasicAttackMontage(size_t Index)
{
	if (Index < 0 || Index >= BasicAttackMontages.size())
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "Wrong index");
		Index = 0;
	}
	return BasicAttackMontages[Index];
}
