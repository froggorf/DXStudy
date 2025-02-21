// 02.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UEngine.h"


class UEditorEngine : public UEngine
{
public:
	UEditorEngine(D3DApp* Application) : UEngine(Application){};

	virtual void InitEngine() override;

	void AddConsoleText(const std::string& NewDebugText)
	{
		DebugConsoleText.push_back(NewDebugText);
	}
protected:
private:
	void DrawDebugConsole();

public:
protected:
private:
	std::vector<std::string> DebugConsoleText;
	
};