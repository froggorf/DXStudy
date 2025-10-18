#include "CoreMinimal.h"
#include "ImguiDebugConsole.h"

#include "Engine/RenderCore/EditorScene.h"

// 검색에 사용되는 string
std::string FImguiDebugConsole::DebugConsoleSearchText;
// 디버그 텍스트 벡터
std::vector<DebugText> FImguiDebugConsole::DebugConsoleText;
// Pending Add
std::vector<DebugText> FImguiDebugConsole::PendingAddDebugConsoleText;
// 검색 시 string에 맞는 디버그 문구만 모은 벡터
std::vector<DebugText> FImguiDebugConsole::SearchingDebugConsoleText;
// 가끔 씬 재생성 시 변수들을 인식못하는 경우가 발생하여 다음과같이 딜레이를 한프레임 조정
std::atomic<bool> FImguiDebugConsole::bDelay;

FImguiDebugConsole::FImguiDebugConsole(FScene* SceneData)
	: FImguiPanel(SceneData)
{
}

void FImguiDebugConsole::Draw()
{
	if (bDelay)
	{
		bDelay = false;
		return;
	}
	// Pending Add
	for (const auto& Text : PendingAddDebugConsoleText)
	{
		DebugConsoleText.emplace_back(Text);
	}
	PendingAddDebugConsoleText.clear();

	// Render 시작
	if (ImGui::Begin("Debug Console"))
	{
		// EditBox
		char* CurrentText = DebugConsoleSearchText.data();
		ImGui::Text("Search: ");
		ImGui::SameLine();
		if (ImGui::InputText(" ", CurrentText, 100))
		{
			DebugConsoleSearchText = CurrentText;
			SearchDebugConsole();
		}

		ImGui::SameLine();
		ImVec2 MousePos = ImGui::GetMousePos();
		ImGui::Text("x = %.2f , y = %.2f", MousePos.x, MousePos.y);
		ImGui::SameLine();
		ImGui::Text("FPS = %.2f", GEngine->GetApplication()->CurrentFrame);
		ImGui::SameLine();
		ImGui::Text("RenderFPS = %.2f", RenderFPS);
		if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN)))
		{
			// 디버그 리스트 박스의 맨 아래를 볼 시 맨 아래로 고정
			bool bIsFixListBox = ImGui::GetScrollMaxY() == ImGui::GetScrollY();

			// 검색 중 체크
			bool bWhileSearching = !DebugConsoleSearchText.empty();
			if (bWhileSearching)
			{
				for (const DebugText& Text : SearchingDebugConsoleText)
				{
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
				}
			}
			// 검색 아닐 시
			else
			{
				size_t Size = DebugConsoleText.size();
				for (int i = 0; i < Size; ++i)
				{
					const DebugText& Text = DebugConsoleText[i];
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
				}
			}

			// 리스트 맨 아래였을 시 고정
			if (bIsFixListBox)
			{
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::EndListBox();
		}

		ImGui::End();
	}
}

void FImguiDebugConsole::ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData)
{
	// 타입 캐스팅 가능한지 여부를 체크
	if (CommandData->IsOfType(FImguiDebugConsoleCommandData::GetClassID()))
	{
		FImguiPanelCommandData* Data             = CommandData.get();
		auto                    DebugConsoleData = static_cast<FImguiDebugConsoleCommandData*>(Data);
		PendingAddDebugConsoleText.push_back(DebugConsoleData->DebugText);
	}
}

void FImguiDebugConsole::SearchDebugConsole()
{
	SearchingDebugConsoleText.clear();

	if (DebugConsoleSearchText.size() == 0)
	{
		return;
	}

	for (const auto& Text : DebugConsoleText)
	{
		const std::string& LogText = Text.Text;
		if (LogText.contains(DebugConsoleSearchText))
		{
			SearchingDebugConsoleText.push_back(Text);
		}
	}
}
