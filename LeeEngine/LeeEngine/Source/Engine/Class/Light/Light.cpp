#include "CoreMinimal.h"
#include "Light.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/SceneProxy/FStaticMeshSceneProxy.h"

bool bInitLightAsset = false;
std::shared_ptr<FStaticMeshSceneProxy>	FLightInfo::LightVolumeMesh[static_cast<UINT>(ELightType::Count)];
std::shared_ptr<UMaterialInterface>		FLightInfo::LightMaterial[static_cast<UINT>(ELightType::Count)];
std::shared_ptr<UMaterialInterface>		FLightInfo::LightVolumeMaterial;

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
		LightMaterial[static_cast<UINT>(ELightType::Directional)]->SetDepthStencilState(EDepthStencilStateType::DST_NO_TEST_NO_WRITE);
		LightMaterial[static_cast<UINT>(ELightType::Directional)]->SetBlendStateType(EBlendStateType::BST_One_One);
	}

	// Point Light
	{
		std::shared_ptr<UStaticMesh> SphereMesh = UStaticMesh::GetStaticMesh("SM_Sphere");
		if (!SphereMesh)
		{
			AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()["SM_Sphere"]);	
			SphereMesh = UStaticMesh::GetStaticMesh("SM_Sphere");
		}
		LightVolumeMesh[static_cast<UINT>(ELightType::Point)] = std::make_shared<FStaticMeshSceneProxy>(0,0,SphereMesh);
		LightMaterial[static_cast<UINT>(ELightType::Point)] = UMaterial::GetMaterialCache("M_PointLight");	
	}

	LightVolumeMaterial = UMaterial::GetMaterialCache("M_Volume");
	
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
		if (LightType == static_cast<int>(ELightType::Directional))
		{
			LightMaterial[LightType]->Binding();
			LightVolumeMesh[LightType]->Draw();
		}

		else
		{
			LightVolumeMesh[LightType]->SetSceneProxyWorldTransform(FTransform{{WorldPos.x,WorldPos.y,WorldPos.z}, {0,0,0,1}, {Radius/2,Radius/2,Radius/2}});
			LightVolumeMaterial->Binding();
			GDirectXDevice->SetRSState(ERasterizerType::RT_TwoSided);
			GDirectXDevice->SetBSState(EBlendStateType::BST_Default);
			GDirectXDevice->SetDSState(EDepthStencilStateType::VOLUME_CHECK,1);
			LightVolumeMesh[LightType]->Draw();


			LightMaterial[LightType]->Binding();
			GDirectXDevice->SetRSState(ERasterizerType::RT_CullFront); // 카메라가 광원 내부로 진입할 경우 대비
			GDirectXDevice->SetBSState(EBlendStateType::BST_One_One); // 빛이 누적되어야 하기 때문
			GDirectXDevice->SetDSState(EDepthStencilStateType::STENCIL_EQUAL, 0);
			LightVolumeMesh[LightType]->Draw();
		}
		
		
	}
}


std::shared_ptr<FStaticMeshSceneProxy> FDecalInfo::BoxSceneProxy;
void FDecalInfo::Render()
{
	if (!BoxSceneProxy)
	{
		std::shared_ptr<UStaticMesh> BoxMesh = UStaticMesh::GetStaticMesh("SM_Cube");
		if (!BoxMesh)
		{
			AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()["SM_Cube"]);	
			BoxMesh = UStaticMesh::GetStaticMesh("SM_Cube");
		}
		BoxSceneProxy = std::make_shared<FStaticMeshSceneProxy>(0,0,BoxMesh);
	}

	BoxSceneProxy->SetSceneProxyWorldTransform(Transform);
	DecalMaterial->Binding();
	const std::shared_ptr<UTexture>& PositionTargetTexture = UTexture::GetTextureCache("PositionTargetTex");
	GDirectXDevice->GetDeviceContext()->PSSetShaderResources(10,1,PositionTargetTexture->GetSRV().GetAddressOf());
	BoxSceneProxy->Draw();

}
