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

	unsigned int MeshCount = 0;

	// Vertex Data [Meshcount] -> 버텍스들 벡터
	std::vector<std::vector<MyVertexData>> VertexData;
	std::vector<std::vector<UINT>> IndexData;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	// 머테리얼 정보
	std::vector<std::shared_ptr<UMaterialInterface>> Materials;

	// 비동기 에셋 로드 이후부터 로드가 완료되기 이전 렌더링을 요청 시 에셋 로드 중 렌더링이 진행되어 런타임 에러가 발생
	// 따라서 로드에 성공했는지를 두어 관리
	std::atomic<bool> bLoadSuccess = false;

	float BoundSphereRadius = 0;
};
