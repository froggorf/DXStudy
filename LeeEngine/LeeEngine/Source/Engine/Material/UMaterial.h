// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Class/UTexture.h"
#include "Engine/UObject/UObject.h"

template <typename Type>
struct FMaterialParameterDesc
{
	std::string Name;
	uint32_t    Size;
	uint32_t    Offset; // cbuffer 내 오프셋
	Type        Value;
};

struct FMaterialParameterLayout
{
	std::vector<FMaterialParameterDesc<int>>   IntParams;
	std::vector<FMaterialParameterDesc<float>> FloatParams;
	/*std::vector<FMaterialParameterDesc<XMFLOAT2>> Float2Params;
	std::vector<FMaterialParameterDesc<XMFLOAT3>> Float3Params;
	std::vector<FMaterialParameterDesc<XMFLOAT4>> Float4Params;*/

	// 정렬을 위해 무조건 16의 배수로 설정되어야함 
	uint32_t TotalSize = 0;
};

class FShader
{
public:
	FShader()
	{
	}

	virtual ~FShader()
	{
	}

	void SetShaderID(UINT NewID);

	UINT GetShaderID() const
	{
		return ShaderID;
	}

	static std::shared_ptr<FShader> GetShader(const std::string& Name)
	{
		auto Shader = ShaderCache.find(Name);
		if (Shader != ShaderCache.end())
		{
			return Shader->second;
		}
		return nullptr;
	}

	static void AddShaderCache(const std::string& Name, const std::shared_ptr<FShader>& NewShader)
	{
		ShaderCache[Name] = NewShader;
	}

protected:
	UINT                                                             ShaderID = -1;
	static std::unordered_map<std::string, std::shared_ptr<FShader>> ShaderCache;

	friend class UMaterial;
};

class FGraphicsShader : public FShader
{
public:
	FGraphicsShader()           = default;
	~FGraphicsShader() override = default;
};

class FVertexShader : public FGraphicsShader
{
public:
	FVertexShader()
	{
	};

	~FVertexShader() override
	{
	};

	Microsoft::WRL::ComPtr<ID3DBlob>           VSBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  InputLayout;

	void CompileVertexShader(const std::string& FilePath, const std::string& FuncName);
};

class FPixelShader : public FGraphicsShader
{
public:
	FPixelShader()
	{
	};

	~FPixelShader() override
	{
	}

	void CompilePixelShader(const std::string& FilePath, const std::string& FuncName);;

	Microsoft::WRL::ComPtr<ID3DBlob>          PSBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};

enum class EBlendMode
{
	BM_Deferred,
	BM_Opaque,
	BM_Masked,
	BM_Translucent
};

class UMaterialInterface : public UObject
{
	MY_GENERATE_BODY(UMaterialInterface)

	UMaterialInterface()
	{
	};
	~UMaterialInterface() override = default;

	virtual UINT GetMaterialID() const
	{
		return -1;
	}

	virtual ERasterizerType GetRSType() const
	{
		return RasterizerType;
	};

	virtual EDepthStencilStateType GetDepthStencilState() const
	{
		return DepthStencilState;
	}

	virtual EBlendStateType GetBlendStateType() const {return BlendStateType;}

	virtual EBlendMode GetBlendModeType() const
	{
		return BlendMode;
	}

	static std::shared_ptr<UMaterialInterface> GetMaterialCache(const std::string& MaterialName);

	virtual bool IsMaterialInstance() const
	{
		return false;
	}

	virtual Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const
	{
		return nullptr;
	}

	virtual Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const
	{
		return nullptr;
	}

	virtual void BindingMaterialInstanceUserParam() const
	{
	}

	// 파라미터 변경 함수
	virtual void SetScalarParam(const std::string& Name, float NewValue)
	{
	}

	virtual void SetTextureParam(UINT TextureSlot, std::shared_ptr<UTexture> NewTexture)
	{
	}

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	virtual void Binding()
	{
	};

	void SetBlendMode(EBlendMode InBlendMode) {BlendMode = InBlendMode;}
	void SetRasterizerType(ERasterizerType InRasterizerType) {RasterizerType = InRasterizerType;}
	void SetBlendStateType(EBlendStateType InBlendStateType) {BlendStateType = InBlendStateType;}
	void SetDepthStencilState(EDepthStencilStateType InDepthStencilState) {DepthStencilState = InDepthStencilState;}

	EBlendMode      BlendMode = EBlendMode::BM_Opaque;
	ERasterizerType RasterizerType = ERasterizerType::RT_CullBack;
	EBlendStateType BlendStateType = EBlendStateType::BST_Default;
	EDepthStencilStateType DepthStencilState = EDepthStencilStateType::DST_LESS;
};

class UMaterial : public UMaterialInterface
{
	MY_GENERATE_BODY(UMaterial)
	friend class UMaterialInstance;
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	UINT GetMaterialID() const override
	{
		return MaterialID;
	}

	void Binding() override;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const override
	{
		return VertexShader->VertexShader;
	}

	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const override
	{
		return PixelShader->PixelShader;
	}

	void MapAndBindParameterConstantBuffer() const;

	void SetTexture(UINT SlotIndex, const std::shared_ptr<UTexture>& NewTexture);

protected:
	std::shared_ptr<FVertexShader>   VertexShader;
	std::shared_ptr<FPixelShader>    PixelShader;
	std::shared_ptr<FGeometryShader> GeometryShader;

	// 디폴트 텍스쳐 데이터
	std::vector<std::shared_ptr<UTexture>> Textures;

	// 텍스쳐 파라미터
	std::vector<std::shared_ptr<UTexture>> TextureParams;

	FMaterialParameterLayout             DefaultParams;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ParamConstantBuffer;

	UINT MaterialID = -1;

	static UINT MaterialIDCount;
};

class UMaterialInstance : public UMaterialInterface
{
	MY_GENERATE_BODY(UMaterialInstance)
	UMaterialInstance()           = default;
	~UMaterialInstance() override = default;
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	UINT GetMaterialID() const override
	{
		return ParentMaterial->GetMaterialID();
	};
	void Binding() override;

	ERasterizerType GetRSType() const override
	{
		return ParentMaterial->RasterizerType;
	};

	EBlendMode GetBlendModeType() const override
	{
		return ParentMaterial->BlendMode;
	}

	bool IsMaterialInstance() const override
	{
		return true;
	}

	// 파라미터 설정 함수
	void SetScalarParam(const std::string& Name, float NewValue) override;
	void SetTextureParam(UINT TextureSlot, std::shared_ptr<UTexture> NewTexture) override;

	// 유저 파라미터 바인딩 함수
	void BindingMaterialInstanceUserParam() const override;

	std::shared_ptr<UMaterialInstance> GetInstance() const
	{
		auto NewInstance            = std::make_shared<UMaterialInstance>();
		NewInstance->OverrideParams = OverrideParams;
		NewInstance->OverrideTextures = OverrideTextures;
		NewInstance->ParentMaterial = ParentMaterial;
		return NewInstance;
	}

	FMaterialParameterLayout OverrideParams;
	// 텍스쳐 슬롯, 오버라이드 텍스쳐
	std::vector<std::shared_ptr<UTexture>> OverrideTextures;

private:
	std::shared_ptr<UMaterial> ParentMaterial;
};

// ==============================================
// ================= StructuredBuffer ====================

enum class SB_TYPE
{
	SRV_ONLY, // t
	SRV_UAV,  // t u
};

class FStructuredBuffer
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> MainBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> WriteBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBuffer;

	D3D11_BUFFER_DESC Desc;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  SRV; // t 레지스터 바인딩
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAV; // u 레지스터 바인딩

	UINT ElementSize;  // 크기
	UINT ElementCount; // 개수

	SB_TYPE Type;     // t u 레지스터 바인딩 설정
	bool    bSysMove; // SystemMemory 와 데이터 전송 가능

public:
	int Create(UINT _ElementSize, UINT _ElementCount, SB_TYPE _Type, bool _SysMemMove, void* _SysMem = nullptr);

	UINT GetElementCount() const
	{
		return ElementCount;
	}

	UINT GetElementSize() const
	{
		return ElementSize;
	}

	UINT GetBufferSize() const
	{
		return ElementSize * ElementCount;
	}

	void SetData(void* _SysMem, UINT _ElementCount = 0);
	void GetData(void* _SysMem, UINT _ElementCount = 0);
	void Binding(UINT _TexRegisterNum);
	void Clear(UINT _TexRegisterNum);

	void Binding_CS_UAV(UINT RegisterNum);
	void Clear_CS_UAV(UINT RegisterNum);
	void Binding_CS_SRV(UINT RegisterNum);
	void Clear_CS_SRV(UINT RegisterNum);

	FStructuredBuffer();
	virtual ~FStructuredBuffer();
};

// ==============================================
// ================ 컴퓨트 셰이더 ================
class FComputeShader : public FShader, public std::enable_shared_from_this<FComputeShader>
{
public:
	FComputeShader(const std::string& FilePath, const std::string& FuncName, UINT ThreadPerGroupX, UINT ThreadPerGroupY, UINT ThreadPerGroupZ);
	~FComputeShader() override = default;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> GetComputeShader() const
	{
		return ComputeShader;
	}

	// 상속받은 컴퓨트 셰이더에서 UAV등을 바인딩 하는 함수
	virtual bool Binding() = 0;
	// 상속받은 컴퓨트 셰이더에서 Dispath 그룹개수를 계산하는 함수
	virtual void CalculateGroupCount() = 0;
	// 상속받은 컴퓨트 셰이더에서 상수버퍼를 바인딩 하는 함수
	virtual void MapAndBindConstantBuffer() = 0;
	// 상속받은 컴퓨트 셰이더에서 바인딩한 UAV를 초기화하는 함수
	virtual void ClearBinding() = 0;

	void Execute_Enqueue();
	void Execute_Immediately();

private:
	void CreateComputeShader(const std::string& FilePath, const std::string& FuncName);

protected:
	UINT GroupX;
	UINT GroupY;
	UINT GroupZ;

	const UINT ThreadPerGroupX;
	const UINT ThreadPerGroupY;
	const UINT ThreadPerGroupZ;

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;

private:
	Microsoft::WRL::ComPtr<ID3DBlob>            CSBlob;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> ComputeShader;
};

class FSetColorCS : public FComputeShader
{
public:
	FSetColorCS();

private:
	std::shared_ptr<UTexture> TargetTexture;
	XMFLOAT4                  Color;

public:
	void SetTargetTexture(const std::shared_ptr<UTexture>& Target)
	{
		TargetTexture = Target;
	}

	void SetClearColor(XMFLOAT4 NewColor)
	{
		Color = NewColor;
	}

	bool Binding() override;
	void CalculateGroupCount() override;
	void ClearBinding() override;
	void MapAndBindConstantBuffer() override;
};

class FTickParticleCS : public FComputeShader
{
public:
	FTickParticleCS();

private:
	std::shared_ptr<FStructuredBuffer> ParticleBuffer;
	std::shared_ptr<FStructuredBuffer> SpawnBuffer;
	std::shared_ptr<FStructuredBuffer> ModuleBuffer;

public:
	void SetParticleBuffer(const std::shared_ptr<FStructuredBuffer>& InBuffer)
	{
		ParticleBuffer = InBuffer;
	}

	void SetSpawnBuffer(const std::shared_ptr<FStructuredBuffer>& InBuffer)
	{
		SpawnBuffer = InBuffer;
	}

	void SetModuleBuffer(const std::shared_ptr<FStructuredBuffer>& InBuffer)
	{
		ModuleBuffer = InBuffer;
	}

	bool Binding() override;
	void CalculateGroupCount() override;
	void ClearBinding() override;
	void MapAndBindConstantBuffer() override;
};

// ==============================================

// ===========GeometryShader===================
class FGeometryShader : public FShader
{
public:
	FGeometryShader()
	{
	};

	~FGeometryShader() override
	{
	};
	Microsoft::WRL::ComPtr<ID3DBlob>             GSBlob;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> GeometryShader;
	void                                         CompileGeometryShader(const std::string& FilePath, const std::string& FuncName);
};
