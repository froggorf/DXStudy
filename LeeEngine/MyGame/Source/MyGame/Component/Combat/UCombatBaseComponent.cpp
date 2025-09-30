#include "CoreMinimal.h"
#include "UCombatBaseComponent.h"

#include "Engine/RenderCore/EditorScene.h"

UCombatBaseComponent::UCombatBaseComponent()
{
	
}

void UCombatBaseComponent::SetBasicAttackMontages(const std::vector<std::string>& NewMontagesName)
{
	size_t Size = NewMontagesName.size();
	BasicAttackMontages.clear();
	BasicAttackMontages.resize(NewMontagesName.size());
	for (size_t Index = 0; Index < Size; ++Index)
	{
		BasicAttackMontages[Index] = UAnimMontage::GetAnimationAsset(NewMontagesName[Index]);
		if (!BasicAttackMontages[Index])
		{
			MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", BasicAttackMontages (Index) Montage Not exist - "+NewMontagesName[Index]);
		}
	}
	
}

