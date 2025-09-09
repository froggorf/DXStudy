#include "CoreMinimal.h"
#include "AssetManager.h"

#include "Engine/Class/UTexture.h"
#include "Engine/Misc/QueuedThreadPool.h"

using namespace Microsoft::WRL;
using namespace concurrency;
std::unordered_map<std::string, std::string> AssetManager::AssetNameAndAssetPathMap;
concurrent_unordered_map<std::string, std::weak_ptr<UObject>> AssetManager::AsyncAssetCache;
concurrent_unordered_map<std::string, concurrent_vector<AssetLoadedCallback>> AssetManager::LoadingCallbackMap;

void AssetManager::LoadModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, std::vector<std::vector<MyVertexData>>& AllVertices,std::vector<std::vector<UINT>>& AllIndices,std::vector<ComPtr<ID3D11Buffer>>& pVertexBuffer, std::vector<ComPtr<ID3D11Buffer>>& pIndexBuffer, float& BoundSphereLength)
{
	AllVertices.clear();
	AllIndices.clear();
	std::string filePath = GEngine->GetDirectoryPath() + path;

	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(filePath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals |aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;

		return;
	}

	ProcessScene(scene, AllVertices, AllIndices, path.contains("UECOORD"));

	// BoundSphereLength 계산
	aiVector3D MinBound = aiVector3D{FLT_MAX, FLT_MAX, FLT_MAX};
	aiVector3D MaxBound = aiVector3D{-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for (const std::vector<MyVertexData>& Vertices : AllVertices)
	{
		for (const MyVertexData& Vertex : Vertices)
		{
			MinBound.x = std::min(MinBound.x, Vertex.Pos.x);
			MinBound.y = std::min(MinBound.y, Vertex.Pos.y);
			MinBound.z = std::min(MinBound.z, Vertex.Pos.z);

			MaxBound.x = max(MaxBound.x, Vertex.Pos.x);
			MaxBound.y = max(MaxBound.y, Vertex.Pos.y);
			MaxBound.z = max(MaxBound.z, Vertex.Pos.z);
		}
	}
	BoundSphereLength = (MaxBound - MinBound).Length() / 2;
	

	// 모델 로드 성공
	MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "Model - " + filePath+" Load Success");
	for (UINT i = 0; i < scene->mNumMeshes; ++i)
	{
		// 버텍스 버퍼
		ComPtr<ID3D11Buffer> vertexBuffer;
		D3D11_BUFFER_DESC    vertexBufferDesc = {};
		vertexBufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags       = 0;
		vertexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth            = sizeof(MyVertexData) * static_cast<UINT>(AllVertices[i].size());
		D3D11_SUBRESOURCE_DATA initVertexData = {};
		initVertexData.pSysMem                = AllVertices[i].data();
		HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
		pVertexBuffer.push_back(vertexBuffer);

		// 인덱스 버퍼
		ComPtr<ID3D11Buffer> indexBuffer;
		D3D11_BUFFER_DESC    indexBufferDesc = {};
		indexBufferDesc.BindFlags            = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth            = sizeof(UINT) * static_cast<UINT>(AllIndices[i].size());
		indexBufferDesc.CPUAccessFlags       = 0;
		indexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initIndexData = {};
		initIndexData.pSysMem                = AllIndices[i].data();
		HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
		pIndexBuffer.push_back(indexBuffer);
	}
}

void AssetManager::LoadModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11Buffer>& pVertexBuffer, ComPtr<ID3D11Buffer>& pIndexBuffer)
{
	std::string filePath = GEngine->GetDirectoryPath() + path;

	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	std::vector<std::vector<MyVertexData>> allVertices;
	std::vector<std::vector<UINT>>         allIndices;

	ProcessScene(scene, allVertices, allIndices,path.contains("UECOORD"));

	// 결과 출력

	// 모델 로드 성공
	MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "Model - " + filePath+" Load Success");

	// 버텍스 버퍼
	ComPtr<ID3D11Buffer> vertexBuffer;
	D3D11_BUFFER_DESC    vertexBufferDesc = {};
	vertexBufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags       = 0;
	vertexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth            = sizeof(MyVertexData) * static_cast<UINT>(allVertices[0].size());
	D3D11_SUBRESOURCE_DATA initVertexData = {};
	initVertexData.pSysMem                = allVertices[0].data();
	HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
	//   pVertexBuffer.push_back(vertexBuffer);
	pVertexBuffer = (vertexBuffer.Get());

	// 인덱스 버퍼
	ComPtr<ID3D11Buffer> indexBuffer;
	D3D11_BUFFER_DESC    indexBufferDesc = {};
	indexBufferDesc.BindFlags            = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth            = sizeof(UINT) * static_cast<UINT>(allIndices[0].size());
	indexBufferDesc.CPUAccessFlags       = 0;
	indexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA initIndexData = {};
	initIndexData.pSysMem                = allIndices[0].data();
	HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
	//pIndexBuffer.push_back(indexBuffer);
	pIndexBuffer = (indexBuffer.Get());
}

void AssetManager::LoadSkeletalModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, std::vector<ComPtr<ID3D11Buffer>>& pVertexBuffer, std::vector<ComPtr<ID3D11Buffer>>& pIndexBuffer, std::map<std::string, BoneInfo>& modelBoneInfoMap, float& BoundSphereLength)
{
	// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
	std::string filePath = GEngine->GetDirectoryPath() + path;

	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	std::vector<std::vector<MyVertexData>> allVertices;
	std::vector<std::vector<UINT>>         allIndices;

	ProcessScene(scene, allVertices, allIndices, path.contains("UECOORD"));

	// BoundSphereLength 계산
	aiVector3D MinBound = aiVector3D{FLT_MAX, FLT_MAX, FLT_MAX};
	aiVector3D MaxBound = aiVector3D{-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for (const std::vector<MyVertexData>& Vertices : allVertices)
	{
		for (const MyVertexData& Vertex : Vertices)
		{
			MinBound.x = std::min(MinBound.x, Vertex.Pos.x);
			MinBound.y = std::min(MinBound.y, Vertex.Pos.y);
			MinBound.z = std::min(MinBound.z, Vertex.Pos.z);

			MaxBound.x = max(MaxBound.x, Vertex.Pos.x);
			MaxBound.y = max(MaxBound.y, Vertex.Pos.y);
			MaxBound.z = max(MaxBound.z, Vertex.Pos.z);
		}
	}
	BoundSphereLength = (MaxBound - MinBound).Length() / 2;
	
	// VertexData를 SkeletalVertexData로 변환
	std::vector<std::vector<MyVertexData>> allSkeletalVertices(scene->mNumMeshes);
	for (int meshIndex = 0; meshIndex < allVertices.size(); ++meshIndex)
	{
		allSkeletalVertices[meshIndex].resize(allVertices[meshIndex].size());

		for (int vertexIndex = 0; vertexIndex < allVertices[meshIndex].size(); ++vertexIndex)
		{
			allSkeletalVertices[meshIndex][vertexIndex].Pos       = allVertices[meshIndex][vertexIndex].Pos;
			allSkeletalVertices[meshIndex][vertexIndex].Normal    = allVertices[meshIndex][vertexIndex].Normal;
			allSkeletalVertices[meshIndex][vertexIndex].TexCoords = allVertices[meshIndex][vertexIndex].TexCoords;
			for (int maxBoneInfluenceCount = 0; maxBoneInfluenceCount < MAX_BONE_INFLUENCE; ++maxBoneInfluenceCount)
			{
				allSkeletalVertices[meshIndex][vertexIndex].m_Weights[maxBoneInfluenceCount] = 0.0f;
				allSkeletalVertices[meshIndex][vertexIndex].m_BoneIDs[maxBoneInfluenceCount] = -1;
			}
		}
	}

	for (UINT meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		ExtractBoneWeightForVertices(allSkeletalVertices[meshIndex], scene->mMeshes[meshIndex], modelBoneInfoMap);
	}

	// 모델 로드 성공
	MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "SkeletalModel - " + filePath+" Load Success");

	for (UINT i = 0; i < scene->mNumMeshes; ++i)
	{
		// 버텍스 버퍼
		ComPtr<ID3D11Buffer> vertexBuffer;
		D3D11_BUFFER_DESC    vertexBufferDesc = {};
		vertexBufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags       = 0;
		vertexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth            = sizeof(MyVertexData) * static_cast<UINT>(allSkeletalVertices[i].size());
		D3D11_SUBRESOURCE_DATA initVertexData = {};
		initVertexData.pSysMem                = allSkeletalVertices[i].data();
		HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
		pVertexBuffer.push_back(vertexBuffer);

		// 인덱스 버퍼
		ComPtr<ID3D11Buffer> indexBuffer;
		D3D11_BUFFER_DESC    indexBufferDesc = {};
		indexBufferDesc.BindFlags            = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth            = sizeof(UINT) * static_cast<UINT>(allIndices[i].size());
		indexBufferDesc.CPUAccessFlags       = 0;
		indexBufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initIndexData = {};
		initIndexData.pSysMem                = allIndices[i].data();
		HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
		pIndexBuffer.push_back(indexBuffer);
	}
}

void AssetManager::LoadTextureFromFile(const std::wstring& szFile, const ComPtr<ID3D11Device> pDevice, std::vector<ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView)
{
	ScratchImage image;

	std::filesystem::path p(szFile.c_str());
	std::wstring          ext = p.extension();

	if (ext == L".dds" || ext == L".DDS")
		LoadFromDDSFile(szFile.c_str(), DDS_FLAGS_NONE, nullptr, image);
	else if (ext == L".tga" || ext == L".TGA")
		LoadFromTGAFile(szFile.c_str(), nullptr, image);
	else // png, jpg, jpeg, bmp
		LoadFromWICFile(szFile.c_str(), WIC_FLAGS_NONE, nullptr, image);

	ID3D11Texture2D* pTexture = nullptr;
	HR(DirectX::CreateTexture(pDevice.Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), ( ID3D11Resource**)&pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip       = 0;
	srvDesc.Texture2D.MipLevels             = 1;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	pTexture->GetDesc(&textureDesc);
	srvDesc.Format = textureDesc.Format;

	ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	HR(pDevice->CreateShaderResourceView(pTexture, &srvDesc, srv.ReleaseAndGetAddressOf()));
	vTextureShaderResourceView.push_back(srv);

	MY_LOG("TextureLoad", EDebugLogLevel::DLL_Display, "Texture Load Success");
}

void AssetManager::LoadTextureFromFile(const std::wstring& szFile, const ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11ShaderResourceView>& pTextureShaderResourceView)
{
	ScratchImage image;

	std::filesystem::path p(szFile.c_str());
	std::wstring          ext = p.extension();

	if (ext == L".dds" || ext == L".DDS")
		LoadFromDDSFile(szFile.c_str(), DDS_FLAGS_NONE, nullptr, image);
	else if (ext == L".tga" || ext == L".TGA")
		LoadFromTGAFile(szFile.c_str(), nullptr, image);
	else // png, jpg, jpeg, bmp
		LoadFromWICFile(szFile.c_str(), WIC_FLAGS_NONE, nullptr, image);

	ID3D11Texture2D* pTexture = nullptr;
	HR(DirectX::CreateTexture(pDevice.Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), ( ID3D11Resource**)&pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip       = 0;
	srvDesc.Texture2D.MipLevels             = 1;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	pTexture->GetDesc(&textureDesc);
	srvDesc.Format = textureDesc.Format;

	ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	HR(pDevice->CreateShaderResourceView(pTexture, &srvDesc, srv.ReleaseAndGetAddressOf()));

	pTextureShaderResourceView = (srv.Get());
	MY_LOG("TextureLoad", EDebugLogLevel::DLL_Display, "Texture Load Success");
}

void AssetManager::LoadTexture(class UTexture* Texture, const nlohmann::json& AssetData)
{
	std::string FilePath  = GEngine->GetDirectoryPath() + std::string{AssetData["FilePath"]};
	auto        WFilePath = std::wstring{FilePath.begin(), FilePath.end()};

	ScratchImage image;

	std::filesystem::path p(WFilePath.c_str());
	std::wstring          ext = p.extension();

	if (ext == L".dds" || ext == L".DDS")
		LoadFromDDSFile(WFilePath.c_str(), DDS_FLAGS_NONE, nullptr, image);
	else if (ext == L".tga" || ext == L".TGA")
		LoadFromTGAFile(WFilePath.c_str(), nullptr, image);
	else // png, jpg, jpeg, bmp
		LoadFromWICFile(WFilePath.c_str(), WIC_FLAGS_NONE, nullptr, image);

	if (!image.GetImages())
	{
		assert(nullptr && "No Valid Path");
	}
	CreateShaderResourceView(GDirectXDevice->GetDevice().Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), Texture->SRView.GetAddressOf());
	Texture->SRView->GetResource((ID3D11Resource**)Texture->Texture2D.GetAddressOf());
	Texture->Texture2D->GetDesc(&Texture->Desc);

	assert(Texture->GetSRV()&& "No valid Texture");

	MY_LOG("TextureLoad", EDebugLogLevel::DLL_Display, "Texture Load Success");
}

std::shared_ptr<UTexture> AssetManager::CreateTexture(const std::string& Name, UINT Width, UINT Height, DXGI_FORMAT PixelFormat, UINT BindFlag, D3D11_USAGE Usage)
{
	if (!GDirectXDevice || !GDirectXDevice->GetDevice())
	{
		return nullptr;
	}
	auto Texture = std::make_shared<UTexture>();

	Texture->Desc.Format             = PixelFormat;
	Texture->Desc.Width              = Width;
	Texture->Desc.Height             = Height;
	Texture->Desc.ArraySize          = 1;
	Texture->Desc.BindFlags          = BindFlag;
	Texture->Desc.CPUAccessFlags     = 0;
	Texture->Desc.Usage              = Usage;
	Texture->Desc.MipLevels          = 1;
	Texture->Desc.SampleDesc.Count   = 1;
	Texture->Desc.SampleDesc.Quality = 0;
	Texture->Desc.MiscFlags          = 0;

	HRESULT hr = GDirectXDevice->GetDevice()->CreateTexture2D(&Texture->Desc, nullptr, Texture->Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		assert(0 && "Texture 생성 실패");
		return nullptr;
	}

	// View 제작
	if (Texture->Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateRenderTargetView(Texture->Texture2D.Get(), nullptr, Texture->RTView. GetAddressOf())))
			assert(0 && "RTV 생성 실패");
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateDepthStencilView(Texture->Texture2D.Get(), nullptr, Texture->DSView. GetAddressOf())))
			assert(0 && "DSV 생성 실패");
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateShaderResourceView(Texture->Texture2D.Get(), nullptr, Texture->SRView. GetAddressOf())))
			assert(0 && "SRV 생성 실패");
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateUnorderedAccessView(Texture->Texture2D.Get(), nullptr, Texture->UAView. GetAddressOf())))
			assert(0 && "UAV 생성 실패");
	}

	GetAssetCacheMap()[Name] = Texture;

	return Texture;
}

std::shared_ptr<UTexture> AssetManager::CreateCubeTexture(const std::string& Name, UINT Width, UINT Height, DXGI_FORMAT PixelFormat, UINT BindFlag, D3D11_USAGE Usage)
{
	if (!GDirectXDevice || !GDirectXDevice->GetDevice())
	{
		return nullptr;
	}
	std::shared_ptr<UTexture> Texture = std::make_shared<UTexture>();

	Texture->Desc.Format             = PixelFormat;
	Texture->Desc.Width              = Width;
	Texture->Desc.Height             = Height;
	Texture->Desc.ArraySize          = 6;
	Texture->Desc.BindFlags          = BindFlag;
	Texture->Desc.CPUAccessFlags     = 0;
	Texture->Desc.Usage              = Usage;
	Texture->Desc.MipLevels          = 1;
	Texture->Desc.SampleDesc.Count   = 1;
	Texture->Desc.SampleDesc.Quality = 0;
	Texture->Desc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	HRESULT hr = GDirectXDevice->GetDevice()->CreateTexture2D(&Texture->Desc, nullptr, Texture->Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		assert(0 && "CubeTexture 생성 실패");
		return nullptr;
	}

	// View 제작
	if (Texture->Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
		RTVDesc.Format = Texture->Desc.Format;
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		RTVDesc.Texture2DArray.MipSlice = 0;
		RTVDesc.Texture2DArray.FirstArraySlice = 0;
		RTVDesc.Texture2DArray.ArraySize = 6;

		if (FAILED(GDirectXDevice->GetDevice()->CreateRenderTargetView(Texture->Texture2D.Get(), &RTVDesc, Texture->RTView. GetAddressOf())))
		{
			assert(0 && "RTV 생성 실패");
		}
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
		DSVDesc.Format = Texture->Desc.Format;
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSVDesc.Texture2DArray.MipSlice = 0;
		DSVDesc.Texture2DArray.FirstArraySlice = 0;
		DSVDesc.Texture2DArray.ArraySize = 6;

		if (FAILED(GDirectXDevice->GetDevice()->CreateDepthStencilView(Texture->Texture2D.Get(), &DSVDesc, Texture->DSView. GetAddressOf())))
		{
			assert(0 && "DSV 생성 실패");
		}
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		SRVDesc.Format = Texture->Desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Texture2DArray.MipLevels = Texture->Desc.MipLevels;
		
		SRVDesc.Texture2DArray.MostDetailedMip = 0;
		if (FAILED(GDirectXDevice->GetDevice()->CreateShaderResourceView(Texture->Texture2D.Get(), &SRVDesc, Texture->SRView. GetAddressOf())))
		{
			assert(0 && "SRV 생성 실패");
		}
	}

	if (Texture->Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateUnorderedAccessView(Texture->Texture2D.Get(), nullptr, Texture->UAView. GetAddressOf())))
			assert(0 && "UAV 생성 실패");
	}

	GetAssetCacheMap()[Name] = Texture;

	return Texture;
}

UObject* AssetManager::ReadMyAsset(const std::string& FilePath)
{
	std::unordered_map<std::string, std::shared_ptr<UObject>>& AssetCache = GetAssetCacheMap();
	if (AssetCache.contains(FilePath.data()))
	{
		return AssetCache[FilePath.data()].get();
	}

	std::ifstream AssetFile(FilePath.data());
	if (!AssetFile.is_open())
	{
		/*
			MY_LOG("(AssetManager::ReadMyAsset) Failed open file", EDebugLogLevel::DLL_Warning, std::string(FilePath.data()));*/
		assert(nullptr && "NoFile");
	}
	nlohmann::json AssetData = nlohmann::json::parse(AssetFile);

	std::string Test = AssetData["Class"];

	std::shared_ptr<UObject> Object = UObject::GetDefaultObject(AssetData["Class"])->CreateInstance();
	Object->LoadDataFromFileData(AssetData);
	//AssetNameAndAssetPathMap[Object->GetName()] = FilePath;

	AssetCache[Object->GetName()] = Object;
	MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, FilePath+" Load Success");
	return Object.get();
}

void AssetManager::ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices, std::vector<std::vector<UINT>>& allIndices, bool bISUEcoord)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		// 각 메쉬의 버텍스와 인덱스 데이터를 저장할 벡터
		std::vector<MyVertexData> vertices;
		std::vector<UINT>         indices;

		// 메쉬 데이터 처리
		ProcessMesh(mesh, vertices, indices, bISUEcoord);

		// 결과를 전체 리스트에 추가
		allVertices.push_back(vertices);
		allIndices.push_back(indices);
	}
}

void AssetManager::ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices, bool bIsUECoord)
{
	// 1. 버텍스 데이터 추출
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		MyVertexData vertex;

		// 정점 위치
		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		// 법선 벡터 (존재 여부 확인)
		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}
		if (nullptr != mesh->mTangents && nullptr != mesh->mBitangents /*mesh->HasTangentsAndBitangents()*/)
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			vertex.Bitangent.x = mesh->mBitangents[i].x;
			vertex.Bitangent.y = mesh->mBitangents[i].y;
			vertex.Bitangent.z = mesh->mBitangents[i].z;
		}

		// 텍스처 좌표 (첫 번째 텍스처 채널만 사용)
		if (mesh->mTextureCoords[0])
		{
			// 텍스처 좌표가 존재하는지 확인
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.TexCoords.x = 0.0f;
			vertex.TexCoords.y = 0.0f;
		}

		// UE Coord -> Lee Coord
		if (bIsUECoord)
		{
			XMVECTOR UECoordToLeeCoord = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.0f),XMConvertToRadians(0.0f),XMConvertToRadians(0.0f));

			XMStoreFloat3(&vertex.Pos, XMVector3Rotate(XMLoadFloat3(&vertex.Pos), UECoordToLeeCoord));
			XMStoreFloat3(&vertex.Normal, XMVector3Rotate(XMLoadFloat3(&vertex.Normal), UECoordToLeeCoord));
			XMStoreFloat3(&vertex.Tangent, XMVector3Rotate(XMLoadFloat3(&vertex.Tangent), UECoordToLeeCoord));
			XMStoreFloat3(&vertex.Bitangent, XMVector3Rotate(XMLoadFloat3(&vertex.Bitangent), UECoordToLeeCoord));

			
		}

		vertices.push_back(vertex);
	}

	// 2. 인덱스 데이터 추출
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}

void AssetManager::SetVertexBoneData(MyVertexData& vertexData, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertexData.m_BoneIDs[i] < 0)
		{
			vertexData.m_Weights[i] = weight;
			vertexData.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void AssetManager::ExtractBoneWeightForVertices(std::vector<MyVertexData>& vVertexData, aiMesh* mesh, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	for (UINT boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		int         boneID   = -1;
		if (!modelBoneInfoMap.contains(boneName))
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id     = boneIndex;
			aiMatrix4x4 aiMat  = mesh->mBones[boneIndex]->mOffsetMatrix;
			newBoneInfo.offset = XMMATRIX(
				aiMat.a1,aiMat.a2,aiMat.a3,aiMat.a4,
				aiMat.b1,aiMat.b2,aiMat.b3,aiMat.b4,
				aiMat.c1,aiMat.c2,aiMat.c3,aiMat.c4,
				aiMat.d1,aiMat.d2,aiMat.d3,aiMat.d4
			);
			newBoneInfo.offset = XMMatrixTranspose(newBoneInfo.offset);
			//ConvertAiMatrixToXMMATRIX(mesh->mBones[boneIndex]->mOffsetMatrix);
			if (boneName.contains("mixamorig:"))
			{
				boneName.replace(boneName.begin(), boneName.begin() + 10, "");
			}
			modelBoneInfoMap[boneName] = newBoneInfo;
			boneID                     = boneIndex;
		}
		else
		{
			boneID = modelBoneInfoMap[boneName].id;
		}
		assert(boneID != -1);

		auto weights    = mesh->mBones[boneIndex]->mWeights;
		int  numWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int   vertexID = weights[weightIndex].mVertexId;
			float weight   = weights[weightIndex].mWeight;
			SetVertexBoneData(vVertexData[vertexID], boneID, weight);
		}
	}
}

void AssetManager::GetAsyncAssetCache(const std::string& AssetName, const AssetLoadedCallback& LoadedCallback)
{
	// 엔진이 초기화 되기 전에는 비동기 에셋 로드를 진행하지 않음
	// 엔진 생성 이전 CDO를 만드는 과정에서 발생할 수 있기 때문에 방지
	if(!GEngine)
	{
		return;
	}

	// 이미 로드한 적이 있어 캐시에 있다면 바로 콜백함수 적용
	const auto Iter = AsyncAssetCache.find(AssetName);
	if(Iter != AsyncAssetCache.end())
	{
		if(Iter->second.expired())
		{
			MY_LOG("Destroy AsyncAssetCache", EDebugLogLevel::DLL_Display, Iter->first);
			AsyncAssetCache.unsafe_erase(Iter); 
		}
		else
		{
			std::shared_ptr<UObject> SharedObj = Iter->second.lock();
			LoadedCallback(SharedObj);
			return;
		}
	}

	// 여기까지 넘어오면 현재 로드가 안된 상태이므로 로드를 진행해줘야함

	// 만약 해당 에셋이 로드중이라면 콜백만 추가하고 종료
	{
		auto FindLoading = LoadingCallbackMap.find(AssetName);
		if(FindLoading != LoadingCallbackMap.end())
		{
			FindLoading->second.push_back(LoadedCallback);
			
			return;
		}
	}

	// 로드 진행
	LoadingCallbackMap[AssetName] = concurrency::concurrent_vector<AssetLoadedCallback>{};
	LoadingCallbackMap[AssetName].push_back(LoadedCallback);

	FTask Task{0, [AssetName]()
	{
		const std::string FilePath = AssetManager::GetAssetNameAndAssetPathMap()[AssetName];

		if (FilePath.empty())
		{
			//07.01 해당부분은 assert를 걸어야 테스트가 편하므로 assert로 수정
			//MY_LOG("AsyncLoadError", EDebugLogLevel::DLL_Error, AssetName + "Not Found");
			assert(nullptr && "잘못된 에셋 경로");
			return;
		}

		std::ifstream AssetFile(FilePath.data());
		if (!AssetFile.is_open())
		{
			assert(nullptr && "잘못된 에셋 경로");
		}
		nlohmann::json AssetData = nlohmann::json::parse(AssetFile);

		std::shared_ptr<UObject> Object = UObject::GetDefaultObject(AssetData["Class"])->CreateInstance();
		Object->LoadDataFromFileData(AssetData);

		AsyncAssetCache[AssetName] = Object;

		for(const AssetLoadedCallback& Callback : LoadingCallbackMap[AssetName])
		{
			Callback(Object);
		}
		LoadingCallbackMap.unsafe_erase(AssetName);

		MY_LOG("AsyncAssetLoad", EDebugLogLevel::DLL_Display, FilePath+" Load Success");
	}, nullptr};

	GThreadPool->AddTask(Task);
}
