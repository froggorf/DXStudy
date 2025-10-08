#include "CoreMinimal.h"
#include "Engine/AssetManager/AssetManager.h"
#include "USkeletalMesh.h"

#include "Engine/Animation/UAnimSequence.h"

FSkeletalMeshRenderData::FSkeletalMeshRenderData(const nlohmann::json& SkeletalMeshFilePathData)
{
	for (auto& p : VertexBuffer)
	{
		p->Release();
	}
	for (auto& p : IndexBuffer)
	{
		p->Release();
	}
	MaterialInterfaces.clear();

	AssetManager::LoadSkeletalModelData(SkeletalMeshFilePathData["ModelData"], GDirectXDevice->GetDevice(), VertexBuffer, IndexBuffer, ModelBoneInfoMap, BoundSphereRadius);
	MeshCount = static_cast<UINT>(VertexBuffer.size());

	// 머테리얼
	{
		auto   MaterialData      = SkeletalMeshFilePathData["Material"];
		size_t MaterialArraySize = MaterialData.size();
		for (UINT count = 0; count < MeshCount; ++count)
		{
			if (MaterialArraySize <= count)
			{
				break;
			}

			std::string                         MaterialName = MaterialData[count];
			std::shared_ptr<UMaterialInterface> MeshMaterial = UMaterialInterface::GetMaterialCache(MaterialName);

			if (!MeshMaterial)
			{
				assert(nullptr && "잘못된 머테리얼");
			}

			MaterialInterfaces.emplace_back(MeshMaterial);
		}

		if (MaterialInterfaces.size() == 0)
		{
			std::string Name = SkeletalMeshFilePathData["Name"];
			MY_LOG(Name, EDebugLogLevel::DLL_Error, "In static mesh .myasset file, Material data no valid");
			return;
		}

		UINT currentTextureCount = static_cast<UINT>(MaterialInterfaces.size());
		for (; currentTextureCount < MeshCount; ++currentTextureCount)
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

std::shared_ptr<USkeletalMesh> USkeletalMesh::GetSkeletalMesh(const std::string& SkeletalMeshName)
{
	return std::dynamic_pointer_cast<USkeletalMesh>(AssetManager::GetAssetCacheByName(SkeletalMeshName));
}

void USkeletalMesh::LoadDataFromFileData(const nlohmann::json& SkeletalMeshAssetData)
{
	UObject::LoadDataFromFileData(SkeletalMeshAssetData);

	RenderData = std::make_shared<FSkeletalMeshRenderData>(SkeletalMeshAssetData);

	if (SkeletalMeshAssetData.contains("DefaultPoseAnimData"))
	{
		DefaultAnimSequence = std::make_shared<UAnimSequence>();
		DefaultAnimSequence->ReadMyAssetFile(SkeletalMeshAssetData["DefaultPoseAnimData"], this);
		DefaultAnimSequence->Traverse();
	}
	
	

	MY_LOG("Load", EDebugLogLevel::DLL_Warning, "Load USkeletalMesh - "+GetName() + " Complete!");
}
