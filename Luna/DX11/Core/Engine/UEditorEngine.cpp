// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEditorEngine.h"

#include "imgui_internal.h"


std::shared_ptr<UEditorEngine> GEditorEngine = nullptr;

std::map<EDebugLogLevel, ImVec4> DebugText::Color = std::map<EDebugLogLevel, ImVec4>
{
	{EDebugLogLevel::DLL_Error, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Fatal, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Display, ImVec4(0.8f,0.8f,0.8f,1.0f)},
	{EDebugLogLevel::DLL_Warning, ImVec4(0.95f,0.73f,0.125f,1.0f)},
};

void UEditorEngine::InitEngine()
{
	UEngine::InitEngine();

	AddImGuiRenderFunction(std::bind(&UEditorEngine::DrawDebugConsole, this));
	MY_LOG("Init", EDebugLogLevel::DLL_Display, "UEditorEngine init");
	
}

void UEditorEngine::DrawDebugConsole()
{
	ImGui::Begin("Debug Console");

	// EditBox
	char* CurrentText = DebugConsoleSearchText.data();
	ImGui::Text("Search: ");
	ImGui::SameLine();
	if(ImGui::InputText(" ",CurrentText,100))
	{
		DebugConsoleSearchText = CurrentText;
		SearchDebugConsole();
	}
	
	ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN));

	// 디버그 리스트 박스의 맨 아래를 볼 시 맨 아래로 고정
	bool bIsFixListBox = ImGui::GetScrollMaxY() == ImGui::GetScrollY();
	

	// 검색 중 체크
	bool bWhileSearching = DebugConsoleSearchText.size() != 0;
	if(bWhileSearching)
	{
		for(const DebugText& Text : SearchingDebugConsoleText)
		{
			ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
		}	
	}
	// 검색 아닐 시
	else
	{
		for(const DebugText& Text : DebugConsoleText)
		{
			ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
			
		}	
	}

	// 리스트 맨 아래였을 시 고정
	if(bIsFixListBox)
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndListBox();

	ImGui::End();
}

void UEditorEngine::SearchDebugConsole()
{
	SearchingDebugConsoleText.clear();

	if(DebugConsoleSearchText.size() == 0)
	{
		return;
	}

	for(const auto& Text : DebugConsoleText)
	{
		const std::string& LogText = Text.Text;
		if(LogText.contains(DebugConsoleSearchText))
		{
			SearchingDebugConsoleText.push_back(Text);
		}
	}
}
