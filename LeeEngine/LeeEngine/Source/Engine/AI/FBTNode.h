#pragma once
#include "FBlackBoard.h"

enum class ETaskResult
{
	Success, Running, Fail
};



// 데코레이터 및 이벤트 -> 실행순서 (데코레이터 - 이벤트 - 노드(컴포짓/태스크))
// 이벤트
// 특정 컴포짓이나 태스크에 부착되어
// 해당 노드가 활성화 시 이벤트가 매틱 실행됨
class FBTEvent
{
public:
	FBTEvent() = default;
	virtual ~FBTEvent() = default;
	virtual void Event(const std::shared_ptr<FBlackBoard>& BlackBoard);
};

// 데코레이터
// 특정 컴포짓이나 태스크에 부착되어 해당 조건을 만족시에만 그 노드가 실행되도록 함
class FBTDecorator
{
public:
	FBTDecorator() = default;
	virtual ~FBTDecorator() = default;
	virtual bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard);
};


// Root, Composite, Task 등의 부모
// AAIController 내 std::shared_ptr<FBTTask> 소유
class FBTNode
{
public:
	FBTNode() = default;
	virtual ~FBTNode() = default;

	// 하위 FBTNode 클래스의 Tick 에서는 FBTNode::Tick을 호출 후 Success 인 경우에만 실행하도록 해야함
	virtual ETaskResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard);

	const std::shared_ptr<FBTNode>& GetParent() const
	{
		if (const std::shared_ptr<FBTNode>& SharedParent = Parent.lock())
		{
			return SharedParent;
		}
		return nullptr;
	}
protected:
	std::shared_ptr<FBTEvent> Event;
	std::vector<std::shared_ptr<FBTDecorator>> Decorators;

	std::vector<std::shared_ptr<FBTNode>> Children;
	std::weak_ptr<FBTNode> Parent;
};

class FBTCompositeNode : public FBTNode
{
public:
	FBTCompositeNode() = default;
	~FBTCompositeNode() override = default;
};

class FBTSequencer : public FBTCompositeNode
{
public:
	~FBTSequencer() override = default;

	ETaskResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;
};

class FBTSelector : public FBTCompositeNode
{
public:
	~FBTSelector() override = default;

	ETaskResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;
};

// Task의 경우 해당 클래스를 상속 받아서 새로운 Task 를 만들면 됨
// ex) FBTTask_MoveTo : FBTNode
class FBTTask : public FBTNode
{
};