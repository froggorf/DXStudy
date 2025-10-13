#include "CoreMinimal.h"
#include "UCombatBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

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
	MyGameCharacter = MyCharacter;
}

void UCombatBaseComponent::BasicAttack()
{
	// 로드가 안됐 을 경우 기본공격 막기
	for (size_t i = 0; i < BasicAttackData.size(); ++i)
	{
		if (!BasicAttackData[i].AnimMontage)
		{
			MY_LOG("No Resource" , EDebugLogLevel::DLL_Warning, GetFunctionName + "-> No Montage Yet" + std::to_string(i));
			return;
		}
	}

	// HeavyAttack 에서 사용되는 시간값
	LastLeftMouseClickedTime = GEngine->GetTimeSeconds();

	if (bIsBasicAttacking)
	{
		LastBasicAttackClickedTime = GEngine->GetTimeSeconds();
		return;
	}

	// 진행중이 아니면 1타 공격부터 시작하고
	// MontageEnd를 통해서 공격을 계속 진행
	if (const std::shared_ptr<UAnimInstance>& AnimInstance =  MyGameCharacter->GetAnimInstance())
	{
		AnimInstance->Montage_Play(BasicAttackData[0].AnimMontage, 0, Delegate<>(), Delegate<>(), {this, &UCombatBaseComponent::BasicAttackEnded});
	}
	bIsBasicAttacking = true;
}

void UCombatBaseComponent::BasicAttackEnded()
{
	float CurrentTime = GEngine->GetTimeSeconds();
	if (LastBasicAttackClickedTime + BasicAttackApplyTime >= CurrentTime)
	{
		CurrentBasicAttackCombo = (CurrentBasicAttackCombo + 1) % static_cast<UINT>(BasicAttackData.size());
		if (const std::shared_ptr<UAnimInstance>& AnimInstance =  MyGameCharacter->GetAnimInstance())
		{
			AnimInstance->Montage_Play(BasicAttackData[CurrentBasicAttackCombo].AnimMontage, 0, Delegate<>{},Delegate<>{},{this, &UCombatBaseComponent::BasicAttackEnded});
		}
	}
	else
	{
		CurrentBasicAttackCombo = 0;
		bIsBasicAttacking = false;
	}
}

void UCombatBaseComponent::HeavyAttack()
{
	bIsBasicAttacking = false;
}

void UCombatBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Started, this, &UCombatBaseComponent::BasicAttack);
	InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Trigger, this, &UCombatBaseComponent::HeavyAttack);
	InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Released, this, &UCombatBaseComponent::HeavyAttackMouseReleased);
}

void UCombatBaseComponent::SetBasicAttackData(const std::vector<std::string>& AttackMontageNames, const std::vector<FAttackData>& NewAttackData)
{
	if (AttackMontageNames.size() != NewAttackData.size())
	{
		assert(nullptr && "수가 다름");
	}

	size_t Size = NewAttackData.size();
	BasicAttackData.clear();
	BasicAttackData.resize(Size);
	for (size_t Index = 0; Index < Size; ++Index)
	{
		memcpy(&BasicAttackData[Index], &NewAttackData[Index], sizeof(FAttackData));

		AssetManager::GetAsyncAssetCache(AttackMontageNames[Index],[this, Index](std::shared_ptr<UObject> Object)
			{
				BasicAttackData[Index].AnimMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (!BasicAttackData[Index].AnimMontage)
				{
					MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", BasicAttackMontages (Index) Montage Not exist - "+ std::to_string(Index));
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
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

const FAttackData& UCombatBaseComponent::GetBasicAttackData(size_t Index)
{
	if (Index < 0 || Index >= BasicAttackData.size())
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "Wrong index");
		Index = 0;
	}

	return BasicAttackData[Index];
}
