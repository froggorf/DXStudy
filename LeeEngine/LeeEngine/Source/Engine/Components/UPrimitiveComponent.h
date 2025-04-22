// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "USceneComponent.h"

class FPrimitiveSceneProxy;

class UPrimitiveComponent : public USceneComponent
{
	MY_GENERATED_BODY(UPrimitiveComponent)

public:
	UPrimitiveComponent();
	~UPrimitiveComponent() override;

	virtual void Register() override;

	virtual std::vector< std::shared_ptr<FPrimitiveSceneProxy> > CreateSceneProxy() const
	{
		return std::vector<std::shared_ptr<FPrimitiveSceneProxy>>{};
	}

	void SetScalarParam(UINT MeshIndex, const std::string& ParamName, float Value) const;

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override
	{
		static float NewSpeedX = 1, NewSpeedY = 1;
		if(ImGui::SliderFloat("NewSpeedX", &NewSpeedX, -1.0f, 1.0f))
		{
			SetScalarParam(0, "WaterSpeedX", NewSpeedX);
		}
		if(ImGui::SliderFloat("NewSpeedY", &NewSpeedY, -1.0f, 1.0f))
		{
			SetScalarParam(0, "WaterSpeedY", NewSpeedY);
		}
	}
#endif

protected:
	
private:

public:
protected:
private:


};
