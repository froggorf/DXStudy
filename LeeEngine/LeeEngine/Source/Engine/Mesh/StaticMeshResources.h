// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once


#include "Engine/MyEngineUtils.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/DirectX/Device.h"

class FStaticMeshRenderData
{
public:
	FStaticMeshRenderData(const nlohmann::json& StaticMeshFilePathData)
	{
		for(auto& p : VertexBuffer)
		{
			p->Release();
		}
		for(auto& p : IndexBuffer)
		{
			p->Release();
		}
		Materials.clear();

		AssetManager::LoadModelData(StaticMeshFilePathData["ModelData"],GDirectXDevice->GetDevice(),VertexBuffer,IndexBuffer);
		MeshCount = VertexBuffer.size();
		

		// 머테리얼
		{
			auto MaterialData = StaticMeshFilePathData["Material"];
			int MaterialArraySize = MaterialData.size();
			for(int count = 0; count < MeshCount; ++count)
			{
				if(MaterialArraySize <= count)
				{
					break;
				}

				std::string MaterialName = MaterialData[count];
				std::shared_ptr<UMaterialInterface> MeshMaterial = UMaterialInterface::GetMaterialCache(MaterialName);

				Materials.emplace_back(MeshMaterial);
			}

			if(Materials.size() == 0)
			{
				std::string Name = StaticMeshFilePathData["Name"];
				MY_LOG(Name, EDebugLogLevel::DLL_Error, "In static mesh .myasset file, Material data no valid");
				return;
			}

			int currentTextureCount = Materials.size();
			for(; currentTextureCount < MeshCount; ++currentTextureCount)
			{
				Materials.push_back(Materials[0]);
			}	
		}
	}


protected:
private:
public:
	unsigned int MeshCount;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	// 머테리얼 정보
	std::vector<std::shared_ptr<UMaterialInterface>> Materials;
	
};
