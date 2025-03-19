// 02.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UEngine.h"
#include "RenderCore/RenderingThread.h"

#include <atlImage.h>

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
	UEditorEngine() : UEngine(nullptr) {	}

	UEditorEngine(D3DApp* Application) : UEngine(Application)	{	};
	~UEditorEngine() override;
	virtual void InitEngine() override;
	virtual void PostLoad() override;
	virtual const std::string& GetDefaultMapName() override;

	void AddDebugText(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText);

	//void AddConsoleText()
	//{
	//	std::string NewText = Category + " : " + InDebugText;
	//	DebugText NewDebugText{NewText, DebugLevel};
	//	//DebugConsoleText.push_back(NewDebugText);
	//	
	//	FScene::AddConsoleText_GameThread(NewDebugText);
	//}


	void DrawEngineTitleBar();

protected:
	void CreateRenderThread() override;
private:
public:
protected:
private:
	CImage LogoImage;
	

};
