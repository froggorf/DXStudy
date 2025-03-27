#include "USkeletalMesh.h"


FTest::FTest(const nlohmann::json& SkeletalMeshFilePathData)
{
	for(auto& p : VertexBuffer)
	{
		p->Release();
	}
	for(auto& p : IndexBuffer)
	{
		p->Release();
	}
	for(auto& p : TextureSRV)
	{
		p->Release();
	}

	AssetManager::LoadSkeletalModelData(SkeletalMeshFilePathData["ModelData"], GDirectXDevice->GetDevice(), VertexBuffer,IndexBuffer, ModelBoneInfoMap);
	MeshCount = VertexBuffer.size();
	int TextureArraySize = SkeletalMeshFilePathData["TextureData"].size();
	for(int count = 0; count < MeshCount; ++count)
	{
		if(TextureArraySize < count)
		{
			/*MY_LOG(StaticMeshFilePathData["Name"], EDebugLogLevel::DLL_Warning, "Texture count not match with mesh count! ");*/
			break;
		}

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
		const std::string RelativePath = SkeletalMeshFilePathData["TextureData"][count];
		const std::string TextureFilePath = GEngine->GetDirectoryPath() + RelativePath;
		AssetManager::LoadTextureFromFile(std::wstring().assign(TextureFilePath.begin(),TextureFilePath.end()), GDirectXDevice->GetDevice(), SRV);
		TextureSRV.push_back(SRV);
	}

	if(TextureSRV.size() == 0)
	{
		/*MY_LOG(StaticMeshFilePathData["Name"], EDebugLogLevel::DLL_Error, "In static mesh .myasset file, texture data no valid");*/
		return;
	}

	int currentTextureCount = TextureSRV.size();
	for(; currentTextureCount < MeshCount; ++currentTextureCount)
	{
		TextureSRV.push_back(TextureSRV[0]);
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

	RenderData = std::make_unique<FTest>(SkeletalMeshAssetData);

	GetSkeletalMeshCacheMap()[GetName()] = shared_from_this();

	MY_LOG("Load", EDebugLogLevel::DLL_Warning, "Load USkeletalMesh - "+GetName() + " Complete!");
}
