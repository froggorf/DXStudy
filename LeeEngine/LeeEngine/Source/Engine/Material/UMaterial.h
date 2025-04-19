// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Class/UTexture.h"
#include "Engine/UObject/UObject.h"




class FShader
{
public:
	FShader() {}
	virtual ~FShader() {}
protected:

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

public:
	void CompileVertexShader(const std::string& FilePath, const std::string& FuncName);
};

class FPixelShader : public FGraphicsShader
{
public:
	FPixelShader() {};
	~FPixelShader() override {}
	void CompileVertexShader(const std::string& FilePath, const std::string& FuncName);;

	Microsoft::WRL::ComPtr<ID3DBlob>			PSBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	PixelShader;
};


class UMaterialInterface : public UObject
{
	MY_GENERATED_BODY(UMaterialInterface)


public:
	static std::shared_ptr<UMaterialInterface> GetMaterialCache(const std::string& MaterialName)
	{
		auto Target = MaterialCache.find(MaterialName);
		if(Target != MaterialCache.end())
		{
			return Target->second;
		}
		return nullptr;
	}

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	virtual void Binding() {};
protected:
	static std::unordered_map<std::string, std::shared_ptr<UMaterialInterface>> MaterialCache;
};

class UMaterial : public UMaterialInterface, public std::enable_shared_from_this<UMaterial>
{
	MY_GENERATED_BODY(UMaterial)

public:
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	void Binding() override;
protected:
	std::shared_ptr<FVertexShader> VertexShader;
	std::shared_ptr<FPixelShader> PixelShader;

	std::vector<std::shared_ptr<UTexture>> Textures;
	
};