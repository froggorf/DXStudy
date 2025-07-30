#include "CoreMinimal.h"
#include "UMaterial.h"
#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

using namespace Microsoft::WRL;
UINT                                                      UMaterial::MaterialIDCount = 0;
std::unordered_map<std::string, std::shared_ptr<FShader>> FShader::ShaderCache;

void FShader::SetShaderID(UINT NewID)
{
	ShaderID = NewID;
}

void FVertexShader::CompileVertexShader(const std::string& FilePath, const std::string& FuncName)
{
	std::string  TempDirectoryPath = GEngine->GetDirectoryPath();
	auto         TempShaderPath    = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath    = TempShaderPath + std::wstring{FilePath.begin(), FilePath.end()};
	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "vs_4_0", VSBlob.GetAddressOf()));
	if (VSBlob == nullptr)
	{
		assert(0 && "VSBlob nullptr");
	}
	HR(GDirectXDevice->GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf()));
	
	// Input Layout

	D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT                     numElements   = ARRAYSIZE(inputLayout);
	HR(GDirectXDevice->GetDevice()->CreateInputLayout(inputLayout, numElements, VSBlob->GetBufferPointer(), VSBlob-> GetBufferSize(), InputLayout.GetAddressOf()));
}

void FPixelShader::CompilePixelShader(const std::string& FilePath, const std::string& FuncName)
{
	std::string  TempDirectoryPath = GEngine->GetDirectoryPath();
	auto         TempShaderPath    = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath    = TempShaderPath + std::wstring{FilePath.begin(), FilePath.end()};

	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "ps_4_0", PSBlob.GetAddressOf()));

	HR(GDirectXDevice->GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf()));
	if (PSBlob == nullptr)
	{
		assert(0 && "psblob nullptr");
	}
}

std::shared_ptr<UMaterialInterface> UMaterialInterface::GetMaterialCache(const std::string& MaterialName)
{
	return std::dynamic_pointer_cast<UMaterialInterface>(AssetManager::GetAssetCacheByName(MaterialName));
}

void UMaterialInterface::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);
}

void UMaterial::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UMaterialInterface::LoadDataFromFileData(AssetData);
	if (AssetData["Name"] == "M_DeferredSphere")
	{
		int a = 0;
	}
	// 버텍스 셰이더 로드
	{
		auto        VertexShaderData = AssetData["VertexShader"];
		std::string VSName           = VertexShaderData["FilePath"];
		std::string FuncName         = VertexShaderData["Func"];
		auto        VSTarget         = FShader::ShaderCache.find(VSName + FuncName);
		if (VSTarget == FShader::ShaderCache.end())
		{
			auto NewVS = std::make_shared<FVertexShader>();
			NewVS->CompileVertexShader(VSName, FuncName);

			VSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{VSName + FuncName, NewVS}).first;
			VSTarget->second->SetShaderID(static_cast<UINT>(FShader::ShaderCache.size()));
		}
		VertexShader = std::dynamic_pointer_cast<FVertexShader>(VSTarget->second);
	}

	// 픽셀셰이더 로드
	{
		// 07.30 : 픽셀셰이더는 존재하지 않는 머테리얼이 있을수도 있으므로
		if (AssetData.contains("PixelShader"))
		{
			auto        PixelShaderData = AssetData["PixelShader"];
			std::string PSName          = PixelShaderData["FilePath"];
			std::string FuncName        = PixelShaderData["Func"];
			auto        PSTarget        = FShader::ShaderCache.find(PSName + FuncName);
			if (PSTarget == FShader::ShaderCache.end())
			{
				auto NewPS = std::make_shared<FPixelShader>();
				NewPS->CompilePixelShader(PSName, FuncName);
				PSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{PSName + FuncName, NewPS}).first;
				PSTarget->second->SetShaderID(static_cast<UINT>(FShader::ShaderCache.size()));
			}
			PixelShader = std::dynamic_pointer_cast<FPixelShader>(PSTarget->second);
		}
		
	}

	// GeometryShader 로드
	{
		if (AssetData.contains("GeometryShader"))
		{
			auto        GeometryShaderData = AssetData["GeometryShader"];
			std::string GSName             = GeometryShaderData["FilePath"];
			std::string FuncName           = GeometryShaderData["Func"];
			auto        GSTarget           = FShader::ShaderCache.find(GSName + FuncName);
			if (GSTarget == FShader::ShaderCache.end())
			{
				auto NewGS = std::make_shared<FGeometryShader>();
				NewGS->CompileGeometryShader(GSName, FuncName);
				GSTarget = FShader::ShaderCache.insert(std::pair<std::string, std::shared_ptr<FShader>>{GSName + FuncName, NewGS}).first;
				GSTarget->second->SetShaderID(static_cast<UINT>(FShader::ShaderCache.size()));
			}
			GeometryShader = std::dynamic_pointer_cast<FGeometryShader>(GSTarget->second);
		}
	}

	// 텍스쳐 정보가 있다면 텍스쳐 로드
	{
		if (AssetData.contains("Textures") && AssetData["Textures"].size() != 0)
		{
			auto   TextureData = AssetData["Textures"];
			size_t TextureSize = TextureData.size();
			for (int i = 0; i < TextureSize; ++i)
			{
				std::string TextureName = TextureData[i];
				Textures.emplace_back(UTexture::GetTextureCache(TextureName));
			}
		}
	}

	// 블렌드 모드
	{
		std::string BlendModeStr = AssetData["BlendMode"];
		if (BlendModeStr == "Opaque")
		{
			BlendMode = EBlendMode::BM_Opaque;
		}
		else if (BlendModeStr == "Masked")
		{
			BlendMode = EBlendMode::BM_Masked;
		}
		else if (BlendModeStr == "Translucent")
		{
			BlendMode = EBlendMode::BM_Translucent;
		}
		else if (BlendModeStr == "Deferred")
		{
			BlendMode = EBlendMode::BM_Deferred;
		}
		else
		{
			// 잘못된 BlendMode
			//assert(0 && "잘못된 BlendMode");
		}
	}

	// Rasterizer State
	{
		int bIsTwoSided = AssetData["TwoSided"];
		if (bIsTwoSided)
		{
			RasterizerType = ERasterizerType::RT_TwoSided;
		}
		else
		{
			RasterizerType = ERasterizerType::RT_CullBack;
		}
	}

	// Params
	if (AssetData.contains("Params"))
	{
		auto ParamData = AssetData["Params"];

		for (const auto& Data : ParamData)
		{
			std::string Type = Data["Type"];
			if ("float" == Type)
			{
				FMaterialParameterDesc<float> FloatParam;
				FloatParam.Name   = Data["Name"];
				FloatParam.Offset = Data["Offset"];
				FloatParam.Size   = 4;
				FloatParam.Value  = Data["DefaultValue"];
				DefaultParams.FloatParams.emplace_back(FloatParam);
			}
			else if ("int" == Type)
			{
				FMaterialParameterDesc<int> IntParam;
				IntParam.Name   = Data["Name"];
				IntParam.Offset = Data["Offset"];
				IntParam.Size   = 4;
				IntParam.Value  = Data["DefaultValue"];
				DefaultParams.IntParams.emplace_back(IntParam);
			}
			else
			{
				// 04.22 float 와 int 만 파라미터로 되도록
				assert(0);
			}
		}
		DefaultParams.TotalSize = AssetData["TotalSize"];

		if (DefaultParams.TotalSize > 0)
		{
			// Constant Buffer 생성
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags         = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags    = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.ByteWidth         = DefaultParams.TotalSize;
			HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ParamConstantBuffer.GetAddressOf()));
		}
	}

	if (AssetData.contains("TextureParams") && AssetData["TextureParams"].size() != 0)
	{
		auto   TextureParamData = AssetData["TextureParams"];
		size_t TextureParamSize = TextureParamData.size();
		for (int i = 0; i < TextureParamSize; ++i)
		{
			std::string TextureName = TextureParamData[i];
			TextureParams.emplace_back(UTexture::GetTextureCache(TextureName));
		}
	}
	MaterialID = MaterialIDCount++;
}

void UMaterial::Binding()
{
	UMaterialInterface::Binding();

	GDirectXDevice->SetVertexShader(VertexShader.get());
	GDirectXDevice->SetPixelShader(PixelShader.get());
	GDirectXDevice->SetGeometryShader(GeometryShader.get());

	GDirectXDevice->SetRSState(GetRSType());
	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();

	for (int i = 0; i < Textures.size(); ++i)
	{
		DeviceContext->PSSetShaderResources(i, 1, Textures[i]->GetSRV().GetAddressOf());
	}
	for (int i = 0; i < TextureParams.size(); ++i)
	{
		DeviceContext->PSSetShaderResources(static_cast<UINT>(Textures.size() + i), 1, TextureParams[i]->GetSRV().GetAddressOf());
	}

	MapAndBindParameterConstantBuffer();
}

void UMaterial::MapAndBindParameterConstantBuffer() const
{
	// 파라미터가 없음
	if (DefaultParams.TotalSize == 0)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	HR(GDirectXDevice->GetDeviceContext()->Map(ParamConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, & cbMapSub));

	for (const auto& FloatParam : DefaultParams.FloatParams)
	{
		memcpy(static_cast<char*>(cbMapSub.pData) + FloatParam.Offset, (&FloatParam.Value), FloatParam.Size);
	}
	for (const auto& IntParam : DefaultParams.IntParams)
	{
		memcpy(static_cast<char*>(cbMapSub.pData) + IntParam.Offset, &IntParam.Value, IntParam.Size);
	}

	GDirectXDevice->GetDeviceContext()->Unmap(ParamConstantBuffer.Get(), 0);

	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_UserParam), 1, ParamConstantBuffer.GetAddressOf());
}

void UMaterialInstance::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UMaterialInterface::LoadDataFromFileData(AssetData);

	std::string ParentMaterialName = AssetData["ParentMaterial"];
	ParentMaterial                 = std::dynamic_pointer_cast<UMaterial>(GetMaterialCache(ParentMaterialName));
	if (!ParentMaterial)
	{
		assert(0&&"잘못된 부모 머테리얼");
	}

	if (AssetData.contains("OverrideParams"))
	{
		auto OverrideParamData = AssetData["OverrideParams"];
		for (const auto& Data : OverrideParamData)
		{
			std::string Type = Data["Type"];
			if ("float" == Type)
			{
				FMaterialParameterDesc<float> FloatParam;
				FloatParam.Name  = Data["Name"];
				FloatParam.Value = Data["Value"];
				OverrideParams.FloatParams.emplace_back(FloatParam);
			}
			else if ("int" == Type)
			{
				FMaterialParameterDesc<int> IntParam;
				IntParam.Name  = Data["Name"];
				IntParam.Value = Data["Value"];
				OverrideParams.IntParams.emplace_back(IntParam);
			}
			else
			{
				// 04.22 float 와 int 만 파라미터로 되도록
				assert(0);
			}
		}
	}
}

void UMaterialInstance::Binding()
{
	UMaterialInterface::Binding();

	// 머테리얼 인스턴스로만 SceneProxy가 이루어져 있을 수 있으므로 바인딩을 진행 (Pipeline State가 중복적으로 값을 설정하지는 않음)
	if (ParentMaterial)
	{
		ParentMaterial->Binding();
	}
}

void UMaterialInstance::SetScalarParam(const std::string& Name, float NewValue)
{
	// 부모에 유효한 파라미터인지 확인
	auto ParentTargetParamIter = std::ranges::find_if(ParentMaterial->DefaultParams.FloatParams,
													[Name](const FMaterialParameterDesc<float>& A)
													{
														return A.Name == Name;
													});
	if (ParentTargetParamIter == ParentMaterial->DefaultParams.FloatParams.end())
	{
		MY_LOG("Warning", EDebugLogLevel::DLL_Warning, "SetScalarParam no valid ParamName");
		return;
	}

	// 현재 머테리얼 인스턴스에서 파라미터로 되어있는지
	auto OverrideParamIter = std::ranges::find_if(OverrideParams.FloatParams,
												[Name](const FMaterialParameterDesc<float>& A)
												{
													return A.Name == Name;
												});
	if (OverrideParamIter != OverrideParams.FloatParams.end())
	{
		OverrideParamIter->Value = NewValue;
	}
	else
	{
		OverrideParams.FloatParams.emplace_back(Name, 4, 0, NewValue);
	}
}

void UMaterialInstance::SetTextureParam(UINT TextureSlot, std::shared_ptr<UTexture> NewTexture)
{
	if (OverrideTextures.size() == 0)
	{
		OverrideTextures.resize(ParentMaterial->TextureParams.size());
	}
	OverrideTextures[TextureSlot] = NewTexture;
}

void UMaterialInstance::BindingMaterialInstanceUserParam() const
{
	// Constant Buffer
	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	if (ParentMaterial->ParamConstantBuffer)
	{
		HR(GDirectXDevice->GetDeviceContext()->Map(ParentMaterial->ParamConstantBuffer.Get(), 0, D3D11_MAP:: D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));

		for (const auto& FloatParam : ParentMaterial->DefaultParams.FloatParams)
		{
			auto OverrideParam = std::ranges::find_if(OverrideParams.FloatParams,
													[&](const FMaterialParameterDesc<float>& A)
													{
														return A.Name == FloatParam.Name;
													});
			// Override 되었음
			if (OverrideParam != OverrideParams.FloatParams.end())
			{
				memcpy(static_cast<char*>(cbMapSub.pData) + FloatParam.Offset, (&OverrideParam->Value), FloatParam.Size);
			}
			// Override 안됨
			else
			{
				memcpy(static_cast<char*>(cbMapSub.pData) + FloatParam.Offset, (&FloatParam.Value), FloatParam.Size);
			}
		}
		for (const auto& IntParam : ParentMaterial->DefaultParams.IntParams)
		{
			auto OverrideParam = std::ranges::find_if(OverrideParams.IntParams,
													[&](const FMaterialParameterDesc<int>& A)
													{
														return A.Name == IntParam.Name;
													});
			// Override 되었음
			if (OverrideParam != OverrideParams.IntParams.end())
			{
				memcpy(static_cast<char*>(cbMapSub.pData) + IntParam.Offset, (&OverrideParam->Value), IntParam.Size);
			}
			// Override 안됨
			else
			{
				memcpy(static_cast<char*>(cbMapSub.pData) + IntParam.Offset, (&IntParam.Value), IntParam.Size);
			}
		}

		GDirectXDevice->GetDeviceContext()->Unmap(ParentMaterial->ParamConstantBuffer.Get(), 0);
	}

	// 텍스쳐
	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();
	for (int i = 0; i < ParentMaterial->TextureParams.size(); ++i)
	{
		if (OverrideTextures.size() > i && OverrideTextures[i])
		{
			DeviceContext->PSSetShaderResources(static_cast<UINT>(ParentMaterial->Textures.size() + i), 1, OverrideTextures[i]->GetSRV().GetAddressOf());
		}
		else
		{
			DeviceContext->PSSetShaderResources(static_cast<UINT>(ParentMaterial->Textures.size() + i), 1, ParentMaterial->TextureParams[i]->GetSRV().GetAddressOf());
		}
	}
}

FComputeShader::FComputeShader(const std::string& FilePath, const std::string& FuncName, UINT ThreadPerGroupX, UINT ThreadPerGroupY, UINT ThreadPerGroupZ)
	: ThreadPerGroupX(ThreadPerGroupX), ThreadPerGroupY(ThreadPerGroupY), ThreadPerGroupZ(ThreadPerGroupZ)
{
	CreateComputeShader(FilePath, FuncName);
}

void FComputeShader::Execute_Enqueue()
{
	auto Lambda = [this](std::shared_ptr<FScene>& SceneData)
	{
		if (!Binding())
		{
			return;
		}

		CalculateGroupCount();

		//GDirectXDevice->SetComputeShader(this);
		GDirectXDevice->GetDeviceContext()->CSSetShader(ComputeShader.Get(), nullptr, 0);

		GDirectXDevice->GetDeviceContext()->Dispatch(GroupX, GroupY, GroupZ);

		ClearBinding();
	};
	ENQUEUE_RENDER_COMMAND(Lambda);
}

void FComputeShader::Execute_Immediately()
{
	if (!Binding())
	{
		return;
	}

	// Dispath시 GroupXYZ의 크기를 계산
	CalculateGroupCount();

	GDirectXDevice->GetDeviceContext()->CSSetShader(ComputeShader.Get(), nullptr, 0);

	GDirectXDevice->GetDeviceContext()->Dispatch(GroupX, GroupY, GroupZ);

	ClearBinding();
}

void FComputeShader::CreateComputeShader(const std::string& FilePath, const std::string& FuncName)
{
	std::string Path           = GEngine->GetDirectoryPath() + FilePath;
	auto        ShaderFilePath = std::wstring{Path.begin(), Path.end()};

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> ErrorBlob;
	if (FAILED(D3DCompileFromFile(ShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE , FuncName.c_str(), "cs_5_0", Flag, 0 , CSBlob.GetAddressOf(), ErrorBlob.GetAddressOf())))
	{
		if (3 == GetLastError())
		{
			MessageBox(nullptr, L"파일이 존재하지 않습니다", L"쉐이더 컴파일 실패", MB_OK);
		}
		else
		{
			MessageBoxA(nullptr, static_cast<char*>(ErrorBlob->GetBufferPointer()), "쉐이더 컴파일 실패", MB_OK);
		}

		return;
	}

	// 컴파일한 코드로 쉐이더 객체 생성하기
	GDirectXDevice->GetDevice()->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), nullptr, ComputeShader.GetAddressOf());
}

FSetColorCS::FSetColorCS()
	: FComputeShader("/Shader/ComputeShader/SetColor.fx", "CS_SetColor", 32, 32, 1), Color(0.0f, 1.0f, 1.0f, 1.0f)
{
	// Constant Buffer 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags         = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags    = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth         = 32;
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffer.GetAddressOf()));
}

bool FSetColorCS::Binding()
{
	if (nullptr == TargetTexture || nullptr == TargetTexture->GetUAV())
	{
		return false;
	}

	// 색칠할 텍스쳐럴 U0 레지스터에 Binding
	UINT i = -1;
	GDirectXDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, TargetTexture->GetUAV().GetAddressOf(), &i);

	MapAndBindConstantBuffer();

	return true;
}

void FSetColorCS::CalculateGroupCount()
{
	UINT Width  = TargetTexture->GetWidth();
	UINT Height = TargetTexture->GetHeight();

	GroupX = Width / ThreadPerGroupX;
	GroupY = Height / ThreadPerGroupY;
	GroupZ = 1;

	if (Width % ThreadPerGroupX)
		GroupX += 1;
	if (Height % ThreadPerGroupY)
		GroupY += 1;
}

void FSetColorCS::ClearBinding()
{
	UINT                       i       = -1;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	GDirectXDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &nullUAV, &i);
	TargetTexture = nullptr;
}

void FSetColorCS::MapAndBindConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	HR(GDirectXDevice->GetDeviceContext()->Map(ConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub ));

	int Width  = TargetTexture->GetWidth();
	int Height = TargetTexture->GetHeight();

	memcpy(cbMapSub.pData, &Color, 16);
	memcpy(static_cast<char*>(cbMapSub.pData) + 16, &Width, 4);
	memcpy(static_cast<char*>(cbMapSub.pData) + 20, &Height, 4);

	GDirectXDevice->GetDeviceContext()->Unmap(ConstantBuffer.Get(), 0);

	GDirectXDevice->GetDeviceContext()->CSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_ComputeShader), 1, ConstantBuffer.GetAddressOf());
}

FTickParticleCS::FTickParticleCS()
	: FComputeShader("/Shader/TickParticleCS.fx", "CS_TickParticle", 256, 1, 1)
{
}

bool FTickParticleCS::Binding()
{
	if (nullptr == ParticleBuffer || nullptr == SpawnBuffer || nullptr == ModuleBuffer)
	{
		return false;
	}

	ParticleBuffer->Binding_CS_UAV(0);
	SpawnBuffer->Binding_CS_UAV(1);
	// 렌더링 시 사용되는 t 레지스터지만,
	// CS에서 같은 번호를 사용해 파티클 관련 레지스터 번호임을 명시
	ModuleBuffer->Binding_CS_SRV(ParticleDataRegister);

	return true;
}

void FTickParticleCS::CalculateGroupCount()
{
	GroupX = ParticleBuffer->GetElementCount() / ThreadPerGroupX;
	if (ParticleBuffer->GetElementCount() % ThreadPerGroupX)
		++GroupX;

	GroupY = 1;
	GroupZ = 1;
}

void FTickParticleCS::ClearBinding()
{
	ParticleBuffer->Clear_CS_UAV(0);
	ParticleBuffer = nullptr;

	SpawnBuffer->Clear_CS_UAV(1);
	SpawnBuffer = nullptr;

	ModuleBuffer->Clear_CS_SRV(ParticleDataRegister);
	ModuleBuffer = nullptr;
}

void FTickParticleCS::MapAndBindConstantBuffer()
{
	// 원래는 파티클 최대 개수를 상수버퍼를 통해 건네줘야하지만,
	// 모든 파티클이 최대 MaxParticleCount (500) 개의 파티클을 지정했으며,
	// CS hlsl 코드 내에도 #define 으로 같은 값을 지정하므로,
	// 상수버퍼로 건네주지 않음
}

FStructuredBuffer::FStructuredBuffer()
	: Desc{}, ElementSize(0), ElementCount(0)
{
}

FStructuredBuffer::~FStructuredBuffer()
{
}

void FGeometryShader::CompileGeometryShader(const std::string& FilePath, const std::string& FuncName)
{
	std::string  TempDirectoryPath = GEngine->GetDirectoryPath();
	auto         TempShaderPath    = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	std::wstring ShaderFilePath    = TempShaderPath + std::wstring{FilePath.begin(), FilePath.end()};

	HR(CompileShaderFromFile(ShaderFilePath.c_str(), FuncName.c_str(), "gs_4_0", GSBlob.GetAddressOf()));

	HR(GDirectXDevice->GetDevice()->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, GeometryShader.GetAddressOf()));
}

int FStructuredBuffer::Create(UINT _ElementSize, UINT _ElementCount, SB_TYPE _Type, bool _SysMemMove, void* _SysMem)
{
	MainBuffer = nullptr;
	SRV        = nullptr;
	UAV        = nullptr;

	ElementSize  = _ElementSize;
	ElementCount = _ElementCount;
	Type         = _Type;
	// Desc 작성
	Desc.ByteWidth = ElementSize * ElementCount;

	if (SB_TYPE::SRV_ONLY == _Type)
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	else
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	Desc.MiscFlags      = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.Usage          = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;

	// 구조화버퍼 생성시 추가 설정
	Desc.StructureByteStride = ElementSize;

	if (nullptr == _SysMem)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateBuffer(&Desc, nullptr, MainBuffer.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	else
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem                = _SysMem;

		if (FAILED(GDirectXDevice->GetDevice()->CreateBuffer(&Desc, &tSub, MainBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
	}

	// 구조화 버퍼가 SystemMemory 랑 통신이 가능해야 한다.
	if (_SysMemMove)
	{
		D3D11_BUFFER_DESC BufferDesc = Desc;

		// Write Bufer
		BufferDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
		BufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(GDirectXDevice->GetDevice()->CreateBuffer(&BufferDesc, nullptr, WriteBuffer.GetAddressOf())))
		{
			return E_FAIL;
		}

		// ReadBuffer
		BufferDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
		BufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (FAILED(GDirectXDevice->GetDevice()->CreateBuffer(&BufferDesc, nullptr, ReadBuffer.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	// ShaderResourceView 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
	tSRVDesc.ViewDimension                   = D3D_SRV_DIMENSION_BUFFER;
	tSRVDesc.BufferEx.NumElements            = _ElementCount;

	if (FAILED(GDirectXDevice->GetDevice()->CreateShaderResourceView(MainBuffer.Get(), &tSRVDesc, SRV.GetAddressOf())))
	{
		assert(nullptr);
		return E_FAIL;
	}

	if (SB_TYPE::SRV_UAV == Type)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
		tUAVDesc.ViewDimension                    = D3D11_UAV_DIMENSION_BUFFER;
		tUAVDesc.Buffer.NumElements               = _ElementCount;

		if (FAILED(GDirectXDevice->GetDevice()->CreateUnorderedAccessView(MainBuffer.Get(), &tUAVDesc, UAV.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
	}

	return S_OK;
}

void FStructuredBuffer::SetData(void* _SysMem, UINT _ElementCount)
{
	assert(_ElementCount <= ElementCount);

	if (0 == _ElementCount)
	{
		_ElementCount = ElementCount;
	}

	// SysMem -> WirteBuffer
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	GDirectXDevice->GetDeviceContext()->Map(WriteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _SysMem, ElementSize * _ElementCount);
	GDirectXDevice->GetDeviceContext()->Unmap(WriteBuffer.Get(), 0);

	// WriteBuffer -> MainBuffer
	GDirectXDevice->GetDeviceContext()->CopyResource(MainBuffer.Get(), WriteBuffer.Get());
}

void FStructuredBuffer::GetData(void* _SysMem, UINT _ElementCount)
{
	//assert(_ElementCount <= ElementCount);

	//if (_ElementCount == 0)
	//{
	//	_ElementCount = ElementCount;
	//}

	//// MainBuffer -> ReadBuffer
	//CONTEXT->CopyResource(ReadBuffer.Get(), MainBuffer.Get());

	//// ReadBuffer -> SysMem
	//D3D11_MAPPED_SUBRESOURCE tSub = {};
	//CONTEXT->Map(ReadBuffer.Get(), 0, D3D11_MAP_READ, 0, &tSub);
	//memcpy(_SysMem, tSub.pData, ElementSize * _ElementCount);
	//CONTEXT->Unmap(ReadBuffer.Get(), 0);
}

void FStructuredBuffer::Binding(UINT _TexRegisterNum)
{
	GDirectXDevice->GetDeviceContext()->VSSetShaderResources(_TexRegisterNum, 1, SRV.GetAddressOf());
	GDirectXDevice->GetDeviceContext()->HSSetShaderResources(_TexRegisterNum, 1, SRV.GetAddressOf());
	GDirectXDevice->GetDeviceContext()->DSSetShaderResources(_TexRegisterNum, 1, SRV.GetAddressOf());
	GDirectXDevice->GetDeviceContext()->GSSetShaderResources(_TexRegisterNum, 1, SRV.GetAddressOf());
	GDirectXDevice->GetDeviceContext()->PSSetShaderResources(_TexRegisterNum, 1, SRV.GetAddressOf());
}

void FStructuredBuffer::Clear(UINT _TexRegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	GDirectXDevice->GetDeviceContext()->VSSetShaderResources(_TexRegisterNum, 1, &pSRV);
	GDirectXDevice->GetDeviceContext()->HSSetShaderResources(_TexRegisterNum, 1, &pSRV);
	GDirectXDevice->GetDeviceContext()->DSSetShaderResources(_TexRegisterNum, 1, &pSRV);
	GDirectXDevice->GetDeviceContext()->GSSetShaderResources(_TexRegisterNum, 1, &pSRV);
	GDirectXDevice->GetDeviceContext()->PSSetShaderResources(_TexRegisterNum, 1, &pSRV);
}

void FStructuredBuffer::Binding_CS_UAV(UINT RegisterNum)
{
	assert(UAV.Get() != nullptr && "UAV is nullptr");
	UINT i = -1;
	GDirectXDevice->GetDeviceContext()->CSSetUnorderedAccessViews(RegisterNum, 1, UAV.GetAddressOf(), &i);
}

void FStructuredBuffer::Clear_CS_UAV(UINT RegisterNum)
{
	UINT                       i       = -1;
	ID3D11UnorderedAccessView* NullUAV = nullptr;
	GDirectXDevice->GetDeviceContext()->CSSetUnorderedAccessViews(RegisterNum, 1, &NullUAV, &i);
}

void FStructuredBuffer::Binding_CS_SRV(UINT RegisterNum)
{
	assert(SRV.Get() != nullptr && "SRV is nullptr");
	GDirectXDevice->GetDeviceContext()->CSSetShaderResources(RegisterNum, 1, SRV.GetAddressOf());
}

void FStructuredBuffer::Clear_CS_SRV(UINT RegisterNum)
{
	ID3D11ShaderResourceView* NullSRV = nullptr;
	GDirectXDevice->GetDeviceContext()->CSSetShaderResources(RegisterNum, 1, &NullSRV);
}
