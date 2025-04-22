#include "CoreMinimal.h"
#include "UMaterial.h"

#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"

std::unordered_map<std::string, std::shared_ptr<FShader>> FShader::ShaderCache;
std::unordered_map<std::string, std::shared_ptr<UMaterialInterface>> UMaterialInterface::MaterialCache;

void FShader::SetShaderID(UINT NewID)
{
	ShaderID = NewID;
}

void FVertexShader::CompileVertexShader(const std::string& FilePath, const std::string& FuncName)
{
	
	std::string TempDirectoryPath =  GEngine->GetDirectoryPath();
	std::wstring TempShaderPath = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath = TempShaderPath + std::wstring{FilePath.begin(),FilePath.end()};
	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "vs_4_0", VSBlob.GetAddressOf()));
	HR(GDirectXDevice->GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf()));
}

void FPixelShader::CompilePixelShader(const std::string& FilePath, const std::string& FuncName)
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
		std::string InputLayoutType = VertexShaderData["InputLayoutType"];
		auto VSTarget = FShader::ShaderCache.find(VSName+FuncName);
		if(VSTarget == FShader::ShaderCache.end())
		{
			std::shared_ptr<FVertexShader> NewVS = std::make_shared<FVertexShader>();
			NewVS->CompileVertexShader(VSName,FuncName);
			if(InputLayoutType == "StaticMesh")
			{
				NewVS->InputLayoutType = EInputLayoutType::ILT_StaticMesh;	
			}
			else if(InputLayoutType == "SkeletalMesh")
			{
				NewVS->InputLayoutType = EInputLayoutType::ILT_SkeletalMesh;	
			}
			else
			{
				// 잘못된 데이터
				assert(0);
			}

			VSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{ VSName+FuncName, NewVS}).first;
			VSTarget->second->SetShaderID(FShader::ShaderCache.size());
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
			NewPS->CompilePixelShader(PSName,FuncName);
			PSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{ PSName+FuncName, NewPS}).first;
			PSTarget->second->SetShaderID(FShader::ShaderCache.size());
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

	// Rasterizer State
	{
		int bIsTwoSided = AssetData["TwoSided"];
		if(bIsTwoSided)
		{
			RasterizerType = ERasterizerType::RT_TwoSided;	
		}
		else
		{
			RasterizerType = ERasterizerType::RT_CullBack;
		}
	}

	// Params
	if(AssetData.contains("Params"))
	{
		auto ParamData = AssetData["Params"];

		for(const auto& Data : ParamData)
		{
			std::string Type = Data["Type"];
			if("float" == Type)
			{
				FMaterialParameterDesc<float> FloatParam;
				FloatParam.Name = Data["Name"];
				FloatParam.Offset = Data["Offset"];
				FloatParam.Size = 4;
				FloatParam.DefaultValue = Data["DefaultValue"];
				Params.FloatParams.emplace_back(FloatParam);
			}
			else if("int" == Type)
			{
				FMaterialParameterDesc<int> IntParam;
				IntParam.Name = Data["Name"];
				IntParam.Offset = Data["Offset"];
				IntParam.Size = 4;
				IntParam.DefaultValue = Data["DefaultValue"];
				Params.IntParams.emplace_back(IntParam);
			}
			else
			{
				// 04.22 float 와 int 만 파라미터로 되도록
				assert(0);
			}
		}
		Params.TotalSize = AssetData["TotalSize"];

		if(Params.TotalSize > 0)
		{
			// Constant Buffer 생성
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.ByteWidth = Params.TotalSize;
			HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ParamConstantBuffer.GetAddressOf()));	
		}
		
	}

	UMaterialInterface::MaterialCache[GetName()] = shared_from_this();
	MaterialID = MaterialCache.size();
}

void UMaterial::Binding()
{
	UMaterialInterface::Binding();

	GDirectXDevice->SetVertexShader(VertexShader.get());
	GDirectXDevice->SetPixelShader(PixelShader.get());

	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();
	for(int i = 0; i < Textures.size(); ++i)
	{
		DeviceContext->PSSetShaderResources(i,1, Textures[i]->GetSRV().GetAddressOf());	
	}

	MapAndBindParameterConstantBuffer();

	GDirectXDevice->SetInputLayout(GetInputLayoutType());
}

void UMaterial::MapAndBindParameterConstantBuffer() const
{
	// 파라미터가 없음
	if(Params.TotalSize == 0)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	HR(GDirectXDevice->GetDeviceContext()->Map(ParamConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));

	for(const auto& FloatParam : Params.FloatParams)
	{
		memcpy(static_cast<char*>(cbMapSub.pData) + FloatParam.Offset, (&FloatParam.DefaultValue), FloatParam.Size);	
	}
	for(const auto& IntParam : Params.IntParams)
	{
		memcpy(static_cast<char*>(cbMapSub.pData)+ IntParam.Offset, &IntParam.DefaultValue, IntParam.Size);	
	}

	GDirectXDevice->GetDeviceContext()->Unmap(ParamConstantBuffer.Get(),0);

	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_UserParam),1,ParamConstantBuffer.GetAddressOf());
	
}

void UMaterialInstance::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UMaterialInterface::LoadDataFromFileData(AssetData);

	std::string ParentMaterialName = AssetData["ParentMaterial"];
	ParentMaterial = std::dynamic_pointer_cast<UMaterial>(UMaterial::GetMaterialCache(ParentMaterialName));
	if(!ParentMaterial)
	{
		// 잘못된 부모 머테리얼
		assert(0);
	}
	UMaterialInterface::MaterialCache[GetName()] = shared_from_this();
}

void UMaterialInstance::Binding()
{
	UMaterialInterface::Binding();

	// 머테리얼 인스턴스로만 SceneProxy가 이루어져 있을 수 있으므로 바인딩을 진행 (Pipeline State가 중복적으로 값을 설정하지는 않음)
	if(ParentMaterial)
	{
		ParentMaterial->Binding();
	}

	
}
