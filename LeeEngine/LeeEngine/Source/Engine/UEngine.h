// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include <functional>
#include <thread>
#include <unordered_set>

#include "Animation/Animation.h"
#include "DirectX/d3dApp.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"


class USceneComponent;

class UEngine : public UObject
{
	MY_GENERATED_BODY(UEngine)

public:
	UEngine() = default;
	UEngine(D3DApp* CurrentApplication)
	{
		Application = CurrentApplication;
	}
	~UEngine() override;
	virtual void InitEngine();
	void PostLoad() override;
	virtual void LoadDataFromDefaultEngineIni();
	virtual void LoadDefaultMap();
	virtual const std::string& GetDefaultMapName();

	void Tick(float DeltaSeconds);

	const std::shared_ptr<UWorld>& GetWorld() const {return CurrentWorld;}

	const std::string& GetDirectoryPath() const { return CurrentDirectory; }

	void MakeComponentTransformDirty(std::shared_ptr<USceneComponent>& SceneComponent);
	
	XMMATRIX Test_DeleteLater_GetViewMatrix() const
	{
		XMFLOAT3 Pos= XMFLOAT3(0.0f,5.0f,-5.0f);
		XMFLOAT3 View = XMFLOAT3(0.0f,-1.0f,1.0f);
		XMStoreFloat3(&View, XMVector3Normalize(XMLoadFloat3(&View)));
		return XMMatrixLookToLH(XMLoadFloat3(&Pos), XMLoadFloat3(&View), XMVectorSet(0.0f,1.0f,0.0f,0.0f));
	}
	XMMATRIX Test_DeleteLater_GetProjectionMatrix() const
	{
		return XMMatrixPerspectiveFovLH(0.5*XM_PI, 2540.0f/1440.0f, 1.0f, 1000.0f);
	}
	XMFLOAT3 Test_DeleteLater_GetCameraPosition() const {return Application->Test_DELETELATER_GetCameraPosition();}

	void JoinThreadsAtDestroy();
	bool IsStartGameThread() { return GameThreadFrameCount > 0; }

	const D3DApp* GetApplication() const {return Application;}
	HWND GetWindow() const {return Application->GetMainWnd();}

	const std::string& GetEngineDirectory()const {return EngineDirectory;}

	virtual void HandleInput( UINT msg, WPARAM wParam, LPARAM lParam);


	Animation* TestAnim1 = nullptr;
	Animation* TestAnim2 = nullptr;
protected:
	virtual void CreateRenderThread();
private:
	// ============= ImGui =============
	void InitImGui();
	// =================================
	void LoadAllObjectsFromFile();
public:
protected:
	// Default Engine ini Data
	std::map<std::string, std::string> EngineData;

	// =============== 쓰레드 =================
	std::vector<std::thread>		GameThreads;
	UINT							GameThreadFrameCount = 0;
	std::thread						RenderThread;
	UINT							RenderThreadFrameCount = 0;
	// =======================================

private:
	D3DApp* Application;
	std::shared_ptr<UWorld> CurrentWorld;

	bool bGameStart = false;


	// =============== 컴퍼넌트 변경사항 ================
	// 중복 원소만 들어가지 않으면되며, 순서가 중요하지 않음
	std::unordered_map<UINT, std::shared_ptr<USceneComponent>> ComponentsTransformDirty;
	std::string CurrentDirectory;
	std::string EngineDirectory;
};
