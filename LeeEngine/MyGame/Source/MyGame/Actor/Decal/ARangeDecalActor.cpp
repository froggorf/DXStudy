#include "CoreMinimal.h"
#include "ARangeDecalActor.h"

#include "Engine/Class/Light/ULightComponent.h"

std::shared_ptr<UMaterialInterface> ARangeDecalActor::MI_SkillRangeDecal = nullptr;

ARangeDecalActor::ARangeDecalActor()
{
	if (!MI_SkillRangeDecal)
	{
		MI_SkillRangeDecal = UMaterial::GetMaterialCache("MI_SkillRangeDecal");
	}

	DecalComp =std::make_shared<UDecalComponent>();
	DecalComp->SetupAttachment(GetRootComponent());
	DecalComp->SetDecalMaterial(MI_SkillRangeDecal);
	DecalComp->SetIsLight(true);
}

ARangeDecalActor* ARangeDecalActor::SetProgress(float NewProgress)
{
	NewProgress = std::clamp(NewProgress, 0.0f, 1.0f);
	DecalComp->GetDecalMaterial()->SetScalarParam("Progress", NewProgress);

	return this;
}

ARangeDecalActor* ARangeDecalActor::SetBaseColor(const XMFLOAT3& NewBaseColor)
{
	DecalComp->GetDecalMaterial()->SetScalarParam("BaseColorR", NewBaseColor.x);
	DecalComp->GetDecalMaterial()->SetScalarParam("BaseColorG", NewBaseColor.y);
	DecalComp->GetDecalMaterial()->SetScalarParam("BaseColorB", NewBaseColor.z);

	return this;
}

ARangeDecalActor* ARangeDecalActor::SetActiveColor(const XMFLOAT3& ActiveColor)
{
	DecalComp->GetDecalMaterial()->SetScalarParam("ActiveColorR", ActiveColor.x);
	DecalComp->GetDecalMaterial()->SetScalarParam("ActiveColorG", ActiveColor.y);
	DecalComp->GetDecalMaterial()->SetScalarParam("ActiveColorB", ActiveColor.z);

	return this;
}

ARangeDecalActor* ARangeDecalActor::SetHalfAngleDeg(float NewHalfAngleDeg)
{
	NewHalfAngleDeg = std::clamp(NewHalfAngleDeg, 0.0f, 180.0f);
	DecalComp->GetDecalMaterial()->SetScalarParam("HalfAngleDeg", NewHalfAngleDeg);

	return this;
}

ARangeDecalActor* ARangeDecalActor::SetForward(const XMFLOAT3& NewForward)
{
	XMFLOAT3 NormalizedForward;
	XMStoreFloat3(&NormalizedForward, XMVector3Normalize(XMVectorSet(NewForward.x,0.0f,NewForward.z,0.0f)));
	DecalComp->GetDecalMaterial()->SetScalarParam("ForwardX", NormalizedForward.x);
	DecalComp->GetDecalMaterial()->SetScalarParam("ForwardZ", NormalizedForward.z);

	return this;
}

ARangeDecalActor* ARangeDecalActor::SetForward(const XMFLOAT2& NewForward)
{
	XMVECTOR NormalizedForward = XMVector2Normalize(XMLoadFloat2(&NewForward));
	DecalComp->GetDecalMaterial()->SetScalarParam("ForwardX", XMVectorGetX(NormalizedForward));
	DecalComp->GetDecalMaterial()->SetScalarParam("ForwardZ",XMVectorGetY(NormalizedForward));

	return this;
}
