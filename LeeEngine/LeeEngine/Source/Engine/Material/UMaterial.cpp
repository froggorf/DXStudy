#include "CoreMinimal.h"
#include "UMaterial.h"

#include "Engine/UEngine.h"

std::unordered_map<std::string, std::shared_ptr<FShader>> FShader::ShaderCache;
std::unordered_map<std::string, std::shared_ptr<UMaterialInterface>> UMaterialInterface::MaterialCache;

void FVertexShader::CompileVertexShader(const std::string& FilePath, const std::string& FuncName)
{
	
	std::string TempDirectoryPath =  GEngine->GetDirectoryPath();
	std::wstring TempShaderPath = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath = TempShaderPath + std::wstring{FilePath.begin(),FilePath.end()};
	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "vs_4_0", VSBlob.GetAddressOf()));
	HR(GDirectXDevice->GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf()));
}

void FPixelShader::CompileVertexShader(const std::string& FilePath, const std::string& FuncName)
{
	std::string TempDirectoryPath =  GEngine->GetDirectoryPath();
	std::wstring TempShaderPath = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath = TempShaderPath + std::wstring{FilePath.begin(),FilePath.end()};

	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "ps_4_0", PSBlob.GetAddressOf()));

	HR(GDirectXDevice->GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf()));
}

void UMaterialInterface::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

}

void UMaterial::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UMaterialInterface::LoadDataFromFileData(AssetData);

	if(UMaterialInterface::MaterialCache.contains(GetName()))
	{
		// 이미 캐시를 진행한 머테리얼
		return;
	}

	// 버텍스 셰이더 로드
	{
		auto VertexShaderData = AssetData["VertexShader"];
		std::string VSName = VertexShaderData["FilePath"];
		std::string FuncName = VertexShaderData["Func"];
		auto VSTarget = FShader::ShaderCache.find(VSName+FuncName);
		if(VSTarget == FShader::ShaderCache.end())
		{
			std::shared_ptr<FVertexShader> NewVS = std::make_shared<FVertexShader>();
			NewVS->CompileVertexShader(VSName,FuncName);
			VSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{ VSName+FuncName, NewVS}).first;

		}
		VertexShader = std::dynamic_pointer_cast<FVertexShader>(VSTarget->second);
	}

	// 픽셀셰이더 로드
	{
		auto PixelShaderData = AssetData["PixelShader"];
		std::string PSName = PixelShaderData["FilePath"];
		std::string FuncName = PixelShaderData["Func"];
		auto PSTarget = FShader::ShaderCache.find(PSName+FuncName);
		if(PSTarget == FShader::ShaderCache.end())
		{
			std::shared_ptr<FPixelShader> NewPS = std::make_shared<FPixelShader>();
			NewPS->CompileVertexShader(PSName,FuncName);
			PSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{ PSName+FuncName, NewPS}).first;

		}
		PixelShader = std::dynamic_pointer_cast<FPixelShader>(PSTarget->second);
	}

	// 텍스쳐 정보가 있다면 텍스쳐 로드
	{
		if(AssetData.contains("Textures"))
		{
			auto TextureData = AssetData["Textures"];
			size_t TextureSize = TextureData.size();
			for(int i = 0 ; i< TextureSize; ++i)
			{
				std::string TextureName = TextureData[i];
				Textures.emplace_back(UTexture::GetTextureCache(TextureName));	
			}
			
		}
	}

	// 블렌드 모드
	{
		std::string BlendModeStr = AssetData["BlendMode"];
		if(BlendModeStr == "Opaque")
		{
			BlendMode = EBlendMode::BM_Opaque;
		}
		else if(BlendModeStr == "Masked")
		{
			BlendMode = EBlendMode::BM_Masked;
		}else if(BlendModeStr == "Translucent")
		{
			BlendMode = EBlendMode::BM_Translucent;
		}else
		{
			// 잘못된 BlendMode
			assert(0);
		}
	}

	UMaterialInterface::MaterialCache[GetName()] = shared_from_this();


}

void UMaterial::Binding()
{
	UMaterialInterface::Binding();

	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();
	for(int i = 0; i < Textures.size(); ++i)
	{
		DeviceContext->PSSetShaderResources(i,1, Textures[i]->GetSRV().GetAddressOf());	
	}
}
