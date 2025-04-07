#include "CoreMinimal.h"

#include "UAimOffsetBlendSpace.h"


UAimOffsetBlendSpace::UAimOffsetBlendSpace()
{

}

void UAimOffsetBlendSpace::GetAnimationBoneMatrices(const XMFLOAT2& AnimValue, float CurrentAnimTime,
	std::vector<XMMATRIX>& OutMatrices, std::vector<FAnimNotifyEvent>& OutActiveNotifies)
{
	std::vector<XMMATRIX> AO_Matrices(MAX_BONES, XMMatrixIdentity());
	UBlendSpace::GetAnimationBoneMatrices(AnimValue, CurrentAnimTime, OutMatrices,OutActiveNotifies);

	for (int i = 0; i < MAX_BONES; ++i)
	{
		XMMATRIX InverseDefaultAnimMatrices = XMMatrixInverse(nullptr, DefaultAnimMatrices[i]);
		OutMatrices[i] = InverseDefaultAnimMatrices * AO_Matrices[i];
	}
	
}

void UAimOffsetBlendSpace::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UBlendSpace::LoadDataFromFileData(AssetData);

	DefaultAnimMatrices = std::vector<XMMATRIX>(MAX_BONES, XMMatrixIdentity());
	std::vector<FAnimNotifyEvent> Dummy;
	UBlendSpace::GetAnimationBoneMatrices(XMFLOAT2{(HorizontalValue.x+HorizontalValue.y)/2, (VerticalValue.x+VerticalValue.y)/2}, 0.0f, DefaultAnimMatrices,Dummy);
}
