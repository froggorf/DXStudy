#include "CoreMinimal.h"
#include "TestBT.h"

#include "Engine/GameFramework/AActor.h"

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
	bool Value;
	if (!BlackBoard->GetBlackBoardValue("MoveMode", &Value, EBlackBoardValueType::Bool))
	{
		return false;
	}

	return Value;
}

void FBTTask_MoveToAnyWhere::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	CurrentMoveTime = 0.0f;

	OwningActor = nullptr;
	if (!BlackBoard->GetBlackBoardValue("Owner", &OwningActor, EBlackBoardValueType::Actor) || !OwningActor)
	{
		return;
	}

	StartLocation = OwningActor->GetActorLocation();

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

	if (!OwningActor)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor");
		return EBTNodeResult::Fail;
	}


	CurrentMoveTime = std::min(MoveTime, CurrentMoveTime + DeltaSeconds);

	XMFLOAT3 NewLocation = MyMath::Lerp(StartLocation, TargetLocation, CurrentMoveTime / MoveTime);
	OwningActor->SetActorLocation_Teleport(NewLocation);

	if (CurrentMoveTime >= MoveTime)
	{
		return EBTNodeResult::Success;
	}
	else
	{
		return EBTNodeResult::Running;
	}
	
}

void FBTTask_Wait::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
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
	bool InitMoveMode = true;
	BlackBoard->AddKeyValue("MoveMode", &InitMoveMode, EBlackBoardValueType::Bool);
	AActor* InitActor = nullptr;
	BlackBoard->AddKeyValue("Owner", InitActor, EBlackBoardValueType::Actor);

	// 첫번째 시퀀서
	std::shared_ptr<FBTSequencer> MoveRandomSequence = std::make_shared<FBTSequencer>();
	BTRoot->AddChild(MoveRandomSequence);

	// MoveMode 체크 데코레이터 추가
	std::shared_ptr<FBTDecorator_MoveCheck> MoveCheck = std::make_shared<FBTDecorator_MoveCheck>();
	MoveRandomSequence->Decorators.emplace_back(MoveCheck);

	// 랜덤위치 이동 Task
	std::shared_ptr<FBTTask_MoveToAnyWhere> MoveToTask = std::make_shared<FBTTask_MoveToAnyWhere>();
	MoveRandomSequence->AddChild(MoveToTask);

	std::shared_ptr<FBTTask_Wait> WaitTask= std::make_shared<FBTTask_Wait>();
	WaitTask->SetWaitTime(3.0f);
	MoveRandomSequence->AddChild(WaitTask);
}
