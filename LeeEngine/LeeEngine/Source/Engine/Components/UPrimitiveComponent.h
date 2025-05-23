// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "USceneComponent.h"

class FPrimitiveSceneProxy;

class UPrimitiveComponent : public USceneComponent
{
	MY_GENERATE_BODY(UPrimitiveComponent)

	UPrimitiveComponent();
	~UPrimitiveComponent() override;

	void Register() override;

	virtual std::vector<std::shared_ptr<FPrimitiveSceneProxy>> CreateSceneProxy()
	{
		return std::vector<std::shared_ptr<FPrimitiveSceneProxy>>{};
	}

	virtual void RegisterSceneProxies();

	// 해당 프리미티브 머테리얼의 스칼라 파라미터를 변경하는 함수
	void SetScalarParam(UINT MeshIndex, const std::string& ParamName, float Value) const;
	// 해당 프리미티브 머테리얼의 텍스쳐 파라미터를 변경하는 함수
	void SetTextureParam(UINT MeshIndex, UINT TextureSlot, const std::shared_ptr<UTexture>& Texture) const;

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

protected:
	size_t RegisteredSceneProxyCount = 0;
};
