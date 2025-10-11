// 03.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "ImguiWorldOutliner.h"
#include "ImguiViewport.h"
#include "Engine/GameFramework/AActor.h"

FImguiWorldOutliner::FImguiWorldOutliner(FScene* Scene, class FImguiLevelViewport* Owner)
	: FImguiPanel(Scene)
{
	LevelViewportOwnerPanel = Owner;
}

void FImguiWorldOutliner::Draw()
{
	//Pending Kill
	for (const std::shared_ptr<AActor>& PendingKillActor : PendingKillWorldOutlinerActors)
	{
		auto RemovedActor = std::ranges::find(WorldOutlinerActors, PendingKillActor);
		if (RemovedActor != WorldOutlinerActors.end())
		{
			WorldOutlinerActors.erase(RemovedActor);	
		}
	}
	PendingKillWorldOutlinerActors.clear();


	// Pending Add
	for (const auto& NewOutlinerActor : PendingAddWorldOutlinerActors)
	{
		WorldOutlinerActors.push_back(NewOutlinerActor);
	}
	PendingAddWorldOutlinerActors.clear();

	// WorldOutliner
	static int CurrentItem = -1;
	if (ImGui::Begin("World Outliner", nullptr))
	{
		if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN)))
		{
			size_t ActorCount = WorldOutlinerActors.size();
			for (size_t i = 0; i < ActorCount; i++)
			{
				const bool is_selected = (CurrentItem == i);
				if (ImGui::Selectable(WorldOutlinerActors[i]->GetName().c_str(), is_selected))
				{
					CurrentItem = static_cast<int>(i);

					LevelViewportOwnerPanel->SelectActorFromWorldOutliner(WorldOutlinerActors[i]);
				}

				// 선택된 항목에 대한 포커스 처리
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}

		ImGui::End();
	}
}

void FImguiWorldOutliner::InitLevelData()
{
	WorldOutlinerActors.clear();
	PendingAddWorldOutlinerActors.clear();
	PendingKillWorldOutlinerActors.clear();
}

void FImguiWorldOutliner::PendingAddWorldOutlinerActor(const std::shared_ptr<AActor>& NewActor)
{
	PendingAddWorldOutlinerActors.emplace_back(NewActor);
}

void FImguiWorldOutliner::PendingKillWorldOutlinerActor(const std::shared_ptr<AActor>& RemoveActor)
{
	PendingKillWorldOutlinerActors.emplace_back(RemoveActor);
}
