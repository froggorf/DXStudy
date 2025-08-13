// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "USceneComponent.h"
#include "Engine/Physics/UPhysicsEngine.h"
#include "Engine/Misc/Delegate.h"

class UShapeComponent;
class FPrimitiveSceneProxy;


#define FComponentHitSignature Delegate<UShapeComponent*, AActor*, UShapeComponent*, const FHitResult& /*Normal Impulse는 생략*/>
#define FComponentBeginOverlapSignature Delegate<UShapeComponent*, AActor*, UShapeComponent*>
#define FComponentEndOverlapSignature Delegate<UShapeComponent*, AActor*, UShapeComponent*>


class UPrimitiveComponent : public USceneComponent
{
	MY_GENERATE_BODY(UPrimitiveComponent)

	UPrimitiveComponent();
	~UPrimitiveComponent() override = default;

	void Register() override;
	void UnRegister() override;
	virtual std::vector<std::shared_ptr<FPrimitiveSceneProxy>> CreateSceneProxy()
	{
		return std::vector<std::shared_ptr<FPrimitiveSceneProxy>>{};
	}

	virtual void RegisterSceneProxies();


	// 해당 프리미티브 머테리얼의 스칼라 파라미터를 변경하는 함수
	void SetScalarParam(UINT MeshIndex, const std::string& ParamName, float Value) const;
	// 해당 프리미티브 머테리얼의 텍스쳐 파라미터를 변경하는 함수
	void SetTextureParam(UINT MeshIndex, UINT TextureSlot, const std::shared_ptr<UTexture>& Texture) const;

	// 하위클래스에서 적합한 BodyInstance를 생성하도록 재정의 해줘야함
	// ex) UStaticMeshComponent에서는 UConvexComponent를 생성하도록 재정의함
	virtual std::shared_ptr<UShapeComponent> CreateBodyInstance();

	// 콜리젼 오브젝트 타입을 변경하는 함수
	void SetCollisionObjectType(ECollisionChannel Channel);
	// 특정 채널에 대해서 콜리젼 반응을 설정하는 함수
	void SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse);

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

	std::shared_ptr<UShapeComponent> GetBodyInstance() const {return BodyInstance;}

	// CollisionEnabled
	void SetCollisionEnabled(ECollisionEnabled NewType);

	// OnComponentHit 델리게이트
	FComponentHitSignature OnComponentHit;
	// OnBeginOverlap
	FComponentBeginOverlapSignature OnComponentBeginOverlap;
	// OnComponentEndOverlap
	FComponentEndOverlapSignature OnComponentEndOverlap;

	// TODO: 08.13 현재는 초기 설정값만 적용되도록 했는데 추후에는 런타임 중에도 적용할 수 있도록 변경해야함
	void SetDoFrustumCulling(bool NewDoFrustumCulling){bDoFrustumCulling = NewDoFrustumCulling;}
protected:
	size_t RegisteredSceneProxyCount = 0;

	std::shared_ptr<UShapeComponent> BodyInstance;

	// 생성자에서 ObjectChannel 이나 Response를 바꿀경우에 BodyInstance가 생성되기 이전이라서 적용이 안되기때문에
	// 임시변수에 값을 저장해놓고 생성시 적용하는 방식으로 구현
	ECollisionChannel TempCollisionChannel;
	std::array<ECollisionResponse, static_cast<UINT>(ECollisionChannel::Count)> TempCollisionResponse = {};

	bool bDoFrustumCulling = true;
};
