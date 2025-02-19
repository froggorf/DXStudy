// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


// VFX 업데이트, Subsystem업데이트, 레벨 업데이트 같은 전체적인 역할을 관리하는 클래스
#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Level/ULevel.h"
#include "Engine/UObject/UObject.h"

class USceneComponent;
class ULevel;
class AActor;

class UWorld : public UObject, public std::enable_shared_from_this<UWorld>
{
public:
	UWorld();
	~UWorld() override;
	void Init() override;
	void PostLoad() override;

	void TickWorld(float DeltaSeconds);
	void Tick();


	std::shared_ptr<ULevel> GetPersistentLevel() const { return PersistentLevel; }
	void SetPersistentLevel(const std::shared_ptr<ULevel> NewLevel) { PersistentLevel = NewLevel; }

	void TestDrawWorld();

	void ImguiRender_WorldOutliner();
	void ImGuiAction_SelectActor(const std::shared_ptr<AActor>& NewSelectActor);
	// 타겟 컴퍼넌트의 정보와 해당 타겟 컴퍼넌트의 자식들의 정보를 저장
	void ImGuiAction_FindActorComponentsAndNames(const std::shared_ptr<USceneComponent>& TargetComponent, int CurrentHierarchyDepth);
	void ImGuiRender_ActorDetail();
	void ImGuizmoRender_SelectComponentGizmo();
	
protected:
private:
public:
protected:
private:
	// 콜렉션과 관련된 현재 레벨
	std::shared_ptr<ULevel>	PersistentLevel;

	// 콜렉션에 있는 모든 레벨정보
	std::set<std::shared_ptr<ULevel>> Levels;

	// 현재 선택된 액터
	// TODO: 추후 에디터 기능으로 분리하기
	std::shared_ptr<AActor> CurrentSelectedActor;
	std::vector<std::shared_ptr<USceneComponent>> SelectActorComponents;
	std::vector<std::string> SelectActorComponentNames;
	int CurrentSelectedComponentIndex = -1;
};
