// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Class/UTexture.h"
#include "Engine/UObject/UObject.h"

template<typename Type>
struct FMaterialParameterDesc
{
	std::string Name;
	uint32_t Size;
	uint32_t Offset; // cbuffer 내 오프셋
	Type Value;
};

struct FMaterialParameterLayout
{
	std::vector<FMaterialParameterDesc<int>> IntParams;
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
	FShader() {}
	virtual ~FShader() {}

	void SetShaderID(UINT NewID);
	UINT GetShaderID() const {return ShaderID;}
protected:
	UINT ShaderID = -1;
	static std::unordered_map<std::string, std::shared_ptr<FShader>> ShaderCache;

	friend class UMaterial;
};

class FGraphicsShader : public FShader
{
public:
	FGraphicsShader() = default;
	~FGraphicsShader() override = default;
};

class FVertexShader : public FGraphicsShader
{
public:
	FVertexShader() {};
	~FVertexShader() override {};

	Microsoft::WRL::ComPtr<ID3DBlob>			VSBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	VertexShader;
	EInputLayoutType InputLayoutType = EInputLayoutType::ILT_Count;

public:
	void CompileVertexShader(const std::string& FilePath, const std::string& FuncName);
};

class FPixelShader : public FGraphicsShader
{
public:
	FPixelShader() {};
	~FPixelShader() override {}
	void CompilePixelShader(const std::string& FilePath, const std::string& FuncName);;

	Microsoft::WRL::ComPtr<ID3DBlob>			PSBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	PixelShader;
};

enum class EBlendMode
{
	BM_Opaque,
	BM_Masked,
	BM_Translucent
};

class UMaterialInterface : public UObject
{
	MY_GENERATED_BODY(UMaterialInterface)

public:
	UMaterialInterface() : BlendMode(EBlendMode::BM_Opaque), RasterizerType(ERasterizerType::RT_CullBack), BlendStateType(EBlendStateType::BST_Default) {};
	~UMaterialInterface() override = default;
	virtual UINT GetMaterialID() const {return -1;}
	virtual ERasterizerType GetRSType() const {return RasterizerType;};

	static std::shared_ptr<UMaterialInterface> GetMaterialCache(const std::string& MaterialName)
	{
		auto Target = MaterialCache.find(MaterialName);
		if(Target != MaterialCache.end())
		{
			return Target->second;
		}
		return nullptr;
	}

	virtual bool IsMaterialInstance() const {return false;}

	virtual Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const {return nullptr;}
	virtual Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const {return nullptr;}
	virtual EInputLayoutType GetInputLayoutType() const {return EInputLayoutType::ILT_Count;}

	virtual void BindingMaterialInstanceUserParam() const {}

	virtual void SetScalarParam(const std::string& Name, float NewValue) {}

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	virtual void Binding() {};
protected:
	static std::unordered_map<std::string, std::shared_ptr<UMaterialInterface>> MaterialCache;
public:
	EBlendMode BlendMode;
	ERasterizerType RasterizerType;
	EBlendStateType BlendStateType;
};

class UMaterial : public UMaterialInterface, public std::enable_shared_from_this<UMaterial>
{
	MY_GENERATED_BODY(UMaterial)
	friend class UMaterialInstance;
public:
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	UINT GetMaterialID() const override {return MaterialID;}
	void Binding() override;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const override{return VertexShader->VertexShader;}
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const override {return PixelShader->PixelShader;}
	EInputLayoutType GetInputLayoutType() const override {return VertexShader->InputLayoutType;}

	void MapAndBindParameterConstantBuffer() const;
protected:
	std::shared_ptr<FVertexShader> VertexShader;
	std::shared_ptr<FPixelShader> PixelShader;

	std::vector<std::shared_ptr<UTexture>> Textures;

	FMaterialParameterLayout	DefaultParams;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ParamConstantBuffer;

	UINT MaterialID = -1;
	
};

class UMaterialInstance : public UMaterialInterface, public std::enable_shared_from_this<UMaterialInstance>
{
	MY_GENERATED_BODY(UMaterialInstance)
public:
	UMaterialInstance() = default;
	~UMaterialInstance() override = default;
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	UINT GetMaterialID() const override {return ParentMaterial->GetMaterialID();};
	void Binding() override;
	ERasterizerType GetRSType() const override {return ParentMaterial->RasterizerType;};

	bool IsMaterialInstance() const override {return true;}
	void SetScalarParam(const std::string& Name, float NewValue) override;

	void BindingMaterialInstanceUserParam() const override;

	std::shared_ptr<UMaterialInstance> GetInstance() const
	{
		std::shared_ptr<UMaterialInstance> NewInstance = std::make_shared<UMaterialInstance>();
		NewInstance->OverrideParams = OverrideParams;
		NewInstance->ParentMaterial = ParentMaterial;
		return NewInstance;
	}

	FMaterialParameterLayout	OverrideParams;
private:
	std::shared_ptr<UMaterial> ParentMaterial;

};