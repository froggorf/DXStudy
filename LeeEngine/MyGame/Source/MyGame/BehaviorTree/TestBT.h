#pragma once
#include "Engine/AI/FBTNode.h"

enum class EBlackBoardValueCheckType
{
	Equal, NotEqual,
	Less, LessOrEqual,
	Greater, GreaterOrEqual,
	IsTrue, IsFalse,
};

template<class TDataClass>
class FBTDecorator_BlackBoardValueCheck : public FBTDecorator
{
public:
	bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override
	{
		BlackBoard->AddBlackBoardValueChangeObserver(BlackBoardKey);
	}
	void Initialize(const std::string& TargetKeyName, const typename TDataClass::FDataType& InValue, EBlackBoardValueCheckType ValueCheckType)
	{
		BlackBoardKey = TargetKeyName;
		this->ValueCheckType = ValueCheckType;
		TargetValue = InValue;
	}
private:
	std::string BlackBoardKey;
	EBlackBoardValueCheckType ValueCheckType = EBlackBoardValueCheckType::Equal;

	typename TDataClass::FDataType TargetValue;
};

template <class TDataClass>
bool FBTDecorator_BlackBoardValueCheck<TDataClass>::Eval(const std::shared_ptr<FBlackBoard>& BlackBoard)
{
	const typename TDataClass::FDataType CurrentVal = BlackBoard->GetValue<TDataClass>(BlackBoardKey);

	if constexpr (std::is_same_v<typename TDataClass::FDataType, bool>)
	{
		switch (ValueCheckType)
		{
		case EBlackBoardValueCheckType::IsTrue:
			return CurrentVal;
		case EBlackBoardValueCheckType::IsFalse:
			return !CurrentVal;
		default:
			// bool의 다른 처리를 아래에서 진행하기 위해
			break;
		}
	}

	switch (ValueCheckType)
	{
	case EBlackBoardValueCheckType::Equal:
		return TargetValue == CurrentVal;
	case EBlackBoardValueCheckType::Greater:
		return TargetValue < CurrentVal;
	case EBlackBoardValueCheckType::GreaterOrEqual:
		return TargetValue <= CurrentVal;
	case EBlackBoardValueCheckType::Less:
		return TargetValue > CurrentVal;
	case EBlackBoardValueCheckType::LessOrEqual:
		return TargetValue >= CurrentVal;
	case EBlackBoardValueCheckType::NotEqual:
		return TargetValue != CurrentVal;
	default:
		break;
	}

	return false;
}

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

class FBTTask_MoveToPlayer : public FBTTask
{
public:
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;

private:
	std::weak_ptr<class AEnemyBase> OwningActor;
	std::weak_ptr<class AActor> Player;
};

class FBTTask_EnemyBasicAttack : public FBTTask
{
public:
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	void FinishAttack() { bAttackFinish = true;}
private:
	std::weak_ptr<class AEnemyBase> OwningActor;
	std::weak_ptr<class AActor> Player;
	bool bAttackFinish = false;
	bool bDoAttack = false;
	float LastAttackTime = 0.0f;

	static constexpr float CoolDownTime = 5.0f;
};

class FBTTask_RunFromPlayer : public FBTTask
{
public:
	void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;

private:
	std::weak_ptr<class AEnemyBase> OwningActor;
	std::weak_ptr<class AActor> Player;
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


// 플레이어가 거리 200m 이내에 있다면,
	// 체력이 20% 이상이면
		// 적 앞으로 다가간다
		// 일반공격 한다
	// 아니라면
		// 적의 반대방향으로 도망친다
// 플레이어가 주변에 없다면
	// 주변을 배회한다
	// 5초 기다린다.
	

class UTestBT : public UBehaviorTree
{
	MY_GENERATE_BODY(UTestBT)
public:
	UTestBT();
	~UTestBT() override = default;

	void OnConstruct() override;
	
};