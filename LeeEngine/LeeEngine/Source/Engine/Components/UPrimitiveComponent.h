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


protected:
	
private:

public:
protected:
private:


};
