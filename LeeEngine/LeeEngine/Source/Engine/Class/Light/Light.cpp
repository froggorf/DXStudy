#include "CoreMinimal.h"
#include "Light.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/SceneProxy/FStaticMeshSceneProxy.h"

bool bInitLightAsset = false;
std::shared_ptr<FStaticMeshSceneProxy>	FLightInfo::LightVolumeMesh[static_cast<UINT>(ELightType::Count)];
std::shared_ptr<UMaterialInterface>		FLightInfo::LightMaterial[static_cast<UINT>(ELightType::Count)];

void FLightInfo::InitLight()
{
	// Directional Light
	{
		std::shared_ptr<UStaticMesh> DirectionalRectMesh = UStaticMesh::GetStaticMesh("SM_DeferredMergeRect");
		if (!DirectionalRectMesh)
		{
			AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()["SM_DeferredMergeRect"]);	
			DirectionalRectMesh = UStaticMesh::GetStaticMesh("SM_DeferredMergeRect");
		}
		LightVolumeMesh[static_cast<UINT>(ELightType::Directional)] = std::make_shared<FStaticMeshSceneProxy>(0,0,DirectionalRectMesh);
		LightMaterial[static_cast<UINT>(ELightType::Directional)] = UMaterial::GetMaterialCache("M_DirLight");	
	}
	
}

void FLightInfo::Render()
{
	if (!bInitLightAsset)
	{
		InitLight();
		bInitLightAsset = true;
	}

	if (LightVolumeMesh[LightType] && LightMaterial[LightType])
	{
		LightMaterial[LightType]->Binding();
		GDirectXDevice->SetRSState(ERasterizerType::RT_TwoSided);
		GDirectXDevice->SetDSState(EDepthStencilStateType::DST_NO_TEST_NO_WRITE);
		GDirectXDevice->SetBSState(EBlendStateType::BST_One_One);
		LightVolumeMesh[LightType]->Draw();
	}
}
