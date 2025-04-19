#include "CoreMinimal.h"
#include "UMaterial.h"

void UMaterialInterface::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

}
