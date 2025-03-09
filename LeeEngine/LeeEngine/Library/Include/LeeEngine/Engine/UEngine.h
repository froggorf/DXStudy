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

class D3DApp;

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

	void AddImGuiRenderFunction(const std::function<void()>& NewRenderFunction);
	void AddImGuizmoRenderFunction(const std::function<void()>& NewRenderFunction);

	ID3D11Buffer* TestDeleteLater_GetObjConstantBuffer() const {return Application->Test_DeleteLater_GetObjectConstantBuffer();}
	XMMATRIX Test_DeleteLater_GetViewMatrix() const {return Application->Test_DeleteLater_GetViewMatrix();}
	XMMATRIX Test_DeleteLater_GetProjectionMatrix() const {return Application->Test_DeleteLater_GetProjectionMatrix();}
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

	std::vector<std::thread>		GameThreads;
	std::thread						RenderThread;


	std::vector<std::function<void()>> ImGuiRenderFunctions;
	std::vector<std::function<void()>> ImGuizmoRenderFunctions;

	std::string CurrentDirectory;
};