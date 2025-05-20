// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include <functional>
#include <thread>
#include <unordered_set>

#include "Animation/UAnimSequence.h"
#include "DirectX/d3dApp.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"

class USceneComponent;

class UEngine : public UObject
{
	MY_GENERATE_BODY(UEngine)

	UEngine() = default;

	UEngine(D3DApp* CurrentApplication)
	{
		Application = CurrentApplication;
	}

	~UEngine() override;

	virtual void               InitEngine();
	void                       PostLoad() override;
	virtual void               LoadDataFromDefaultEngineIni();
	virtual void               LoadDefaultMap();
	virtual const std::string& GetDefaultMapName();

	float GetDeltaSeconds() const
	{
		return DeltaSeconds;
	}

	// TODO: DELETE_LATER
	void DELETELATER_TestChangeLevel(const std::string& str);

	float GetTimeSeconds() const
	{
		return TimeSeconds;
	}

	/*
	 * 에디터의 경우에는 Play Button 을 통해 호출되며,
	 * 런타임 게임의 경우 엔진의 Init 이후 바로 실행
	 */
	void GameStart();

	void Tick(float DeltaSeconds);

	const std::shared_ptr<UWorld>& GetWorld() const
	{
		return CurrentWorld;
	}

	const std::string& GetDirectoryPath() const
	{
		return CurrentDirectory;
	}

	void MakeComponentTransformDirty(std::shared_ptr<USceneComponent>& SceneComponent);

	XMMATRIX Test_DeleteLater_GetViewMatrix() const
	{
		auto Pos  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		auto View = XMFLOAT3(0.0f, 0.0f, 1.0f);
		XMStoreFloat3(&View, XMVector3Normalize(XMLoadFloat3(&View)));
		return XMMatrixLookToLH(XMLoadFloat3(&Pos), XMLoadFloat3(&View), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}

	XMMATRIX Test_DeleteLater_GetProjectionMatrix() const
	{
		return XMMatrixPerspectiveFovLH(0.5 * XM_PI, 2540.0f / 1440.0f, 0.01f, 10000.0f);
	}

	XMFLOAT3 Test_DeleteLater_GetCameraPosition() const
	{
		return Application->Test_DELETELATER_GetCameraPosition();
	}

	void JoinThreadsAtDestroy();

	bool IsStartGameThread()
	{
		return GameThreadFrameCount > 0;
	}

	D3DApp* GetApplication() const
	{
		return Application;
	}

	HWND GetWindow() const
	{
		return Application->GetMainWnd();
	}

	const std::string& GetEngineDirectory() const
	{
		return EngineDirectory;
	}

	virtual void HandleInput(UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRenderThread();
	void         CreateAudioThread();

private:
	// ============= ImGui =============
	void InitImGui();
	// =================================
	void LoadAllObjectsFromFile();

public:
	std::atomic<bool> bGameStart = false;

protected:
	// Default Engine ini Data
	std::map<std::string, std::string> EngineData;

	// =============== 쓰레드 =================
	std::vector<std::thread> GameThreads;
	UINT                     GameThreadFrameCount = 0;
	std::thread              RenderThread;
	UINT                     RenderThreadFrameCount = 0;
	std::thread              AudioThread;
	// =======================================

private:
	D3DApp*                 Application;
	std::shared_ptr<UWorld> CurrentWorld;
	float                   DeltaSeconds;
	float                   TimeSeconds = 0.0f;

	// =============== 컴퍼넌트 변경사항 ================
	// 중복 원소만 들어가지 않으면되며, 순서가 중요하지 않음
	std::unordered_map<UINT, std::shared_ptr<USceneComponent>> ComponentsTransformDirty;
	std::string                                                CurrentDirectory;
	std::string                                                EngineDirectory;
};
