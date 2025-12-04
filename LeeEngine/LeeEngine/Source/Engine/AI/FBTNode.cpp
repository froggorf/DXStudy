#include "CoreMinimal.h"
#include "FBTNode.h"

#include "Engine/Class/Framework/ACharacter.h"
#include "Engine/RenderCore/EditorScene.h"

bool FBTDecorator::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	return true;
}

EBTNodeResult FBTNode::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
    for (const std::shared_ptr<FBTDecorator>& Decorator : Decorators)
    {
	    if (!Decorator->Eval(BlackBoard))
	    {
		    return EBTNodeResult::Fail;
	    }
    }

    if (Event)
    {
	    Event->ExecuteEvent(BlackBoard);
    }

    return EBTNodeResult::Success;
}

UINT FBTNode::FindChildIndex(const FBTNode* ChildNode)
{
	for (size_t i = 0; i < Children.size(); ++i)
	{
		if (Children[i].get() == ChildNode)
		{
			return static_cast<UINT>(i);
		}
	}

	return -1;
}

void FBTNode::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
    for (size_t i = 0; i < Decorators.size(); ++i)
    {
	    Decorators[i]->OnEnterNode(BlackBoard);
    }
}


EBTNodeResult FBTCompositeNode::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
    if (FBTNode::Tick(DeltaSeconds, BlackBoard) == EBTNodeResult::Fail)
    {
        return EBTNodeResult::Fail;
    }

    // 자식 노드의 실행 자체는 UBehaviorTree::ExecuteNode 를 통해 실행
    return EBTNodeResult::Success;
}

void FBTSequencer::OnChildExecutionFinish(float DeltaSeconds, FBTNode* ChildNode, EBTNodeResult Result)
{
    const std::shared_ptr<FBTNode>& Parent = GetParent();
    const UINT ChildIndex = FindChildIndex(ChildNode);

    // 잘못된 노드
    if (ChildIndex == -1 || (!bIsRootNode && !Parent))
    {
        MY_LOG("Log", EDebugLogLevel::DLL_Error, GetFunctionName + "Not Valid");
        return;
    }

    // 자식 노드가 실패하면 시퀀서도 실패
    if (Result == EBTNodeResult::Fail)
    {
        if (Parent)
        {
			Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Fail);
        }
        return;
    }

    // 자식 노드가 Running이면 시퀀서도 Running
    if (Result == EBTNodeResult::Running)
    {
        if (Parent)
        {
			Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Running);
        }
        return;
    }

    // 자식 노드가 성공했고 마지막 자식이 아니면 다음 자식 실행
    if (ChildIndex < static_cast<UINT>(Children.size()) - 1)
    {
        // 다음 자식 노드를 CurrentRunningNode로 설정하고 다음 틱에서 실행되도록 함
        if (const std::shared_ptr<UBehaviorTree>& BT = OwnerTree.lock())
        {
            Children[ChildIndex+1]->OnEnterNode(BT->GetBlackBoard());
            BT->SetCurrentRunningNode(Children[ChildIndex + 1]);
            if (Parent)
            {
				Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Running);
            }
        }
    }
    // 모든 자식이 성공했으면 시퀀서도 성공
    else
    {
        if (Parent)
        {
			Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Success);
        }
    }
}

void FBTSelector::OnChildExecutionFinish(float DeltaSeconds, FBTNode* ChildNode, EBTNodeResult Result)
{
    const std::shared_ptr<FBTNode>& Parent = GetParent();
    const UINT ChildIndex = FindChildIndex(ChildNode);

    // 잘못된 노드
    // 루트일 경우 정상적으로 다음 노드들을 수행할 수 있게 하기 위해 Parent를 매번 감싸서 처리함
    if (ChildIndex == -1 || (!bIsRootNode && !Parent))
    {
        MY_LOG("Log", EDebugLogLevel::DLL_Error, GetFunctionName + "Not Valid");
        return;
    }

    // 자식 노드가 성공/Running하면 셀렉터이므로 부모에게 성공을 알리고 종료
    if (Result != EBTNodeResult::Fail)
    {
        if (Parent)
        {
            Parent->OnChildExecutionFinish(DeltaSeconds, this, Result);    
        }
        return;
    }

    // 마지막 자식이 아니면 다음 자식 실행
    if (ChildIndex < static_cast<UINT>(Children.size()) - 1)
    {
        // 다음 자식 노드를 CurrentRunningNode로 설정하고 다음 틱에서 실행되도록 함
        if (const std::shared_ptr<UBehaviorTree>& BT = OwnerTree.lock())
        {
            Children[ChildIndex+1]->OnEnterNode(BT->GetBlackBoard());
            BT->SetCurrentRunningNode(Children[ChildIndex + 1]);
            if (Parent)
            {
				Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Running);
            }
        }
    }
    // 모든 자식이 실패했으니 Selector도 실패
    else
    {
        if (Parent)
        {
			Parent->OnChildExecutionFinish(DeltaSeconds, this, EBTNodeResult::Fail);
        }
    }
}

void UBehaviorTree::Register()
{
	UObject::Register();

    OnConstruct();
}

void UBehaviorTree::OnConstruct()
{
    // 블랙보드 생성
    BlackBoard = std::make_shared<FBlackBoard>();
    BlackBoard->OnBlackBoardValueChanged.Add(this, &UBehaviorTree::ClearCurrentRunningNode);

    // 루트 노드 생성
    //BTRoot = bIsRootSelector ? std::make_shared<FBTSelector>() : std::make_shared<FBTSequencer>();
    if (bIsRootSelector)
    {
	    BTRoot = std::make_shared<FBTSelector>();
    }
    else
    {
	    BTRoot = std::make_shared<FBTSequencer>();
    }
    BTRoot->SetRootNode();
    BTRoot->SetOwnerTree(shared_from_this());
}


EBTNodeResult UBehaviorTree::Tick(float DeltaSeconds)
{
    if (!BTRoot)
        return EBTNodeResult::Fail;

    // 현재 실행 중인 노드가 있으면 계속 실행한 뒤
    // 성공/실패 시 부모에게 알린다
    if (CurrentRunningNode)
    {
        EBTNodeResult Result = CurrentRunningNode->Tick(DeltaSeconds, BlackBoard);
        if (Result != EBTNodeResult::Running)
        {
	        const std::shared_ptr<FBTNode>& Parent = CurrentRunningNode->GetParent();
            std::shared_ptr<FBTNode> LastRunningNode = CurrentRunningNode;
            CurrentRunningNode = nullptr;
            if (Parent)
            {
	            Parent->OnChildExecutionFinish(DeltaSeconds, LastRunningNode.get(), Result);
            }
        }

        return Result;
    }
    // 실행 중인 노드가 없으면 루트부터 시작
    else
    {
        return ExecuteNode(DeltaSeconds,BTRoot);
    }
}

void UBehaviorTree::OnTreeCompleted(EBTNodeResult Result)
{

}

void UBehaviorTree::SetCurrentRunningNode(const std::shared_ptr<FBTNode>& NewRunningNode)
{
    if (NewRunningNode->IsCompositeNode())
    {
		ExecuteNode(GEngine->GetDeltaSeconds(), NewRunningNode);	    
    }
    else
    {
	    CurrentRunningNode = NewRunningNode;
    }
}


EBTNodeResult UBehaviorTree::ExecuteNode(float DeltaSeconds, const std::shared_ptr<FBTNode>& Node)
{
    // 처음 노드를 수행하는 것이므로 다음과 같이 OnEnterNode를 호출
    // 해당 함수에선 초기값 설정이나 사전 작업이 발생
    Node->OnEnterNode(BlackBoard);

	if (Node->Tick(DeltaSeconds, BlackBoard) == EBTNodeResult::Fail)
	{
        const std::shared_ptr<FBTNode>& Parent = Node->GetParent();
        if (Parent)
        {
            Parent->OnChildExecutionFinish(DeltaSeconds, Node.get(), EBTNodeResult::Fail);
        }
		return EBTNodeResult::Fail;
	}


    // 컴포짓 노드일 경우 자식이 있으면 0번째 자식을 실행시킴
    // 0번째 자식에서 쭉 나아가 자식이 Task 일 경우 성공하거나 실패 결과를 OnChildExecutionFinish를 통해 건네주어 다음 작업을 처리
    if (Node->IsCompositeNode())
    {
        EBTNodeResult Result = EBTNodeResult::Fail;
        if (!Node->Children.empty())
        {
            Result = ExecuteNode(DeltaSeconds, Node->Children[0]);
        }
        return Result;
    }

    // Task 일 시
    EBTNodeResult Result = Node->Tick(DeltaSeconds, BlackBoard);
    // Running 상태면 현재 실행 중인 노드로 설정
    if (Result == EBTNodeResult::Running)
    {
        CurrentRunningNode = Node;
    }
    // 완료일 경우엔 부모에게 결과 전달
    else
    {
        if (const std::shared_ptr<FBTNode>& Parent = Node->GetParent())
        {
            Parent->OnChildExecutionFinish(DeltaSeconds, Node.get(), Result);
        }
    }

    return Result;
}







// ====================== FBT Task 자식들 ======================

void FBTTask_PlayAnimation::OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
    FBTTask::OnEnterNode(BlackBoard);
    bStartAnimation = false;
    bAnimationFinish = false;

    Character = std::dynamic_pointer_cast<ACharacter>(BlackBoard->GetValue<FBlackBoardValueType_Object>("Owner"));
    PlayingAnimation = std::dynamic_pointer_cast<UAnimMontage>(BlackBoard->GetValue<FBlackBoardValueType_Object>(AnimationBlackBoardKeyName));
}

EBTNodeResult FBTTask_PlayAnimation::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
    if (FBTTask::Tick(DeltaSeconds , BlackBoard) == EBTNodeResult::Fail)
    {
        return EBTNodeResult::Fail;		
    }

    std::shared_ptr<ACharacter> OwnerCharacterShared = Character.lock();
    if (!OwnerCharacterShared)
    {
        MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid OwningActor");
        return EBTNodeResult::Fail;
    }

    std::shared_ptr<UAnimMontage> PlayingAnimationShared = PlayingAnimation.lock();
    if (!PlayingAnimationShared)
    {
        MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "Not valid Animation");
        return EBTNodeResult::Fail;
    }

    if (!bStartAnimation)
    {
        bStartAnimation = true;
        if (const std::shared_ptr<UAnimInstance>& AnimInstance = OwnerCharacterShared->GetAnimInstance())
        {
            AnimInstance->Montage_Play(PlayingAnimationShared, 0, {this, &FBTTask_PlayAnimation::FinishAnimation});
        }
    }

    return bAnimationFinish? EBTNodeResult::Success : EBTNodeResult::Running;
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
