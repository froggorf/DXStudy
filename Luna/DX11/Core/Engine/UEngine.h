// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include <functional>

#include "d3dApp.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"

class D3DApp;

class UEngine : public UObject
{
public:
	UEngine(D3DApp* CurrentApplication)
	{
		Application = CurrentApplication;
	}
	~UEngine() override;
	virtual void InitEngine();
	void PostLoad() override;
	void Tick(float DeltaSeconds);
	void Draw();

	void AddImGuiRenderFunction(const std::function<void()>& NewRenderFunction);
	void AddImGuizmoRenderFunction(const std::function<void()>& NewRenderFunction);

	ID3D11Device* GetDevice() const {return Application->GetDevice();}
	ID3D11DeviceContext* GetDeviceContext() const {return Application->GetDeviceContext();}
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
private:
	D3DApp* Application;
	std::shared_ptr<UWorld> CurrentWorld;

	std::vector<std::function<void()>> ImGuiRenderFunctions;
	std::vector<std::function<void()>> ImGuizmoRenderFunctions;
};