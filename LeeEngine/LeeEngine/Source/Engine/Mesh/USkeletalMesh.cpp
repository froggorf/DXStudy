#include "CoreMinimal.h"
#include "Engine/AssetManager/AssetManager.h"
#include "USkeletalMesh.h"

#include "Engine/Class/UTexture.h"


FSkeletalMeshRenderData::FSkeletalMeshRenderData(const nlohmann::json& SkeletalMeshFilePathData)
{
	for(auto& p : VertexBuffer)
	{
		p->Release();
	}
	for(auto& p : IndexBuffer)
	{
		p->Release();
	}
	MaterialInterfaces.clear();

	AssetManager::LoadSkeletalModelData(SkeletalMeshFilePathData["ModelData"], GDirectXDevice->GetDevice(), VertexBuffer,IndexBuffer, ModelBoneInfoMap);
	MeshCount = VertexBuffer.size();

	// 머테리얼
	{
		auto MaterialData = SkeletalMeshFilePathData["Material"];
		int MaterialArraySize = MaterialData.size();
		for(int count = 0; count < MeshCount; ++count)
		{
			if(MaterialArraySize <= count)
			{
				break;
			}

			std::string MaterialName = MaterialData[count];
			std::shared_ptr<UMaterialInterface> MeshMaterial = UMaterialInterface::GetMaterialCache(MaterialName);

			MaterialInterfaces.emplace_back(MeshMaterial);
		}

		if(MaterialInterfaces.size() == 0)
		{
			std::string Name = SkeletalMeshFilePathData["Name"];
			MY_LOG(Name, EDebugLogLevel::DLL_Error, "In static mesh .myasset file, Material data no valid");
			return;
		}

		int currentTextureCount = MaterialInterfaces.size();
		for(; currentTextureCount < MeshCount; ++currentTextureCount)
		{
			MaterialInterfaces.push_back(MaterialInterfaces[0]);
		}	
	}
	
}

USkeletalMesh::USkeletalMesh()
{

}

USkeletalMesh::~USkeletalMesh()
{
}

enum class EStaticMeshAssetData
{
	ESMAD_Name = 0, ESMAD_ModelDataFilePath, ESMAD_TextureDataFilePath
};

void USkeletalMesh::LoadDataFromFileData(const nlohmann::json& SkeletalMeshAssetData)
{
	if(GetSkeletalMeshCacheMap().contains(GetName()))
	{
		MY_LOG("LoadData", EDebugLogLevel::DLL_Warning, "already load this SkeltalMesh -> " + GetName());
		return;
	}


	UObject::LoadDataFromFileData(SkeletalMeshAssetData);

	RenderData = std::make_shared<FSkeletalMeshRenderData>(SkeletalMeshAssetData);

	GetSkeletalMeshCacheMap()[GetName()] = shared_from_this();

	MY_LOG("Load", EDebugLogLevel::DLL_Warning, "Load USkeletalMesh - "+GetName() + " Complete!");
}
