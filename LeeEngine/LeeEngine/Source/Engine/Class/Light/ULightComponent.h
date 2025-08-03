// 07.29
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Light.h"
#include "Engine/Components/UPrimitiveComponent.h"

class ULightComponent : public USceneComponent
{
	MY_GENERATE_BODY(ULightComponent)


	void Register() override;

	void SetLightType(ELightType _Type);
	ELightType GetLightType() { return (ELightType)Info.LightType; }

	void SetLightColor(XMFLOAT3 Color) { Info.LightColor = Color; }
	XMFLOAT3 GetLightColor() const { return Info.LightColor; }

	void SetLightAmbient(XMFLOAT3 Ambient) { Info.LightAmbient = Ambient; }
	XMFLOAT3 GetLightAmbient() const { return Info.LightAmbient; }

	void SetRadius(float Radius);
	float GetRadius() const { return Info.Radius; }

	void SetAngle(float Angle) { Info.Angle = Angle; }
	float GetAngle() const { return Info.Angle; }


	void TickComponent(float DeltaSeconds) override;
	void Tick_Editor(float DeltaSeconds) override;

	void SetActive(bool NewActive){bActive = NewActive;}
	bool GetActive() const {return bActive;}
private:
	void AddLightInfo();


	FLightInfo Info;
	bool bActive = true;
};



class UDecalComponent : public USceneComponent
{
	MY_GENERATE_BODY(UDecalComponent)

public:
	

	const std::shared_ptr<UMaterialInterface>& GetDecalMaterial() const {return DecalMaterial;}
	void SetDecalMaterial(const std::shared_ptr<UMaterialInterface>& NewDecalMaterial) { DecalMaterial = NewDecalMaterial; }
	void SetIsLight(bool NewIsLight) { bIsLight = NewIsLight;}
	void SetActive(bool NewActive) { bIsActive = NewActive;}

	void TickComponent(float DeltaSeconds) override;
	void Tick_Editor(float DeltaSeconds) override;

private:
	std::shared_ptr<UMaterialInterface> DecalMaterial;
	bool					  bIsLight = false;
	bool bIsActive = true;
	void AddDecalInfo();
};