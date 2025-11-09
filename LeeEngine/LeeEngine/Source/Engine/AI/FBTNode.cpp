#include "CoreMinimal.h"
#include "FBTNode.h"

bool FBTDecorator::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	return true;
}

ETaskResult FBTNode::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	// 먼저 데코레이터가 모두 만족하는지 체크
	for (const std::shared_ptr<FBTDecorator>& Decorator : Decorators)
	{
		if (Decorator)
		{
			if (!Decorator->Eval(BlackBoard))
			{
				return ETaskResult::Fail;
			}
		}
	}

	// 이벤트가 있다면 이벤트 실행
	if (Event)
	{
		Event->Event(BlackBoard);
	}

	return ETaskResult::Success;
}


ETaskResult FBTSequencer::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if(FBTNode::Tick(DeltaSeconds, BlackBoard) != ETaskResult::Success)
	{
		return ETaskResult::Fail;
	}

	// 아래에 아무 노드도 없으면 실패를 반환하여
	// 다른 노드가 실행될 수 있도록 함
	if (Children.empty())
	{
		return ETaskResult::Fail;
	}

	for (const std::shared_ptr<FBTNode>& ChildNode : Children)
	{
		ETaskResult NodeResult = ChildNode->Tick(DeltaSeconds, BlackBoard);

		// 성공이 아님 -> 실패 / 실행중 을 반환
		if (NodeResult != ETaskResult::Success)
		{
			return NodeResult;
		}
	}

	// 현재 주어진 모든 노드들에 대해서 모두 성공한거니 성공을 반환
	return ETaskResult::Success;
}

ETaskResult FBTSelector::Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	if(FBTNode::Tick(DeltaSeconds, BlackBoard) != ETaskResult::Success)
	{
		return ETaskResult::Fail;
	}

	// 아래에 아무 노드도 없으면 실패를 반환하여
	// 다른 노드가 실행될 수 있도록 함
	if (Children.empty())
	{
		return ETaskResult::Fail;
	}

	for (const std::shared_ptr<FBTNode>& ChildNode : Children)
	{
		ETaskResult NodeResult = ChildNode->Tick(DeltaSeconds, BlackBoard);

		// 실패가 아님 -> 성공 / 실행중 을 반환
		if (NodeResult != ETaskResult::Fail)
		{
			return NodeResult;
		}
	}

	// 모든 노드들에 대해서 실패가 발생할 경우라면 실패를 반환해야함
	return ETaskResult::Fail;
}
