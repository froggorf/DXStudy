// assimp 라이브러리와 텍스쳐 로드 라이브러리를 활용하여
// 모델, 텍스쳐 등의 오브젝트 (에셋)을 관리하는 매니저 클래스
#pragma once
#include <concurrent_unordered_map.h>
#include "DirectX/d3dUtil.h"
#include "Engine/RenderCore/EditorScene.h"
class UObject;
struct aiScene;
struct aiMesh;

enum class EAssetDataType
{
	EADT_StaticMesh = 0,
	EADT_Level,
	EADT_SkeletalMesh,
};

template <typename T>concept DerivedFromUObject = std::is_base_of_v<UObject, T>;

class AssetManager
{
public:
	// === 파일 데이터를 읽어 모델 정보를 로드하는 함수 ===
	// 한 메쉬가 여러 오브젝트로 존재시 vector 데이터
	static void LoadModelData(const std::string& path, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pIndexBuffer);
	// 한 메쉬가 하나의 오브젝트로 존재시
	static void LoadModelData(const std::string& path, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer>& pVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pIndexBuffer);
	// SkeletalMesh 모델 데이터 로드
	static void LoadSkeletalModelData(const std::string& path, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pIndexBuffer, std::map<std::string, BoneInfo>& modelBoneInfoMap);

	//static void LoadTextureFromTGA(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	//std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView);
	static void LoadTextureFromFile(const std::wstring& szFile, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView);
	static void LoadTextureFromFile(const std::wstring& szFile, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& vTextureShaderResourceView);
	static void LoadTexture(class UTexture* Texture, const nlohmann::json& AssetData);

	// UINT BindFlag -> D3D11_BIND_FLAG
	static std::shared_ptr<UTexture> CreateTexture(const std::string& Name, UINT Width, UINT Height, DXGI_FORMAT PixelFormat, UINT BindFlag, D3D11_USAGE Usage = D3D11_USAGE_DEFAULT);

	// myasset 파일 읽기
	// myasset을 통해 읽은 UObject 데이터를 unique_ptr를 통해 관리하고 반환
	// UObject에 계승된 클래스만 템플릿의 타입으로 들어갈 수 있도록 컨셉 사용
	//template <DerivedFromUObject T>
	static UObject* ReadMyAsset(const std::string& FilePath);

private:
	// 파일 데이터로 얻은 aiScene 클래스로부터 메쉬 정보 얻는 함수
	static void ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices, std::vector<std::vector<UINT>>& allIndices);

	// 메쉬 정보를 분석하여 VertexType에 맞게 정보 수집
	static void ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices);

	// SkeletalMesh 모델의 버텍스 데이터 내 본 데이터를 설정하는 함수
	static void SetVertexBoneData(MyVertexData& vertexData, int boneID, float weight);
	// 버텍스의 BoneWeight 정보를 추출하는 함수
	static void ExtractBoneWeightForVertices(std::vector<MyVertexData>& vVertexData, aiMesh* mesh, std::map<std::string, BoneInfo>& modelBoneInfoMap);

public:
	static std::unordered_map<std::string, std::string>& GetAssetNameAndAssetPathCacheMap()
	{
		return AssetNameAndAssetPathCacheMap;
	}

	static std::shared_ptr<UObject> GetAssetCacheByName(const std::string& AssetName)
	{
		const std::unordered_map<std::string, std::shared_ptr<UObject>>& Map    = GetAssetCacheMap();
		auto                                                             Target = Map.find(AssetName);
		if (Target != Map.end())
		{
			return Target->second;
		}
		return nullptr;
	}

	static std::shared_ptr<UObject> GetAsyncAssetCache(const std::string& AssetName)
	{
		const auto Iter = AsyncAssetCache.find(AssetName);
		if(Iter != AsyncAssetCache.end())
		{
			// 에셋이 현재 소멸한 상태라면
			if(Iter->second.expired())
			{
				AsyncAssetCache.unsafe_erase(Iter); 
			}
			else
			{
				// std::weak_ptr<UObject> -> std::shared_ptr<UObject>
				std::shared_ptr<UObject> SharedObj = Iter->second.lock();
				return SharedObj;
			}
		}

		return nullptr;
	}
	static void LoadAssetAsync(const std::string& Path)
	{
		
	}

private:
	static std::unordered_map<std::string, std::shared_ptr<UObject>>& GetAssetCacheMap()
	{
		static std::unordered_map<std::string, std::shared_ptr<UObject>> AssetCache;
		return AssetCache;
	}

	static std::unordered_map<std::string, std::string> AssetNameAndAssetPathCacheMap;

	// 게임쓰레드와 잡쓰레드(비동기 에셋 로드 쓰레드)들끼리 접근할 수 있으므로
	// 마이크로 소프트의 concurrent 라이브러리를 사용하여 concurrent_unordered_map 자료구조 사용
	static concurrency::concurrent_unordered_map<std::string, std::weak_ptr<UObject>> AsyncAssetCache;

	// 에셋의 로딩이 요청될 경우 추가되며,
	// {에셋 이름} , 로딩 완료할 경우 실행될 콜백함수를 저장하는 컨테이너
	// A에셋에서 "1" 을 로드 중 B에셋에서 "1"을 동시에 로드 요청할 경우 콜백함수를 계속해서 추가할 수 있게 적용
	static concurrency::concurrent_unordered_map<std::string, std::vector<std::function<void(std::shared_ptr<UObject>)>>> LoadingCallbackMap;

};
