// 02.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UEngine.h"

enum class EDebugLogLevel
{
	DLL_Fatal, DLL_Error, DLL_Warning, DLL_Display, 
};
struct DebugText
{
	DebugText(const std::string& Text, EDebugLogLevel Level)
	{
		this->Text = Text;
		this->Level = Level;
	}
	std::string Text;
	EDebugLogLevel Level;
	static std::map<EDebugLogLevel, ImVec4> Color;
};

class UEditorEngine : public UEngine
{
	MY_GENERATED_BODY(UEditorEngine)
public:
	UEditorEngine() : UEngine(nullptr) {};
	UEditorEngine(D3DApp* Application) : UEngine(Application){};

	virtual void InitEngine() override;
	virtual void PostLoad() override;
	virtual const std::string& GetDefaultMapName() override;

	void AddConsoleText(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText)
	{
		std::string NewText = Category + " : " + InDebugText;
		DebugText NewDebugText{NewText, DebugLevel};
		DebugConsoleText.push_back(NewDebugText);
	}
protected:
private:
	void DrawDebugConsole();
	void SearchDebugConsole();
public:
protected:
private:

	// 디버깅 콘솔
	std::vector<DebugText> DebugConsoleText;
	std::vector<DebugText> SearchingDebugConsoleText;
	std::string DebugConsoleSearchText;
};
