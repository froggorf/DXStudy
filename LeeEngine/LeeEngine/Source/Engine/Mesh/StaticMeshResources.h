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
		for(auto& p : TextureSRV)
		{
			p->Release();
		}

		AssetManager::LoadModelData(StaticMeshFilePathData["ModelData"],GDirectXDevice->GetDevice(),VertexBuffer,IndexBuffer);
		MeshCount = VertexBuffer.size();
		int TextureArraySize = StaticMeshFilePathData["TextureData"].size();
		for(int count = 0; count < MeshCount; ++count)
		{
			if(TextureArraySize < count)
			{
				/*MY_LOG(StaticMeshFilePathData["Name"], EDebugLogLevel::DLL_Warning, "Texture count not match with mesh count! ");*/
				break;
			}

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
			const std::string RelativePath = StaticMeshFilePathData["TextureData"][count];
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


protected:
private:
public:
	unsigned int MeshCount;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	// TODO: 임시 텍스쳐 데이터,
	// UMaterial 구현 이전으로 텍스쳐 SRV 데이터를 해당 클래스에서 관리
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TextureSRV;
};
