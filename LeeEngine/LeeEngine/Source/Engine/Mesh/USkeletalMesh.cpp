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
	for(auto& p : Textures)
	{
		p->Release();
	}
	Textures.clear();

	AssetManager::LoadSkeletalModelData(SkeletalMeshFilePathData["ModelData"], GDirectXDevice->GetDevice(), VertexBuffer,IndexBuffer, ModelBoneInfoMap);
	MeshCount = VertexBuffer.size();
	int TextureArraySize = SkeletalMeshFilePathData["Texture"].size();
	for(int count = 0; count < MeshCount; ++count)
	{
		if(TextureArraySize <= count)
		{
			/*MY_LOG(StaticMeshFilePathData["Name"], EDebugLogLevel::DLL_Warning, "Texture count not match with mesh count! ");*/
			break;
		}

		std::string TextureName = SkeletalMeshFilePathData["Texture"][count];
		std::shared_ptr<UTexture> MeshTexture = UTexture::GetTextureCache(TextureName);

		Textures.push_back(MeshTexture);
	}

	if(Textures.size() == 0)
	{
		std::string Name = SkeletalMeshFilePathData["Name"];
		MY_LOG(Name, EDebugLogLevel::DLL_Error, "In static mesh .myasset file, texture data no valid");
		return;
	}

	int currentTextureCount = Textures.size();
	for(; currentTextureCount < MeshCount; ++currentTextureCount)
	{
		Textures.push_back(Textures[0]);
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

	RenderData = std::make_unique<FSkeletalMeshRenderData>(SkeletalMeshAssetData);

	GetSkeletalMeshCacheMap()[GetName()] = shared_from_this();

	MY_LOG("Load", EDebugLogLevel::DLL_Warning, "Load USkeletalMesh - "+GetName() + " Complete!");
}
