#include "CoreMinimal.h"
#include "StaticMeshResources.h"

FStaticMeshRenderData::FStaticMeshRenderData(const nlohmann::json& StaticMeshFilePathData)
{

	if(StaticMeshFilePathData["Name"].contains("Async"))
	{
		int a = 0;
	}
	for (auto& p : VertexBuffer)
	{
		p->Release();
	}
	for (auto& p : IndexBuffer)
	{
		p->Release();
	}
	Materials.clear();

	AssetManager::LoadModelData(StaticMeshFilePathData["ModelData"], GDirectXDevice->GetDevice(), VertexData,IndexData, VertexBuffer, IndexBuffer,BoundSphereRadius);
	const std::string& Name = StaticMeshFilePathData["ModelData"];
	MeshCount = static_cast<UINT>(VertexBuffer.size());

	// 머테리얼
	{
		auto MaterialData      = StaticMeshFilePathData["Material"];
		UINT MaterialArraySize = static_cast<UINT>(MaterialData.size());
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
				assert(0 && "잘못된 머테리얼 이름");
			}
			Materials.emplace_back(MeshMaterial);
		}

		if (Materials.size() == 0)
		{
			std::string Name = StaticMeshFilePathData["Name"];
			MY_LOG(Name, EDebugLogLevel::DLL_Error, "In static mesh .myasset file, Material data no valid");
			return;
		}

		UINT currentTextureCount = static_cast<UINT>(Materials.size());
		for (; currentTextureCount < MeshCount; ++currentTextureCount)
		{
			Materials.push_back(Materials[0]);
		}
	}

	bLoadSuccess = true;
}
