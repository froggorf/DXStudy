#include "CoreMinimal.h"
#include "USanhwaAnimInstance.h"

#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/Sanhwa/USanhwaUltimateComponent.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"

USanhwaAnimInstance::USanhwaAnimInstance()
{
	BS_LocomotionName = "BS_Sanhwa_Locomotion";
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

	Delegate<> Attack4_FloatingDelegate{this, &USanhwaAnimInstance::MotionWarping_BasicAttack4_Floating};
	NotifyEvent["SH_Attack4_Floating"] = Attack4_FloatingDelegate;

	Delegate<> HideDelegate;
	HideDelegate.Add([this]()
		{
			this->SetSkeletalMeshVisibility(false);
			
		});
	NotifyEvent["Hide"] = HideDelegate;

	Delegate<> ShowDelegate;
	ShowDelegate.Add([this]()
		{
			this->SetSkeletalMeshVisibility(true);
		});
	NotifyEvent["Show"] = ShowDelegate;


	Delegate<> SkillAttackDelegate = {this, &USanhwaAnimInstance::SanhwaSkillAttack};
	NotifyEvent["SH_Skill"] = SkillAttackDelegate;

	Delegate<> BasicAttack0Delegate = {this, &USanhwaAnimInstance::BasicAttack0};
	NotifyEvent["BasicAttack0"] = BasicAttack0Delegate;

	Delegate<> BasicAttack1Delegate = {this, &USanhwaAnimInstance::BasicAttack1};
	NotifyEvent["BasicAttack1"] = BasicAttack1Delegate;

	Delegate<> BasicAttack2Delegate = {this, &USanhwaAnimInstance::BasicAttack2};
	NotifyEvent["BasicAttack2"] = BasicAttack2Delegate;

	Delegate<> BasicAttack3Delegate = {this, &USanhwaAnimInstance::BasicAttack3};
	NotifyEvent["BasicAttack3"] = BasicAttack3Delegate;

	Delegate<> FinalAttackDelegate = {this, &USanhwaAnimInstance::BasicAttack4};
	NotifyEvent["BasicAttack4"] = FinalAttackDelegate;

	Delegate<> HeavyAttackMoveDelegate = {this, &USanhwaAnimInstance::MotionWarping_HeavyAttack_Move};
	NotifyEvent["SH_Heavy_Move"] = HeavyAttackMoveDelegate;

	Delegate<> HeavyAttackStayDelegate = {this, &USanhwaAnimInstance::MotionWarping_HeavyAttack_Stay};
	NotifyEvent["SH_Heavy_Stay"] = HeavyAttackStayDelegate;

	Delegate<> UltimateCameraChangeDelegate = {this, &USanhwaAnimInstance::Ultimate_ChangeCameraToNormal};
	NotifyEvent["Camera"] = UltimateCameraChangeDelegate;

	Delegate<> UltAttackDelegate = {this, &USanhwaAnimInstance::UltAttack};
	NotifyEvent["UltAttack"] = UltAttackDelegate;

	Delegate<> SkillAttack = {this, &USanhwaAnimInstance::SkillAttack};
	NotifyEvent["SkillAttack"] = SkillAttack;

	Delegate<> HeavyAttack = {this, &USanhwaAnimInstance::HeavyAttack};
	NotifyEvent["HeavyAttack"] = HeavyAttack;	
}

void USanhwaAnimInstance::BeginPlay()
{
	UMyGameAnimInstanceBase::BeginPlay();
}

bool USanhwaAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK() && AS_Falling && AS_JumpStart && AS_Land;
}



void USanhwaAnimInstance::MotionWarping_BasicAttack0()
{
	constexpr size_t BasicAttackIndex = 0;
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	const FAttackData& AttackData = CombatComp->GetBasicAttackData(BasicAttackIndex);

	float MoveDistance = AttackData.MoveDistance;
	float MoveTime = AttackData.AnimMontage->GetPlayLength();
	SetWarpingTarget(MyGameCharacter->GetActorForwardVector(), MoveDistance, MoveTime);
}

void USanhwaAnimInstance::MotionWarping_BasicAttack1()
{
	constexpr size_t BasicAttackIndex = 1;
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	const FAttackData& AttackData = CombatComp->GetBasicAttackData(BasicAttackIndex);

	float MoveDistance = AttackData.MoveDistance;
	float MoveTime = AttackData.AnimMontage->GetPlayLength();
	SetWarpingTarget(MyGameCharacter->GetActorForwardVector(), MoveDistance, MoveTime);
}

void USanhwaAnimInstance::MotionWarping_BasicAttack2()
{
	constexpr size_t BasicAttackIndex = 2;
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	const FAttackData& AttackData = CombatComp->GetBasicAttackData(BasicAttackIndex);

	float MoveDistance = AttackData.MoveDistance;
	float MoveTime = AttackData.AnimMontage->GetPlayLength();
	SetWarpingTarget(MyGameCharacter->GetActorForwardVector(), MoveDistance, MoveTime);
}


void USanhwaAnimInstance::MotionWarping_BasicAttack3()
{
	constexpr size_t BasicAttackIndex = 3;
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	const FAttackData& AttackData = CombatComp->GetBasicAttackData(BasicAttackIndex);

	float MoveDistance = AttackData.MoveDistance;
	float MoveTime = AttackData.AnimMontage->GetPlayLength();
	SetWarpingTarget(MyGameCharacter->GetActorForwardVector(), MoveDistance, MoveTime);
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
	
	float Attack4MoveDistance = MoveDistance + CombatComp->GetHeavyAttackData(0).MoveDistance;
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

		float ToEnemyDistance = MyMath::GetDistance(CurActorLocation, EnemyLocation);
		XMFLOAT3 ToEnemyVector = MyMath::GetDirectionUnitVector(CurActorLocation, EnemyLocation);

		Attack4_AttackTargetPos = CurActorLocation + ToEnemyVector* std::min(max(ToEnemyDistance - 150, 0), Attack4MoveDistance);
	}
	else
	{
		XMFLOAT3 ForwardVec = MyGameCharacter->GetActorForwardVector();
		WarpingTargetPos = CurActorLocation + ForwardVec * MoveDistance + XMFLOAT3{0,FloatingDistance , 0};
		Attack4_AttackTargetPos = CurActorLocation + ForwardVec * Attack4MoveDistance;
	}

	MotionWarpingComp->SetTargetLocation(WarpingTargetPos, MoveTime);
}

void USanhwaAnimInstance::MotionWarping_BasicAttack4_Floating()
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
	MotionWarpingComp->SetTargetLocation(CurActorLocation, 10.0f);
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

void USanhwaAnimInstance::MotionWarping_HeavyAttack_Move()
{
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
	float MoveTime = 23 * (1.0f/30);
	XMFLOAT3 CurLocation = MyGameCharacter->GetActorLocation();
	XMFLOAT3 ForwardVector = MyGameCharacter->GetActorForwardVector();
	
	MotionWarpingComp->SetTargetLocation(CurLocation + ForwardVector * CombatComp->GetHeavyAttackMoveDistance(), MoveTime);
	MotionWarpingComp->SetbIsSetPosition(true);
}

void USanhwaAnimInstance::MotionWarping_HeavyAttack_Stay()
{
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
	float MoveTime = 22 * (1.0f/30);
	XMFLOAT3 CurLocation = MyGameCharacter->GetActorLocation();

	MotionWarpingComp->SetTargetLocation(CurLocation, MoveTime);

}

void USanhwaAnimInstance::Ultimate_ChangeCameraToNormal()
{
	if (!MyGameCharacter)
	{
		return;
	}

	MyGameCharacter->ChangeToNormalCamera(0.5f);
}

void USanhwaAnimInstance::BasicAttack0()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetBasicAttackData(0), "SH_BasicAttack");
}

void USanhwaAnimInstance::BasicAttack1()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetBasicAttackData(1), "SH_BasicAttack");
}

void USanhwaAnimInstance::BasicAttack2()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetBasicAttackData(2), "SH_BasicAttack");
}

void USanhwaAnimInstance::BasicAttack3()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetBasicAttackData(3), "SH_BasicAttack");
}

void USanhwaAnimInstance::BasicAttack4()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetBasicAttackData(4), "SH_BasicAttack");
	{
		std::static_pointer_cast<USanhwaCombatComponent>(CombatComp)->Attack4Success();
	}
}

void USanhwaAnimInstance::SkillAttack()
{
	if (const std::shared_ptr<USkillBaseComponent>& SkillComp = MyGameCharacter->GetSkillComponent())
	{
		SkillComp->ApplySkillAttack();
		MyGameCharacter->ApplyDamageToEnemy_Range(SkillComp->GetSkillAttackData(0), "SH_SkillAttack");
	}
}

void USanhwaAnimInstance::HeavyAttack()
{
	if (!MyGameCharacter)
	{
		return;
	}
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	MyGameCharacter->ApplyDamageToEnemy_Range(CombatComp->GetHeavyAttackData(0), "BreakIce");
}

void USanhwaAnimInstance::UltAttack()
{
	if (const std::shared_ptr<UUltimateBaseComponent>& UltComp = MyGameCharacter->GetUltimateComponent())
	{
		UltComp->ApplyUltimateAttack();
		MyGameCharacter->ApplyDamageToEnemy_Range(UltComp->GetSkillAttackData(0), "SH_UltAttack");
	}
}

void USanhwaAnimInstance::SanhwaSkillAttack()
{
	const std::shared_ptr<USkillBaseComponent>& SkillComp = MyGameCharacter->GetSkillComponent();

	const FAttackData& SkillAttackData = SkillComp->GetSkillAttackData(0);
	float MoveDistance = SkillAttackData.MoveDistance;
	float MoveTime = SkillAttackData.AnimMontage->GetPlayLength();
	SetWarpingTarget(MyGameCharacter->GetActorForwardVector(), MoveDistance, MoveTime);
}

void USanhwaAnimInstance::SetSkeletalMeshVisibility(bool NewVisibility)
{
	USkeletalMeshComponent* SkeletalMesh = GetSkeletalMeshComponent();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetVisibility(NewVisibility);
	}
}


void USanhwaAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);
}

