#pragma once
#include "Engine/AI/FBTNode.h"

class FBTDecorator_InCenter : public FBTDecorator
{
	bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
};

class FBTTask_MoveToAnyWhere : public FBTTask
{
	EBTNodeResult TickEveryFrame(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;
};

class UTestBT : public UBehaviorTree
{
	MY_GENERATE_BODY(UTestBT)
public:
	UTestBT();
	~UTestBT() override = default;
};