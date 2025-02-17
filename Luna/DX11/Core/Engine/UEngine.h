// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

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
	void InitEngine();
	void PostLoad() override;
	void Tick(float DeltaSeconds);
	void Draw();

	ID3D11Device* GetDevice() const {return Application->GetDevice();}
	ID3D11DeviceContext* GetDeviceContext() const {return Application->GetDeviceContext();}
	ID3D11Buffer* TestDeleteLater_GetObjConstantBuffer() const {return Application->Test_DeleteLater_GetObjectConstantBuffer();}
protected:
private:
	void LoadAllObjectsFromFile();
public:
protected:
private:
	D3DApp* Application;
	std::shared_ptr<UWorld> CurrentWorld;
	
};