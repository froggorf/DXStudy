// 03.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


#include "ImguiActorDetail.h"

FImguiActorDetail::FImguiActorDetail(FScene* Scene, class FImguiLevelViewport* Owner)
	: FImguiPanel(Scene)
{
	LevelViewportOwnerPanel = Owner;
}

void FImguiActorDetail::Draw()
{
	if(ImGui::Begin("Detail"))
	{
		if(CurrentSelectedActor)
		{
			// 컴퍼넌트들 렌더링
			{
				if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,200.0f)))
				{
					int ComponentCount = SelectActorComponentNames.size();
					for (int i = 0; i < ComponentCount; i++) {
						const bool is_selected = (CurrentSelectedComponentIndex == i);
						if (ImGui::Selectable(SelectActorComponentNames[i].data(), is_selected)) {
							CurrentSelectedComponentIndex = i;
							// TODO : Select Component Action
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndListBox();
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
	SelectActorComponents.clear();
	SelectActorComponentNames.clear();
	CurrentSelectedComponentIndex=-1;
}

void FImguiActorDetail::SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor)
{
	SelectActorComponents.clear();
	SelectActorComponentNames.clear();
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

	std::string HierarchyTabString;
	HierarchyTabString.reserve(2*CurrentHierarchyDepth);
	for(int i = 0; i < CurrentHierarchyDepth; ++i)
	{
		HierarchyTabString += "  ";
	}

	std::string TargetComponentName = HierarchyTabString + TargetComponent->GetName();

	SelectActorComponents.push_back(TargetComponent);
	SelectActorComponentNames.push_back(TargetComponentName);

	const std::vector<std::shared_ptr<USceneComponent>>& TargetComponentChildren = TargetComponent->GetAttachChildren();
	for(const auto& ChildComponent : TargetComponentChildren)
	{
		FindComponentsAndNamesFromActor(ChildComponent, CurrentHierarchyDepth+1);
	}
}
