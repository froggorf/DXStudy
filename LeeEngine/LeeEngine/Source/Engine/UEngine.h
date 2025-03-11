// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include <functional>
#include <thread>

#include "DirectX/d3dApp.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"


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
	void Draw();
	const std::shared_ptr<UWorld>& GetWorld() const {return CurrentWorld;}

	const std::string& GetDirectoryPath() const { return CurrentDirectory; }


	XMMATRIX Test_DeleteLater_GetViewMatrix() const {return Application->Test_DeleteLater_GetViewMatrix();}
	XMMATRIX Test_DeleteLater_GetProjectionMatrix() const {return Application->Test_DeleteLater_GetProjectionMatrix();}
	XMFLOAT3 Test_DeleteLater_GetCameraPosition() const {return Application->Test_DELETELATER_GetCameraPosition();}

	void JoinThreadsAtDestroy();
protected:
private:
	// ============= ImGui =============
	void InitImGui();
	void DrawImGui();
	// =================================
	void LoadAllObjectsFromFile();
public:
protected:
	// Default Engine ini Data
	std::map<std::string, std::string> EngineData;
private:
	D3DApp* Application;
	std::shared_ptr<UWorld> CurrentWorld;

	// =============== 쓰레드 =================
	std::vector<std::thread>		GameThreads;
	UINT							GameThreadFrameCount = 0;
	std::thread						RenderThread;
	UINT							RenderThreadFrameCount = 0;
	// =======================================



	std::string CurrentDirectory;
};