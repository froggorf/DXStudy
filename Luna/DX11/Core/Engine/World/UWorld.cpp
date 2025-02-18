// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UWorld.h"

#include "imgui.h"
#include "Engine/UEngine.h"
#include "Engine/GameFramework/AActor.h"

UWorld::UWorld()
{
	
}

UWorld::~UWorld()
{
}

void UWorld::InitWorldOutliner()
{
	if(PersistentLevel)
	{

	}
}


void UWorld::Init()
{
	UObject::Init();

	if(!PersistentLevel)
	{
		PersistentLevel = std::make_shared<ULevel>(shared_from_this());
	}

	InitWorldOutliner();
	GEngine->AddImGuiRenderFunction(std::bind(&UWorld::ImguiRender_WorldOutliner, this));
}

void UWorld::PostLoad()
{
	UObject::PostLoad();
}

void UWorld::TickWorld(float DeltaSeconds)
{
	std::cout<< "tick world" << std::endl;
	if(PersistentLevel)
	{
		PersistentLevel->TickLevel(DeltaSeconds);
	}
}

void UWorld::Tick()
{
}

void UWorld::TestDrawWorld()
{
	if(PersistentLevel)
	{
		PersistentLevel->TestDrawLevel();
	}
}

void UWorld::ImguiRender_WorldOutliner()
{
	if(PersistentLevel)
	{
		ImGui::NewFrame();

		static int current_item = -1; 
		
		const std::vector<std::shared_ptr<AActor>>& LevelActors = PersistentLevel->GetLevelActors();

		ImGui::Begin("World Outliner");

		if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,-FLT_MIN))) {
			int ActorCount = LevelActors.size();
			for (int i = 0; i < ActorCount; i++) {
				const bool is_selected = (current_item == i);
				if (ImGui::Selectable(LevelActors[i]->GetName().c_str(), is_selected)) {
					current_item = i; 
				}

				// 선택된 항목에 대한 포커스 처리
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}

		/*if(ImGui::BeginListBox("Actors", ImVec2(500,500)))
		{
			ImGui::ListBox()
		}
		ImGui::EndListBox();

		const char* test[] = {"asd", "2","3"};

		static int CurrentSelectActorCount = 0;
		if(ImGui::ListBox("Actors", &CurrentSelectActorCount, test, IM_ARRAYSIZE(test)))
		{
			std::cout<< test[CurrentSelectActorCount] << std::endl;
		}*/


		ImGui::End();

	}
}
