#pragma once
#include "CoreMinimal.h"

class UMaterialInterface;
class FStaticMeshSceneProxy;

// LightType
enum class ELightType
{
	Directional, // 방향성 광원(전역 광원, 태양광)
	Point,		 // 점광원 (전구, 등불, 횃불)
	Spot,		 // 스포트 라이트 (조명)
	Count
};

struct FLightInfo
{
	int			LightType;		// 방향성, 점, 스포트라이트
	XMFLOAT3	WorldPos;		// 광원의 위치
	XMFLOAT3	WorldDir;		// 광원이 바라보는 방향
	XMFLOAT3	LightColor;		// 광원의 빛의 색상
	XMFLOAT3	LightAmbient;	// 광원으로 발생하는 최소한의 빛(환경광)
	float		Radius;			// 광원의 범위
	float		Angle;			// 스포트라이트 범위각도


	static std::shared_ptr<FStaticMeshSceneProxy>	LightVolumeMesh[static_cast<UINT>(ELightType::Count)];
	static std::shared_ptr<UMaterialInterface>		LightMaterial[static_cast<UINT>(ELightType::Count)];
	static std::shared_ptr<UMaterialInterface>		LightVolumeMaterial;

	void InitLight();
	void Render();

	
};


struct FDecalInfo
{
	int bIsLight;
	class UTexture* DecalTexture;

	FTransform Transform;
};