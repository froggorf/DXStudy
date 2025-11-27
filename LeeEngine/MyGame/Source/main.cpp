//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
//#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#pragma message("_DEBUG defined")
#endif
#ifdef NDEBUG
#pragma message("NDEBUG defined")
#endif

#include "CoreMinimal.h"
#include "Engine/Class/Actor/ASkyBox.h"

ASkyBox* Dummy = nullptr;

using namespace DirectX;

class MyGame : public D3DApp
{
public:
	MyGame(HINSTANCE hInstance);
	~MyGame() override;

	// Init
	bool Init() override;
	void OnResize() override;
	void UpdateScene(float dt) override;


private:

	POINT m_LastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" )
#if defined(DEBUG) | defined(_DEBUG)
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MyGame theApp(hInstance);

	if (!theApp.Init())
		return -1;

	bool bResult = theApp.Run();

	return bResult;
}

MyGame::MyGame(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	m_MainWndTitle = L"Character Animation";

	m_LastMousePos.x = 0;
	m_LastMousePos.y = 1;
}

MyGame::~MyGame()
{
	// ComPtr
}

bool MyGame::Init()
{
	if (!D3DApp::Init())
		return false;

	return true;
}



void MyGame::OnResize()
{
	D3DApp::OnResize();
}

void MyGame::UpdateScene(float dt)
{
	GEngine->Tick(dt);
}

