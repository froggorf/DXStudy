#pragma once
#include "FBlackBoard.h"
class UBehaviorTree;

enum class EBTNodeResult
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
	virtual void ExecuteEvent(const std::shared_ptr<FBlackBoard>& BlackBoard) = 0;
};

// 데코레이터
// 특정 컴포짓이나 태스크에 부착되어 해당 조건을 만족시에만 그 노드가 실행되도록 함
class FBTDecorator
{
public:
	FBTDecorator() = default;
	virtual ~FBTDecorator() = default;
	virtual bool Eval(const std::shared_ptr<FBlackBoard>& BlackBoard);
	virtual void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard) {}
};


// Root, Composite, Task 등의 부모
// AAIController 내 std::shared_ptr<FBTNode> 소유
class FBTNode : public std::enable_shared_from_this<FBTNode>
{
public:
	FBTNode() = default;
	virtual ~FBTNode() = default;

	virtual EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard);

	UINT FindChildIndex(const FBTNode* ChildNode);
	// Composite 노드 (Sequencer, Selector) 에서 오버라이딩
	virtual void OnChildExecutionFinish(float DeltaSeconds, FBTNode* ChildNode, EBTNodeResult Result) {};

	std::shared_ptr<FBTNode> GetParent() const
	{
		if (const std::shared_ptr<FBTNode>& SharedParent = Parent.lock())
		{
			return SharedParent;
		}
		return nullptr;
	}

	void SetOwnerTree(const std::shared_ptr<UBehaviorTree>& BT)
	{
		OwnerTree = BT;
	}

	void AddChild(const std::shared_ptr<FBTNode>& NewNode)
	{
		const std::shared_ptr<UBehaviorTree>& BT = OwnerTree.lock();
		assert(BT && "루트의 경우엔 OwnerTree를 먼저 설정해주고서 해줘야함");
		Children.emplace_back(NewNode);
		NewNode->Parent = shared_from_this();
		NewNode->SetOwnerTree(BT);
	}

	// Composite 노드인지 체크하는 함수
	virtual bool IsCompositeNode() const { return false; };

	// 노드가 수행되기 이전 값 초기화 등의 작업을 진행하는 함수
	virtual void OnEnterNode(const std::shared_ptr<FBlackBoard>& BlackBoard);

	void SetRootNode() {bIsRootNode = true;}
public:
	std::shared_ptr<FBTEvent> Event;
	std::vector<std::shared_ptr<FBTDecorator>> Decorators;

	std::vector<std::shared_ptr<FBTNode>> Children;
	std::weak_ptr<FBTNode> Parent;

	std::weak_ptr<UBehaviorTree> OwnerTree;
protected:
	bool bIsRootNode = false;
};

class FBTCompositeNode : public FBTNode
{
public:
	FBTCompositeNode() = default;
	~FBTCompositeNode() override = default;

	EBTNodeResult Tick(float DeltaSeconds, const std::shared_ptr<FBlackBoard>& BlackBoard) override;
	bool IsCompositeNode() const override { return true; }
};

class FBTSequencer : public FBTCompositeNode
{
public:
	~FBTSequencer() override = default;

	void OnChildExecutionFinish(float DeltaSeconds, FBTNode* ChildNode, EBTNodeResult Result) override;
};

class FBTSelector : public FBTCompositeNode
{
public:
	~FBTSelector() override = default;
	void OnChildExecutionFinish(float DeltaSeconds, FBTNode* ChildNode, EBTNodeResult Result) override;
};

// Task의 경우 해당 클래스를 상속 받아서 새로운 Task 를 만들면 됨
// ex) FBTTask_MoveTo : FBTNode
class FBTTask : public FBTNode
{
public:
};


// 해당 클래스를 상속받아 코드상으로 에셋을 만들고,
// AIController에 UBehaviorTree을 연결하여 AI를 재생하도록 구현함
// UBehaviorTree.h 수정
class UBehaviorTree : public UObject, public std::enable_shared_from_this<UBehaviorTree>
{
	MY_GENERATE_BODY(UBehaviorTree)
public:
	UBehaviorTree() = default;
	~UBehaviorTree() override = default;

	void Register() override;
	// 실질적으로 노드를 만드는 함수, Register 에서 실행
	virtual void OnConstruct();

	// 이벤트 기반 틱
	EBTNodeResult Tick(float DeltaSeconds);

	const std::shared_ptr<FBTNode>& GetBTRoot() const {return BTRoot;}
	const std::shared_ptr<FBlackBoard>& GetBlackBoard() const {return BlackBoard;}
	// 현재 실행 중인 노드 설정
	void SetRunningNode(const std::shared_ptr<FBTNode>& Node) { CurrentRunningNode = Node; }

	// 트리 실행 완료 처리
	void OnTreeCompleted(EBTNodeResult Result);

	// 만약 CompositeNode에서 다음 실행하게 할 노드를 지정해야할 경우 사용
	void SetCurrentRunningNode(const std::shared_ptr<FBTNode>& NewRunningNode) { CurrentRunningNode = NewRunningNode; }
	void ClearCurrentRunningNode() {CurrentRunningNode = nullptr;}
	EBTNodeResult ExecuteNode(float DeltaSeconds, const std::shared_ptr<FBTNode>& Node);

protected:
	std::shared_ptr<FBTNode> BTRoot;
	std::shared_ptr<FBlackBoard> BlackBoard;

	// 현재 Running 중인 노드
	// 존재하면 해당 노드를 실행하고,
	// 존재하지 않을 경우 루트부터 다시 실행
	std::shared_ptr<FBTNode> CurrentRunningNode;

	bool bIsRootSelector = true;

};