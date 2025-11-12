#include "CoreMinimal.h"
#include "TestBT.h"

#include "Engine/GameFramework/AActor.h"

bool FBTDecorator_InCenter::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	XMFLOAT3 Value;
	if (!BlackBoard->GetBlackBoardValue("InBox", &Value, EBlackBoardValueType::Float3))
	{
		return false;
	}

	AActor* Owner;
	if (!BlackBoard->GetBlackBoardValue("Owner", &Owner, EBlackBoardValueType::Actor))
	{
		return false;
	}

	float Distance = 100.0f;
	
	if (MyMath::GetDistance(Owner->GetActorLocation(), Value) < Distance)
	{
		return true;
	}

	return false;

}

EBTNodeResult FBTTask_MoveToAnyWhere::TickEveryFrame(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if (FBTTask::TickEveryFrame(DeltaSeconds , BlackBoard) != EBTNodeResult::Success)
	{
		return EBTNodeResult::Fail;
	}

	
}

UTestBT::UTestBT()
{
	// 블랙보드 생성
	BlackBoard = std::make_shared<FBlackBoard>();

	// 루트 노드로 Selector 생성
	std::shared_ptr<FBTSelector> RootNode = std::make_shared<FBTSelector>();

	// 첫 번째 서브트리: 중앙에서 벗어나기 (중앙에 있을 경우)
	std::shared_ptr<FBTSequencer> GoOutSequence = std::make_shared<FBTSequencer>();

	// HasTarget 데코레이터 추가
	std::shared_ptr<FBTDecorator_InCenter> IsInsideDecorator = std::make_shared<FBTDecorator_InCenter>();
	GoOutSequence->Decorators.emplace_back(IsInsideDecorator);

	// 추적 태스크 추가
	std::shared_ptr<FBTTask_MoveTo> MoveToTargetTask = std::make_shared<FBTTask_MoveTo>();
	MoveToTargetTask->SetTargetLocationKey("TargetLocation");
	ChaseSequence->Children.push_back(MoveToTargetTask);

	// 두 번째 서브트리: 순찰
	std::shared_ptr<FBTSequencer> PatrolSequence = std::make_shared<FBTSequencer>();

	// 순찰 태스크들 추가
	std::shared_ptr<FBTTask_GetNextPatrolPoint> GetPatrolPointTask = std::make_shared<FBTTask_GetNextPatrolPoint>();
	std::shared_ptr<FBTTask_MoveTo> MoveToPatrolPointTask = std::make_shared<FBTTask_MoveTo>();
	std::shared_ptr<FBTTask_Wait> WaitAtPatrolPointTask = std::make_shared<FBTTask_Wait>(2.0f);

	PatrolSequence->Children.push_back(GetPatrolPointTask);
	PatrolSequence->Children.push_back(MoveToPatrolPointTask);
	PatrolSequence->Children.push_back(WaitAtPatrolPointTask);

	// 루트 노드에 서브트리 추가 (우선순위 순서대로)
	RootNode->Children.push_back(ChaseSequence);
	RootNode->Children.push_back(PatrolSequence);

	// 부모-자식 관계 설정
	ChaseSequence->SetParent(RootNode);
	PatrolSequence->SetParent(RootNode);

	MoveToTargetTask->SetParent(ChaseSequence);

	GetPatrolPointTask->SetParent(PatrolSequence);
	MoveToPatrolPointTask->SetParent(PatrolSequence);
	WaitAtPatrolPointTask->SetParent(PatrolSequence);

	// 비헤이비어 트리 설정
	BehaviorTree->SetBTRoot(RootNode);
	BehaviorTree->SetBlackBoard(BlackBoard);

	// AI 컨트롤러에 비헤이비어 트리 설정
	AAIController* AIController = new AAIController();
	AIController->SetBehaviorTree(BehaviorTree);
}
