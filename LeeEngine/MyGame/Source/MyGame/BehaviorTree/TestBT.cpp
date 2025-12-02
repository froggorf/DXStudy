#include "CoreMinimal.h"
#include "TestBT.h"

#include "Engine/GameFramework/AActor.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Actor/Decal/ARangeDecalActor.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"

//bool FBTDecorator_BlackBoardValueCheck::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
//{
//	void* Value;
//	if (!BlackBoard->GetBlackBoardValue(BlackBoardKey, &Value, ValueType))
//	{
//		return false;
//	}
//
//	switch (ValueCheckType)
//	{
//	case EBlackBoardValueCheckType::Equal:
//		if (ValueType != )
//	break;
//	case EBlackBoardValueCheckType::NotEqual:
//		break;
//	case EBlackBoardValueCheckType::Less:
//		break;
//	case EBlackBoardValueCheckType::LessOrEqual:
//		break;
//	case EBlackBoardValueCheckType::Greater:
//		break;
//	case EBlackBoardValueCheckType::GreaterOrEqual:
//		break;
//	case EBlackBoardValueCheckType::IsTrue:
//		if (ValueType != EBlackBoardValueType::Bool)
//		{
//			return false;
//		}
//		return *static_cast<bool*>(Value);
//	case EBlackBoardValueCheckType::IsFalse:
//		if (ValueType != EBlackBoardValueType::Bool)
//		{
//			return false;
//		}
//		return !*static_cast<bool*>(Value);
//	default:
//		return false;
//	}
//	return true;
//
//}

bool FBTDecorator_MoveCheck::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	return BlackBoard->GetValue<FBlackBoardValueType_Bool>("MoveMode");
}

void FBTDecorator_MoveCheck::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTDecorator::OnEnterNode(BlackBoard);

	BlackBoard->AddBlackBoardValueChangeObserver("MoveMode");
}

void FBTTask_MoveToAnyWhere::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);

	CurrentMoveTime = 0.0f;

	OwningActor.reset();
	OwningActor = std::dynamic_pointer_cast<AActor>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
	if (OwningActor.expired())
	{
		return;
	}

	std::shared_ptr<AActor> SharedOwningActor = OwningActor.lock();
	StartLocation = SharedOwningActor->GetActorLocation();

	XMFLOAT3 AddVal = XMFLOAT3{ MyMath::FRandRange(MoveMin.x, MoveMax.x), MyMath::FRandRange(MoveMin.y, MoveMax.y), MyMath::FRandRange(MoveMin.z, MoveMax.z) };;
	if (MyMath::RandBool()) AddVal.x *= -1;
	if (MyMath::RandBool()) AddVal.y *= -1;
	if (MyMath::RandBool()) AddVal.z *= -1;

	TargetLocation = StartLocation + AddVal; 
	
}

EBTNodeResult FBTTask_MoveToAnyWhere::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;	
	}
	std::shared_ptr<AActor> SharedOwningActor = OwningActor.lock();
	if (!SharedOwningActor)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor");
		return EBTNodeResult::Fail;
	}


	CurrentMoveTime = std::min(MoveTime, CurrentMoveTime + DeltaSeconds);

	XMFLOAT3 NewLocation = MyMath::Lerp(StartLocation, TargetLocation, CurrentMoveTime / MoveTime);
	SharedOwningActor->SetActorLocation_Teleport(NewLocation);

	if (CurrentMoveTime >= MoveTime)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		return EBTNodeResult::Running;
	}
	
}

void FBTTask_MoveToPlayer::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);
	
	OwningActor = std::dynamic_pointer_cast<AEnemyBase>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
	if (OwningActor.expired())
	{
		return;
	}

	Player = std::dynamic_pointer_cast<AActor>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Player"));
}

EBTNodeResult FBTTask_MoveToPlayer::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;		
	}

	std::shared_ptr<AActor> SharedPlayer = Player.lock();
	std::shared_ptr<AEnemyBase> Owner = OwningActor.lock();
	if (!SharedPlayer || !Owner)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor / SharedPlayer");
		return EBTNodeResult::Fail;
	}

	float Distance = MyMath::GetDistance(Owner->GetActorLocation(), SharedPlayer->GetActorLocation());

	if (Distance < Owner->GetCapsuleComponent()->GetRadius() * 2 + 50.0f)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		Owner->AddMovementInput(MyMath::GetDirectionUnitVector(Owner->GetActorLocation(), SharedPlayer->GetActorLocation()), 1);
		return EBTNodeResult::Running;
	}
	
}

void FBTTask_EnemyBasicAttack::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);
	bAttackFinish = false;
	bDoAttack = false;

	// Note: 해당 값은 노드가 살아있는 동안에 계속해서 존재해야 하므로,
	// 값을 초기화하지 말아야함
	//LastAttackTime = 0.0f;

	OwningActor = std::dynamic_pointer_cast<AEnemyBase>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
	if (OwningActor.expired())
	{
		return;
	}

	Player = std::dynamic_pointer_cast<AActor>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Player"));
}

EBTNodeResult FBTTask_EnemyBasicAttack::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;		
	}

	// 공격 쿨타임이 지나지 않았으면 Fail
	if (GEngine->GetTimeSeconds() < LastAttackTime + CoolDownTime)
	{
		return EBTNodeResult::Fail;
	}

	std::shared_ptr<AActor> SharedPlayer = Player.lock();
	std::shared_ptr<AEnemyBase> Owner = OwningActor.lock();
	if (!SharedPlayer || !Owner)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor / SharedPlayer");
		return EBTNodeResult::Fail;
	}

	if (!bDoAttack)
	{
		bDoAttack = true;
		Owner->PlayAttackMontage(Delegate<>{this, &FBTTask_EnemyBasicAttack::FinishAttack});
		LastAttackTime = GEngine->GetTimeSeconds();
	}
	

	if (bAttackFinish)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		return EBTNodeResult::Running;
	}
}


void FBTTask_RunFromPlayer::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);

	OwningActor = std::dynamic_pointer_cast<AEnemyBase>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
	Player = std::dynamic_pointer_cast<AActor>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Player"));
	
}

EBTNodeResult FBTTask_RunFromPlayer::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;		
	}

	std::shared_ptr<AActor> SharedPlayer = Player.lock();
	std::shared_ptr<AEnemyBase> Owner = OwningActor.lock();
	if (!SharedPlayer || !Owner)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor / SharedPlayer");
		return EBTNodeResult::Fail;
	}

	float Distance = MyMath::GetDistance(Owner->GetActorLocation(), SharedPlayer->GetActorLocation());
	if (Distance > 400.0f)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		Owner->AddMovementInput(MyMath::GetDirectionUnitVector(SharedPlayer->GetActorLocation(), Owner->GetActorLocation()), 1);
		return EBTNodeResult::Running;
	}

}


void FBTTask_Wait::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);

	CurrentWaitTime = 0.0f;	
}

EBTNodeResult FBTTask_Wait::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;
	}

	CurrentWaitTime += DeltaSeconds;
	if (CurrentWaitTime < WaitTime)
	{
		return EBTNodeResult::Running;
	}
	else
	{
		return EBTNodeResult::Success;
	}
}

UTestBT::UTestBT()
{
	

}

void UTestBT::OnConstruct()
{
	UBehaviorTree::OnConstruct();

	// BlackBoard 값 설정해주고
	BlackBoard->SetValue<FBlackBoardValueType_Bool>("MoveMode", true);
	std::shared_ptr<AActor> Actor = nullptr;
	BlackBoard->SetValue<FBlackBoardValueType_Object>("Owner", Actor);
	BlackBoard->SetValue<FBlackBoardValueType_Object>("Player", Actor);
	BlackBoard->SetValue<FBlackBoardValueType_FLOAT>("HealthPercent", 100.0f);

	// 플레이어가 거리 300m 이내에 없다면,
		// 주변을 배회한다
		// 5초 기다린다.	
	{
		// 첫번째 시퀀서
		std::shared_ptr<FBTSequencer> MoveRandomSequence = std::make_shared<FBTSequencer>();
		BTRoot->AddChild(MoveRandomSequence);

		//// MoveMode 체크 데코레이터 추가
		//std::shared_ptr<FBTDecorator_MoveCheck> MoveCheck = std::make_shared<FBTDecorator_MoveCheck>();
		// 11.24 범용 데코레이터로 변경
		std::shared_ptr<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_Bool>> IsPlayerFar = std::make_shared<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_Bool>>();
		IsPlayerFar->Initialize("MoveMode", true, EBlackBoardValueCheckType::Equal);
		MoveRandomSequence->Decorators.emplace_back(IsPlayerFar);

		// 랜덤위치 이동 Task
		/*std::shared_ptr<FBTTask_MoveToAnyWhere> MoveToTask = std::make_shared<FBTTask_MoveToAnyWhere>();
		MoveRandomSequence->AddChild(MoveToTask);*/

		std::shared_ptr<FBTTask_Wait> WaitTask= std::make_shared<FBTTask_Wait>();
		WaitTask->SetWaitTime(0.5f);
		MoveRandomSequence->AddChild(WaitTask);
	}
	// 플레이어가 주변에 있다면
	{
		std::shared_ptr<FBTSelector> HPCheckSelector = std::make_shared<FBTSelector>();
		BTRoot->AddChild(HPCheckSelector);
		std::shared_ptr<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_Bool>> IsPlayerNear = std::make_shared<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_Bool>>();
		IsPlayerNear->Initialize("MoveMode", false, EBlackBoardValueCheckType::IsFalse);
		HPCheckSelector->Decorators.emplace_back(IsPlayerNear);
			

		// 체력이 20% 이상이면 시퀀서를 실행하는데
		{
			std::shared_ptr<FBTSequencer> AttackPlayerSequencer = std::make_shared<FBTSequencer>();
			HPCheckSelector->AddChild(AttackPlayerSequencer);
			std::shared_ptr<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_FLOAT>> HPCheck = std::make_shared<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_FLOAT>>();
			HPCheck->Initialize("HealthPercent", 20.0f, EBlackBoardValueCheckType::GreaterOrEqual);
			AttackPlayerSequencer->Decorators.emplace_back(HPCheck);
			{
				// 적 앞으로 다가간다
				std::shared_ptr<FBTTask_MoveToPlayer> MoveToPlayerTask = std::make_shared<FBTTask_MoveToPlayer>();
				AttackPlayerSequencer->AddChild(MoveToPlayerTask);

				// 적을 공격한다.
				std::shared_ptr<FBTTask_EnemyBasicAttack> EnemyBasicAttackTask = std::make_shared<FBTTask_EnemyBasicAttack>();
				AttackPlayerSequencer->AddChild(EnemyBasicAttackTask);
			}
		}

		// 20% 이하면 도망친다.
		{
			std::shared_ptr<FBTTask_RunFromPlayer> RunFromPlayerTask = std::make_shared<FBTTask_RunFromPlayer>();
			std::shared_ptr<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_FLOAT>> HPCheck = std::make_shared<FBTDecorator_BlackBoardValueCheck<FBlackBoardValueType_FLOAT>>();
			HPCheck->Initialize("HealthPercent", 20.0f, EBlackBoardValueCheckType::Less);
			RunFromPlayerTask->Decorators.emplace_back(HPCheck);
			HPCheckSelector->AddChild(RunFromPlayerTask);
		}


	}
	
}

void FBTTask_DragonSkillCharging::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);
	bStartCharge = false;
	CurrentChargingTime = 0.0f;

	const std::shared_ptr<ADragon>& Dragon = std::dynamic_pointer_cast<ADragon>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
	if (!Dragon)
	{
		return;
	}

	OwnerDragon = Dragon;

	FTransform SpawnTransform;
	SpawnTransform.Translation = Dragon->GetActorLocation();
	float Radius = Dragon->GetFlameSkillRadius() * 2;
	SpawnTransform.Scale3D = XMFLOAT3 {Radius, 5.0f, Radius};
	static std::vector<ECollisionChannel> CollisionChannels;
	if (CollisionChannels.empty())
	{
		CollisionChannels.reserve(static_cast<int>(ECollisionChannel::Count));
		for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			if (Channel == ECollisionChannel::Player || Channel == ECollisionChannel::Enemy)
			{
				continue;
			}
			CollisionChannels.emplace_back(Channel);
		}
	}
	FHitResult HitResult;
	XMFLOAT3 Start = SpawnTransform.Translation;
	XMFLOAT3 End = Start;
	End.y -= 1000.0f;
	if (GPhysicsEngine->LineTraceSingleByChannel(Start, End, CollisionChannels, HitResult))
	{
		SpawnTransform.Translation = HitResult.Location;
	}
	
	if (!ShowRangeActor)
	{
		ShowRangeActor = std::dynamic_pointer_cast<ARangeDecalActor>(GEngine->GetWorld()->SpawnActor("ARangeDecalActor", SpawnTransform));
	}

	
	ShowRangeActor->SetForward(Dragon->GetActorForwardVector())
					->SetHalfAngleDeg(Dragon->GetFlameSkillHalfAngle());
	
}

EBTNodeResult FBTTask_DragonSkillCharging::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;
	}

	const std::shared_ptr<ADragon>& Dragon = OwnerDragon.lock();
	if (!Dragon || !ShowRangeActor)
	{
		return EBTNodeResult::Fail;
	}

	// 차지 시작에 대한 함수를 실행해주고,
	if (!bStartCharge)
	{
		bStartCharge = true;
		if (!Dragon->StartSkillCharge())
		{
			return EBTNodeResult::Fail;
		}
	}

	// 범위 장판 갱신
	CurrentChargingTime += DeltaSeconds;
	ShowRangeActor->SetProgress(CurrentChargingTime / ChargingTime);

	if (CurrentChargingTime >= ChargingTime)
	{
		GEngine->GetWorld()->GetPersistentLevel()->DestroyActor(ShowRangeActor.get());
		ShowRangeActor.reset();

		return EBTNodeResult::Success;
	}
	else
	{
		return EBTNodeResult::Running;
	}
}

void FBTTask_DragonUseSkill::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	FBTTask::OnEnterNode(BlackBoard);
	bDoSkill = false;
	bSkillFinish = false;

	OwningDragon = std::dynamic_pointer_cast<ADragon>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
}

EBTNodeResult FBTTask_DragonUseSkill::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
	{
		return EBTNodeResult::Fail;		
	}


	std::shared_ptr<ADragon> Dragon = OwningDragon.lock();
	if (!Dragon)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor / SharedPlayer");
		return EBTNodeResult::Fail;
	}

	if (!bDoSkill)
	{
		bDoSkill = true;
		Dragon->SkillChargeEnd(Delegate<>{this, &FBTTask_DragonUseSkill::FinishSkill});
	}


	if (bSkillFinish)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		return EBTNodeResult::Running;
	}
}

UDragonBT::UDragonBT()
{
}

void UDragonBT::OnConstruct()
{
	UBehaviorTree::OnConstruct();

	BlackBoard->SetValue<FBlackBoardValueType_Object>("Owner", nullptr);

	std::shared_ptr<FBTSequencer> Sequencer = std::make_shared<FBTSequencer>();
	BTRoot->AddChild(Sequencer);

	{
		std::shared_ptr<FBTTask_DragonSkillCharging> ChargingSkillTask = std::make_shared<FBTTask_DragonSkillCharging>();
		ChargingSkillTask->SetChargingTime(5.0f);

		std::shared_ptr<FBTTask_DragonUseSkill> UseSkillTask = std::make_shared<FBTTask_DragonUseSkill>();

		Sequencer->AddChild(ChargingSkillTask);
		Sequencer->AddChild(UseSkillTask);
	}
	
}
