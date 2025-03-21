// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEditorEngine.h"
#include <fstream>
#include <memory>

#include "Engine/AssetManager/AssetManager.h"
#include "imgui_internal.h"
#include "RenderCore/EditorScene.h"
#include "World/UWorld.h"


std::shared_ptr<UEditorEngine> GEditorEngine = nullptr;

std::map<EDebugLogLevel, ImVec4> DebugText::Color = std::map<EDebugLogLevel, ImVec4>
{
	{EDebugLogLevel::DLL_Error, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Fatal, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Display, ImVec4(0.8f,0.8f,0.8f,1.0f)},
	{EDebugLogLevel::DLL_Warning, ImVec4(0.95f,0.73f,0.125f,1.0f)},
};

UEditorEngine::~UEditorEngine()
{
	if(LogoImage)
	{
		DeleteObject(LogoImage);
	}
}

void UEditorEngine::InitEngine()
{
	UEngine::InitEngine();

	//AddImGuiRenderFunction(std::bind(&UEditorEngine::DrawDebugConsole, this));
	/*FEditorScene::AddImGuiRenderFunction(
		"DebugConsole",
		[]()
		{
			FEditorScene::DrawDebugConsole_RenderThread();
		}
	);
	FEditorScene::AddImGuiRenderFunction(
		"DrawScene",
		[]()
		{
			FEditorScene::DrawImGuiScene_RenderThread();
		}
	);*/

	const std::string& EngineDirectoryString = GetEngineDirectory();
	std::wstring FilePath = std::wstring{EngineDirectoryString.begin(), EngineDirectoryString.end()} + L"/Content/Editor/Logo/LeeEngineLogo.bmp";
	LogoImage.Load(FilePath.c_str());
	if (!LogoImage.IsNull()) {
		MY_LOG("Load -", EDebugLogLevel::DLL_Error, "Load Logo Error");
	}

	MY_LOG("Init", EDebugLogLevel::DLL_Display, "UEditorEngine init");

	
}

void UEditorEngine::PostLoad()
{
	UEngine::PostLoad();

	// Default Engine Map
	//if(GetWorld())
	//{
	//	//std::shared_ptr<ULevel> NewLevel = std::make_shared<ULevel>(*AssetManager::ReadMyAsset<ULevel>(EngineData["EditorStartupMap"]));
	//	//GetWorld()->SetPersistentLevel(NewLevel);
	//	std::shared_ptr<ULevel> NewLevel = std::make_shared<ULevel>(GetWorld());
	//	GetWorld()->SetPersistentLevel(NewLevel);
	//}

}

const std::string& UEditorEngine::GetDefaultMapName()
{
	return EngineData["EditorStartupMap"];
}

void UEditorEngine::DrawEngineTitleBar()
{
	HDC hdc;
	hdc = GetWindowDC(GetWindow());
	RECT rect;
	GetWindowRect(GetWindow(), &rect);
	// 타이틀바 영역 계산
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = 0;
	rect.top = 0;
	rect.bottom = WindowTitleBarHeight; // 타이틀바 높이

	HDC memDC = CreateCompatibleDC(hdc);
	static HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, rect.right-rect.left, rect.bottom-rect.top);
	SelectObject(memDC,hMemBitmap);

	// 원하는 색상으로 타이틀바 채우기
	HBRUSH hBrush = CreateSolidBrush(RGB(21,21,21)); // 파란색 브러시
	FillRect(memDC, &rect, hBrush);
	DeleteObject(hBrush);

	int Gap = 3;
	RECT ImageRect = {Gap,Gap,WindowTitleBarHeight-Gap,WindowTitleBarHeight-Gap};
	if(!LogoImage.IsNull())
	{
		//
		LogoImage.StretchBlt(memDC, ImageRect.left,ImageRect.top,ImageRect.right-ImageRect.left,ImageRect.bottom-ImageRect.top);
	}

	RECT CurrentLevelRect = ImageRect;
	CurrentLevelRect.left = CurrentLevelRect.right+20;
	CurrentLevelRect.right = CurrentLevelRect.left + 200;
	CurrentLevelRect.top = CurrentLevelRect.bottom/2;
	CurrentLevelRect.bottom = CurrentLevelRect.top+30;
	RoundRect(memDC,CurrentLevelRect.left,CurrentLevelRect.top,CurrentLevelRect.right,CurrentLevelRect.bottom*2,15,15);
	if(GetWorld() && GetWorld()->GetPersistentLevel())
	{
		const std::string& PersistentLevelName = GetWorld()->GetPersistentLevel()->GetName();
		DrawTextA(memDC,PersistentLevelName.c_str(),-1, &CurrentLevelRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	}
	MY_LOG("LOG", EDebugLogLevel::DLL_Warning, "REDRAW");
	
	// 텍스트 그리기
	SetBkMode(memDC, TRANSPARENT);
	RECT CloseButtonRect = rect;
	CloseButtonRect.right -=10;
	CloseButtonRect .left = CloseButtonRect.right - 20;
	CloseButtonRect.top +=10;
	CloseButtonRect.bottom = CloseButtonRect.top+10;
	SetTextColor(memDC, RGB(255,0,0));
	DrawTextA(memDC, "X", -1, &CloseButtonRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	BitBlt(hdc,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top, memDC,0,0, SRCCOPY);

	DeleteObject(memDC);
	ReleaseDC(GetWindow(), hdc); // DC 해제
}

void UEditorEngine::CreateRenderThread()
{
	RenderThread = std::thread(&FRenderCommandExecutor::Execute, std::make_shared<FEditorScene>());
}
