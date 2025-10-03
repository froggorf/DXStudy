#include "CoreMinimal.h"
#include "USanhwaAnimInstance.h"

#include "Engine/AssetManager/AssetManager.h"
#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"

USanhwaAnimInstance::USanhwaAnimInstance()
{
	BS_LocomotionName = "BS_UE5MM_Locomotion";

	//for (int i = 0; i < 5; ++i)
	//{
	//	std::string Name = "AS_Sanhwa_BasicAttack" + std::to_string(i);
	//	AssetManager::GetAsyncAssetCache(Name,[this, i](std::shared_ptr<UObject> Object)
	//		{
	//			AS_Test[i] = std::dynamic_pointer_cast<UAnimSequence>(Object);

	//		});	
	//}
	
}

void USanhwaAnimInstance::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();



	Delegate<> Attack0Delegate;
	Attack0Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack0);
	NotifyEvent["SH_Attack0"] = Attack0Delegate;

	Delegate<> Attack1Delegate;
	Attack1Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack1);
	NotifyEvent["SH_Attack1"] = Attack1Delegate;

	Delegate<> Attack2Delegate;
	Attack2Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack2);
	NotifyEvent["SH_Attack2"] = Attack2Delegate;

	Delegate<> Attack3Delegate;
	Attack3Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack3);
	NotifyEvent["SH_Attack3"] = Attack3Delegate;

	Delegate<> Attack4_FloatDelegate;
	Attack4_FloatDelegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack4_Float);
	NotifyEvent["SH_Attack4_Float"] = Attack4_FloatDelegate;

	Delegate<> Attack4_AttackDelegate;
	Attack4_AttackDelegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack4_Attack);
	NotifyEvent["SH_Attack4_Attack"] = Attack4_AttackDelegate;

	Delegate<> HideDelegate;
	HideDelegate.Add([this]()
		{
			//this->SetSkeletalMeshVisibility(false);
			MY_LOG("Hide", EDebugLogLevel::DLL_Warning, "HIDEHIDEHIDE");
		});
	NotifyEvent["Hide"] = HideDelegate;

	Delegate<> ShowDelegate;
	ShowDelegate.Add([this]()
		{
			//this->SetSkeletalMeshVisibility(true);
			MY_LOG("Show", EDebugLogLevel::DLL_Warning, "SHOWSHOWSHOW");
		});
	NotifyEvent["Show"] = ShowDelegate;
}

void USanhwaAnimInstance::BeginPlay()
{
	UMyGameAnimInstanceBase::BeginPlay();
}

bool USanhwaAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK();
}



void USanhwaAnimInstance::MotionWarping_BasicAttack0()
{
	SetWarping_BasicAttack(0, MyGameCharacter->GetActorForwardVector());
}

void USanhwaAnimInstance::MotionWarping_BasicAttack1()
{
	SetWarping_BasicAttack(1, MyGameCharacter->GetActorForwardVector());
}

void USanhwaAnimInstance::MotionWarping_BasicAttack2()
{
	SetWarping_BasicAttack(2, MyGameCharacter->GetActorForwardVector());
}


void USanhwaAnimInstance::MotionWarping_BasicAttack3()
{
	SetWarping_BasicAttack(3, MyGameCharacter->GetActorForwardVector());
}

void USanhwaAnimInstance::MotionWarping_BasicAttack4_Float()
{
	// 특정한 동작을 위해 기존 SetWarping_BasicAttack을 사용하지 않음
	if (!SetMotionWarping())
	{
		return;
	}
	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	const std::shared_ptr<USanhwaCombatComponent>& CombatComp = std::static_pointer_cast<USanhwaCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!MotionWarpingComp || !CombatComp)
	{
		return;
	}

	XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
	float MoveDistance =CombatComp->GetAttack4_FloatMoveDistance();
	float Attack4MoveDistance = MoveDistance + CombatComp->GetAttack4_AttackMoveDistance();
	float FloatingDistance = CombatComp->GetAttack4_FloatingDistance();
	// 하드코딩, TODO: 대응하는 값으로 바꿀 수 있도록 조정해봐야함
	float MoveTime = 7* (1.0f/30);

	XMFLOAT3 WarpingTargetPos;
	if (const AActor* NearestEnemy = CombatComp->FindNearestEnemy(CurActorLocation, Attack4MoveDistance + 100, {}))
	{
		XMFLOAT3 EnemyLocation = NearestEnemy->GetActorLocation();
		WarpingTargetPos = GetWarpingPositionToEnemy(CurActorLocation, EnemyLocation, MoveDistance+30);
		WarpingTargetPos.y += FloatingDistance;
		MyGameCharacter->SetActorRotation(MyMath::GetRotationQuaternionToActor(CurActorLocation, EnemyLocation));

		Attack4_AttackTargetPos = EnemyLocation;
	}
	else
	{
		XMFLOAT3 ForwardVec = MyGameCharacter->GetActorForwardVector();
		WarpingTargetPos = CurActorLocation + ForwardVec * MoveDistance + XMFLOAT3{0,FloatingDistance , 0};
		Attack4_AttackTargetPos = CurActorLocation + ForwardVec * Attack4MoveDistance;
	}

	MotionWarpingComp->SetTargetLocation(WarpingTargetPos, MoveTime);
}

void USanhwaAnimInstance::MotionWarping_BasicAttack4_Attack()
{
	// 특정한 동작을 위해 기존 SetWarping_BasicAttack을 사용하지 않음
	// 특정한 동작을 위해 기존 SetWarping_BasicAttack을 사용하지 않음
	if (!SetMotionWarping())
	{
		return;
	}
	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	const std::shared_ptr<USanhwaCombatComponent>& CombatComp = std::static_pointer_cast<USanhwaCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!MotionWarpingComp || !CombatComp)
	{
		return;
	}

	// 하드코딩, TODO: 대응하는 값으로 바꿀 수 있도록 조정해봐야함
	float MoveTime = 38 * (1.0f/30);
	MotionWarpingComp->SetTargetLocation(Attack4_AttackTargetPos, MoveTime);
}

void USanhwaAnimInstance::SetSkeletalMeshVisibility(bool NewVisibility)
{
	USkeletalMeshComponent* SkeletalMesh = GetSkeletalMeshComponent();
	if (SkeletalMesh)
	{
		//SkeletalMesh->
	}
}


void USanhwaAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);
}

