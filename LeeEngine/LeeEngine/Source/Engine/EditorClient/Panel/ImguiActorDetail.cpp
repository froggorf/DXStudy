// 03.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "ImguiActorDetail.h"
#ifdef WITH_EDITOR
FImguiActorDetail::FImguiActorDetail(FScene* Scene, class FImguiLevelViewport* Owner)
	: FImguiPanel(Scene)
{
	LevelViewportOwnerPanel = Owner;
}

void FImguiActorDetail::Draw()
{
	if(ImGui::Begin("Detail", nullptr, ImGuiWindowFlags_NoMove))
	{
		if(CurrentSelectedActor)
		{
			// 컴퍼넌트들 렌더링
			{
				if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,100.0f)))
				{
					int ComponentCount = SelectActorSceneComponentNames.size();
					for (int i = 0; i < ComponentCount; ++i) {
						const bool is_selected = (CurrentSelectedComponentIndex == i);
						if (ImGui::Selectable(SelectActorSceneComponentNames[i].data(), is_selected)) {
							CurrentSelectedComponentIndex = i;
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}

						
					}

					int ActorComponentCount = SelectActorActorComponentNames.size();
					for(int index = 0; index < ActorComponentCount; ++index)
					{
						const bool is_selected = (CurrentSelectedComponentIndex == index + ComponentCount);
						if (ImGui::Selectable(SelectActorActorComponentNames[index].data(), is_selected)) {
							CurrentSelectedComponentIndex = index + ComponentCount;
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndListBox();
					ImGui::BeginChild("Detail");
					if(const auto& CurrentSelectedComponent = GetCurrentSelectedComponent())
					{
						CurrentSelectedComponent->DrawDetailPanel(0);
					}
					else if(const auto& CurrentSelectedActorComponent = GetCurrentSelectedActorComponent())
					{
						CurrentSelectedActorComponent->DrawDetailPanel(0);
					}
					ImGui::EndChild();
				}
			}

		}
		else
		{
			ImGui::Text("No Select");
		}
		ImGui::End();	
	}
}

void FImguiActorDetail::InitLevelData()
{
	CurrentSelectedActor = nullptr;
	SelectActorSceneComponents.clear();
	SelectActorSceneComponentNames.clear();
	SelectActorActorComponents.clear();
	SelectActorActorComponentNames.clear();
	CurrentSelectedComponentIndex=-1;
}

void FImguiActorDetail::SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor)
{
	SelectActorSceneComponents.clear();
	SelectActorSceneComponentNames.clear();
	SelectActorActorComponents.clear();
	SelectActorActorComponentNames.clear();

	CurrentSelectedComponentIndex = 0;
	CurrentSelectedActor = NewSelectedActor;
	FindComponentsAndNamesFromActor(CurrentSelectedActor->GetRootComponent(), 0);
}

void FImguiActorDetail::FindComponentsAndNamesFromActor(const std::shared_ptr<USceneComponent>& TargetComponent,
	int CurrentHierarchyDepth)
{
	if(!CurrentSelectedActor)
	{
		return;
	}

	// 계층에 따른 SceneComponent 찾기
	std::string HierarchyTabString;
	HierarchyTabString.reserve(2*CurrentHierarchyDepth);
	for(int i = 0; i < CurrentHierarchyDepth; ++i)
	{
		HierarchyTabString += "  ";
	}

	std::string TargetComponentName = HierarchyTabString + TargetComponent->GetName();

	SelectActorSceneComponents.push_back(TargetComponent);
	SelectActorSceneComponentNames.push_back(TargetComponentName);

	const std::vector<std::shared_ptr<USceneComponent>>& TargetComponentChildren = TargetComponent->GetAttachChildren();
	for(const auto& ChildComponent : TargetComponentChildren)
	{
		FindComponentsAndNamesFromActor(ChildComponent, CurrentHierarchyDepth+1);
	}

	// UActorComponent에 대해서 조사
	if(CurrentHierarchyDepth == 0)
	{
		for (const auto& Component : CurrentSelectedActor->GetComponents())
		{
			SelectActorActorComponents.push_back(Component);
			SelectActorActorComponentNames.push_back(Component->GetName());
		}
	}
}
#endif