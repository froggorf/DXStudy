#pragma once
#include "Engine/AI/FBTNode.h"

//enum class EBlackBoardValueCheckType
//{
//	Equal, NotEqual,
//	Less, LessOrEqual,
//	Greater, GreaterOrEqual,
//	IsTrue, IsFalse,
//};
//class FBTDecorator_BlackBoardValueCheck : public FBTDecorator
//{
//public:
//	bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
//
//	void Initialize(const std::string& TargetKeyName, const void* TargetValue, EBlackBoardValueType ValueType, EBlackBoardValueCheckType ValueCheckType)
//	{
//		BlackBoardKey = TargetKeyName;
//		this->TargetValue = TargetValue;
//		this->ValueType = ValueType;
//		this->ValueCheckType = ValueCheckType;
//	}
//
//private:
//	std::string BlackBoardKey;
//	const void* TargetValue = nullptr;
//	EBlackBoardValueType ValueType = EBlackBoardValueType::Int;
//	EBlackBoardValueCheckType ValueCheckType = EBlackBoardValueCheckType::Equal;
//
//};

class FBTDecorator_MoveCheck : public FBTDecorator
{
public:
	bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
};

class FBTTask_MoveToAnyWhere : public FBTTask
{
public:
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;

private:
	static constexpr float MoveTime = 2.0f;
	float CurrentMoveTime = 0.0f;
	XMFLOAT3 StartLocation{};
	XMFLOAT3 TargetLocation{};
	static constexpr XMFLOAT3 MoveMin{200.0f, 0.0f, 200.0f};
	static constexpr XMFLOAT3 MoveMax{500.0f, 0.0f, 500.0f};
	std::weak_ptr<class AActor> OwningActor;
};

class FBTTask_Wait : public FBTTask
{
public:
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;

	void SetWaitTime(float NewWaitTime) {WaitTime = NewWaitTime;}

private:
	float CurrentWaitTime = 0.0f;
	float WaitTime = 1.0f;
};

class UTestBT : public UBehaviorTree
{
	MY_GENERATE_BODY(UTestBT)
public:
	UTestBT();
	~UTestBT() override = default;

	void OnConstruct() override;
	
};