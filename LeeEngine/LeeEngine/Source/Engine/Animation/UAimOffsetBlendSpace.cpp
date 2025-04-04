#include "CoreMinimal.h"

#include "UAimOffsetBlendSpace.h"


UAimOffsetBlendSpace::UAimOffsetBlendSpace()
{

}

void UAimOffsetBlendSpace::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UBlendSpace::LoadDataFromFileData(AssetData);
}
