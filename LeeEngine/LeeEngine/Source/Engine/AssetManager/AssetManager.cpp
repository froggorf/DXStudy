#include "CoreMinimal.h"
#include "AssetManager.h"

#include "Engine/Class/UTexture.h"

using namespace Microsoft::WRL;


std::unordered_map<std::string, std::shared_ptr<UObject>> AssetManager::AssetCache;
std::unordered_map<std::string, std::string> AssetManager::AssetNameAndAssetPathCacheMap;

void AssetManager::LoadModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, std::vector<ComPtr<ID3D11Buffer>>& pVertexBuffer,
	std::vector<ComPtr<ID3D11Buffer>>&pIndexBuffer)
{
    std::string filePath = GEngine->GetDirectoryPath() + path;

    Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder );
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	    return;
	}

	std::vector<std::vector<MyVertexData>> allVertices;
	std::vector<std::vector<UINT>> allIndices;

	ProcessScene(scene, allVertices, allIndices);
	
	// 결과 출력
    std::cout << "Path : " << path<< std::endl;
    for (size_t i = 0; i < allVertices.size(); i++) {
        std::cout << "Mesh " << i << ":\n";
        std::cout << "  Vertices: " << allVertices[i].size() << "\n";
        std::cout << "  Indices: " << allIndices[i].size() << "\n";
    }

    // 모델 로드 성공
    MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "Model - " + filePath+" Load Success");
	for(int i = 0; i < scene->mNumMeshes; ++i)
	{
		// 버텍스 버퍼
        ComPtr<ID3D11Buffer> vertexBuffer;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(MyVertexData) * allVertices[i].size();
		D3D11_SUBRESOURCE_DATA initVertexData = {};
		initVertexData.pSysMem = allVertices[i].data();
		HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
        pVertexBuffer.push_back(vertexBuffer);

		// 인덱스 버퍼
        ComPtr<ID3D11Buffer> indexBuffer;
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * allIndices[i].size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initIndexData = {};
		initIndexData.pSysMem = allIndices[i].data();
		HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
        pIndexBuffer.push_back(indexBuffer);
		
	}
}

void AssetManager::LoadModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11Buffer>& pVertexBuffer,
	ComPtr<ID3D11Buffer>&pIndexBuffer)
{
    std::string filePath = GEngine->GetDirectoryPath() + path;

    Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder );
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	    return;
	}

	std::vector<std::vector<MyVertexData>> allVertices;
	std::vector<std::vector<UINT>> allIndices;

	ProcessScene(scene, allVertices, allIndices);
	
	// 결과 출력
    std::cout << "Path : " << path<< std::endl;
    for (size_t i = 0; i < allVertices.size(); i++) {
        std::cout << "Mesh " << i << ":\n";
        std::cout << "  Vertices: " << allVertices[i].size() << "\n";
        std::cout << "  Indices: " << allIndices[i].size() << "\n";
    }

    // 모델 로드 성공
    MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "Model - " + filePath+" Load Success");

	// 버텍스 버퍼
       ComPtr<ID3D11Buffer> vertexBuffer;
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(MyVertexData) * allVertices[0].size();
	D3D11_SUBRESOURCE_DATA initVertexData = {};
	initVertexData.pSysMem = allVertices[0].data();
	HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
    //   pVertexBuffer.push_back(vertexBuffer);
    pVertexBuffer = (vertexBuffer.Get());

	// 인덱스 버퍼
       ComPtr<ID3D11Buffer> indexBuffer;
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * allIndices[0].size();
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA initIndexData = {};
	initIndexData.pSysMem = allIndices[0].data();
	HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
	//pIndexBuffer.push_back(indexBuffer);
    pIndexBuffer = (indexBuffer.Get());
		
	
}


void AssetManager::LoadSkeletalModelData(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer,
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pIndexBuffer, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
    // https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
    std::string filePath = GEngine->GetDirectoryPath() + path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, 
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    std::vector<std::vector<MyVertexData>> allVertices;
    std::vector<std::vector<UINT>> allIndices;

    ProcessScene(scene, allVertices, allIndices);

    // 결과 출력
    std::cout << "Path : " << path<< std::endl;
    for (size_t i = 0; i < allVertices.size(); i++) {
        std::cout << "Mesh " << i << ":\n";
        std::cout << "  Vertices: " << allVertices[i].size() << "\n";
        std::cout << "  Indices: " << allIndices[i].size() << "\n";
    }

    // VertexData를 SkeletalVertexData로 변환
    std::vector<std::vector<MySkeletalMeshVertexData>> allSkeletalVertices(scene->mNumMeshes);
    for(int meshIndex = 0; meshIndex < allVertices.size(); ++meshIndex)
    {
        
		allSkeletalVertices[meshIndex].resize(allVertices[meshIndex].size());
        
        for (int vertexIndex = 0; vertexIndex < allVertices[meshIndex].size(); ++vertexIndex)
        {
            allSkeletalVertices[meshIndex][vertexIndex].Pos =  allVertices[meshIndex][vertexIndex].Pos;
            allSkeletalVertices[meshIndex][vertexIndex].Normal = allVertices[meshIndex][vertexIndex].Normal;
            allSkeletalVertices[meshIndex][vertexIndex].TexCoords = allVertices[meshIndex][vertexIndex].TexCoords;
            for(int maxBoneInfluenceCount = 0; maxBoneInfluenceCount < MAX_BONE_INFLUENCE; ++maxBoneInfluenceCount)
            {
                allSkeletalVertices[meshIndex][vertexIndex].m_Weights[maxBoneInfluenceCount] = 0.0f;
                allSkeletalVertices[meshIndex][vertexIndex].m_BoneIDs[maxBoneInfluenceCount] = -1;
            }
        }
    }
    

    for(int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        ExtractBoneWeightForVertices(allSkeletalVertices[meshIndex], scene->mMeshes[meshIndex], modelBoneInfoMap);
    }

    // 모델 로드 성공
    MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, "SkeletalModel - " + filePath+" Load Success");

    for(int i = 0; i < scene->mNumMeshes; ++i)
    {
        // 버텍스 버퍼
        ComPtr<ID3D11Buffer> vertexBuffer;
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(MySkeletalMeshVertexData) * allSkeletalVertices[i].size();
        D3D11_SUBRESOURCE_DATA initVertexData = {};
        initVertexData.pSysMem = allSkeletalVertices[i].data();
        HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
        pVertexBuffer.push_back(vertexBuffer);

        // 인덱스 버퍼
        ComPtr<ID3D11Buffer> indexBuffer;
        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.ByteWidth = sizeof(UINT) * allIndices[i].size();
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA initIndexData = {};
        initIndexData.pSysMem = allIndices[i].data();
        HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
        pIndexBuffer.push_back(indexBuffer);

    }
}


void AssetManager::LoadTextureFromFile(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView)
{
    DirectX::ScratchImage image;

    std::filesystem::path p(szFile.c_str());
    std::wstring ext = p.extension();

	if (ext == L".dds" || ext == L".DDS")
		DirectX::LoadFromDDSFile(szFile.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	else if (ext == L".tga" || ext == L".TGA")
		DirectX::LoadFromTGAFile(szFile.c_str(), nullptr, image);
	else // png, jpg, jpeg, bmp
		DirectX::LoadFromWICFile(szFile.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);

    
    ID3D11Texture2D* pTexture = nullptr;
    HR(DirectX::CreateTexture(pDevice.Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&pTexture));
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    pTexture->GetDesc(&textureDesc);
    srvDesc.Format = textureDesc.Format;
    

    ComPtr<ID3D11ShaderResourceView> srv = nullptr;

    HR(pDevice->CreateShaderResourceView(pTexture, &srvDesc, srv.ReleaseAndGetAddressOf()));
    vTextureShaderResourceView.push_back(srv);
    
    MY_LOG("TextureLoad", EDebugLogLevel::DLL_Display, "Texture Load Success");
}

void AssetManager::LoadTextureFromFile(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& pTextureShaderResourceView)
{
    
    DirectX::ScratchImage image;

    std::filesystem::path p(szFile.c_str());
    std::wstring ext = p.extension();

	if (ext == L".dds" || ext == L".DDS")
		DirectX::LoadFromDDSFile(szFile.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	else if (ext == L".tga" || ext == L".TGA")
		DirectX::LoadFromTGAFile(szFile.c_str(), nullptr, image);
	else // png, jpg, jpeg, bmp
		DirectX::LoadFromWICFile(szFile.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);

    
    ID3D11Texture2D* pTexture = nullptr;
    HR(DirectX::CreateTexture(pDevice.Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&pTexture));
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

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
    std::string FilePath = GEngine->GetDirectoryPath() + std::string{AssetData["FilePath"]};
	std::wstring WFilePath = std::wstring{FilePath.begin(), FilePath.end()};

    DirectX::ScratchImage image;

    std::filesystem::path p(WFilePath.c_str());
    std::wstring ext = p.extension();

    if (ext == L".dds" || ext == L".DDS")
        DirectX::LoadFromDDSFile(WFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    else if (ext == L".tga" || ext == L".TGA")
        DirectX::LoadFromTGAFile(WFilePath.c_str(), nullptr, image);
    else // png, jpg, jpeg, bmp
        DirectX::LoadFromWICFile(WFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);

    DirectX::CreateShaderResourceView(GDirectXDevice->GetDevice().Get(),
									  image.GetImages(),
									  image.GetImageCount(),
                                      image.GetMetadata(),
                                      Texture->SRView.GetAddressOf());
    Texture->SRView->GetResource((ID3D11Resource**)Texture->Texture2D.GetAddressOf());
    Texture->Texture2D->GetDesc(&Texture->Desc);

    MY_LOG("TextureLoad", EDebugLogLevel::DLL_Display, "Texture Load Success");
}

std::shared_ptr<UTexture> AssetManager::CreateTexture(const std::string& Name, UINT Width, UINT Height, DXGI_FORMAT PixelFormat, UINT BindFlag, D3D11_USAGE Usage)
{
	std::shared_ptr<UTexture> Texture = std::make_shared<UTexture>();
    
	Texture->Desc.Format = PixelFormat;
    Texture->Desc.Width = Width;
    Texture->Desc.Height = Height;
    Texture->Desc.ArraySize = 1;
    Texture->Desc.BindFlags = BindFlag;
    Texture->Desc.CPUAccessFlags = 0;
    Texture->Desc.Usage = Usage;
    Texture->Desc.MipLevels = 1; 
    Texture->Desc.SampleDesc.Count = 1;
    Texture->Desc.SampleDesc.Quality = 0;
    Texture->Desc.MiscFlags = 0;

    HRESULT hr = GDirectXDevice->GetDevice()->CreateTexture2D(&Texture->Desc, nullptr, Texture->Texture2D.GetAddressOf());
    if (FAILED(hr))
    {
        assert(0 && "Texture 생성 실패");
        return nullptr;
    }

    // View 제작
    if (Texture->Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
    {
        if (FAILED(GDirectXDevice->GetDevice()->CreateRenderTargetView(Texture->Texture2D.Get(), nullptr, Texture->RTView.GetAddressOf())))
            assert(0 && "RTV 생성 실패");
    }

    if (Texture->Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        if (FAILED(GDirectXDevice->GetDevice()->CreateDepthStencilView(Texture->Texture2D.Get(), nullptr, Texture->DSView.GetAddressOf())))
            assert(0 && "DSV 생성 실패");
    }

    if (Texture->Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        if (FAILED(GDirectXDevice->GetDevice()->CreateShaderResourceView(Texture->Texture2D.Get(), nullptr, Texture->SRView.GetAddressOf())))
            assert(0 && "SRV 생성 실패");   
    }

    if (Texture->Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        if (FAILED(GDirectXDevice->GetDevice()->CreateUnorderedAccessView(Texture->Texture2D.Get(), nullptr, Texture->UAView.GetAddressOf())))
            assert(0 && "UAV 생성 실패");
    }

    UTexture::TextureCacheMap[Name] = Texture;

    return Texture;
}

UObject* AssetManager::ReadMyAsset(const std::string& FilePath)
{
    if(AssetCache.find(FilePath.data())!= AssetCache.end())
    {
        return reinterpret_cast<UObject*>(AssetCache[FilePath.data()].get());
    }

    std::ifstream AssetFile(FilePath.data());
    if(!AssetFile.is_open())
    {/*
     MY_LOG("(AssetManager::ReadMyAsset) Failed open file", EDebugLogLevel::DLL_Warning, std::string(FilePath.data()));*/
        assert(1);
    }
    nlohmann::json AssetData = nlohmann::json::parse(AssetFile);

    std::string Test = AssetData["Class"];
   
    std::shared_ptr<UObject> Object = UObject::GetDefaultObject(AssetData["Class"])->CreateInstance();
    Object->LoadDataFromFileData(AssetData);
    AssetNameAndAssetPathCacheMap[Object->GetName()] = FilePath;

    AssetCache[FilePath.data()] = Object;
    MY_LOG("AssetLoad", EDebugLogLevel::DLL_Display, FilePath+" Load Success");
    return Object.get();

}


void AssetManager::ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices,
                                std::vector<std::vector<UINT>>& allIndices)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // 각 메쉬의 버텍스와 인덱스 데이터를 저장할 벡터
        std::vector<MyVertexData> vertices;
        std::vector<UINT> indices;

        // 메쉬 데이터 처리
        ProcessMesh(mesh, vertices, indices);

        // 결과를 전체 리스트에 추가
        allVertices.push_back(vertices);
        allIndices.push_back(indices);
    }
}

void AssetManager::ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices)
{
// 1. 버텍스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        MyVertexData vertex;
		
        // 정점 위치
        vertex.Pos.x = mesh->mVertices[i].x;
        vertex.Pos.y = mesh->mVertices[i].y;
        vertex.Pos.z = mesh->mVertices[i].z;

        // 법선 벡터 (존재 여부 확인)
        if (mesh->HasNormals()) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        // 텍스처 좌표 (첫 번째 텍스처 채널만 사용)
        if (mesh->mTextureCoords[0]) { // 텍스처 좌표가 존재하는지 확인
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.TexCoords.x = 0.0f;
            vertex.TexCoords.y = 0.0f;
        }

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }	
}

void AssetManager::SetVertexBoneData(MySkeletalMeshVertexData& vertexData, int boneID, float weight)
{
    for(int i = 0; i< MAX_BONE_INFLUENCE; ++i)
    {
        if(vertexData.m_BoneIDs[i] < 0)
        {
            vertexData.m_Weights[i] = weight;
            vertexData.m_BoneIDs[i] = boneID;
            break;
        }
    }
}

void AssetManager::ExtractBoneWeightForVertices(std::vector<MySkeletalMeshVertexData>& vVertexData, aiMesh* mesh,
	std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        int boneID = -1;
        if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneIndex;
            aiMatrix4x4 aiMat = mesh->mBones[boneIndex]->mOffsetMatrix;
            newBoneInfo.offset = DirectX::XMMATRIX(
                aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,  // 1열
                aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,  // 2열
                aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,  // 3열
                aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4   // 4열
            );
            //ConvertAiMatrixToXMMATRIX(mesh->mBones[boneIndex]->mOffsetMatrix);
            if(boneName.contains("mixamorig:"))
            {
                boneName.replace(boneName.begin(),boneName.begin()+10, "");
            }
            modelBoneInfoMap[boneName] = newBoneInfo;
            boneID = boneIndex;
        }
        else
        {
            boneID = modelBoneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        

        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexID = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            SetVertexBoneData(vVertexData[vertexID], boneID, weight);
        }
    }
}

