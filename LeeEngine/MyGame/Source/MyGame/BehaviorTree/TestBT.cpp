#include "CoreMinimal.h"
#include "TestBT.h"

#include "Engine/GameFramework/AActor.h"
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
	if (Distance < 75.0f)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		Owner->AddMovementInput(MyMath::GetDirectionUnitVector(Owner->GetActorLocation(), SharedPlayer->GetActorLocation()), 1);
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
		std::shared_ptr<FBTTask_MoveToAnyWhere> MoveToTask = std::make_shared<FBTTask_MoveToAnyWhere>();
		MoveRandomSequence->AddChild(MoveToTask);

		std::shared_ptr<FBTTask_Wait> WaitTask= std::make_shared<FBTTask_Wait>();
		WaitTask->SetWaitTime(3.0f);
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
				// TODO: 만들 예정
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
