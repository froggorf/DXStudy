//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include "../../Core/d3dApp.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct FrameConstantBuffer
{
	XMMATRIX View;
	XMMATRIX Projection;
};

struct ObjConstantBuffer
{
	XMMATRIX World;
};
 
class DrawFromFileApp : public D3DApp
{
public:
	DrawFromFileApp(HINSTANCE hInstance);
	~DrawFromFileApp();

	// Init
	bool Init() override;
	void OnResize() override;
	void UpdateScene(float dt) override;
	void DrawScene() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:	
	void TestLoadModel(const std::string& path);
	void ProcessMesh(aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	void ProcessScene(const aiScene* scene, std::vector<std::vector<Vertex>>& allVertices, std::vector<std::vector<unsigned int>>& allIndices);
	
	void BuildShader();



private:
	std::vector<ComPtr<ID3D11Buffer>>		m_ModelVertexBuffer;
	std::vector<ComPtr<ID3D11Buffer>> 		m_ModelIndexBuffer;

	ComPtr<ID3D11VertexShader>	m_VertexShader;
	ComPtr<ID3D11PixelShader>	m_PixelShader;
	ComPtr<ID3D11InputLayout>	m_InputLayout;

	ComPtr<ID3D11Buffer>		m_FrameConstantBuffer;
	ComPtr<ID3D11Buffer>		m_ObjConstantBuffer;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;

	POINT m_LastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	AllocConsole();
	FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

	DrawFromFileApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

DrawFromFileApp::DrawFromFileApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	m_MainWndTitle = L"Draw 3D Mesh";

	m_Radius = 5.0f; m_Phi = 30.0f; m_Theta = 30.0f;
	m_LastMousePos.x = 0; m_LastMousePos.y = 1;
	m_World = XMMatrixIdentity();
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();
		
}

DrawFromFileApp::~DrawFromFileApp()
{
	// ComPtr
}

bool DrawFromFileApp::Init()
{
	if(!D3DApp::Init())
		return false;

	TestLoadModel("Model/0.obj");
	BuildShader();


	return true;
}

void DrawFromFileApp::OnResize()
{
	D3DApp::OnResize();

	// 프로젝션 매트릭스
	m_Proj = XMMatrixPerspectiveFovLH(0.5*XM_PI, GetWindowAspectRatio(), 1.0f, 1000.0f);
}

void DrawFromFileApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = m_Radius*sinf(m_Phi)*cosf(m_Theta);
	float z = m_Radius*sinf(m_Phi)*sinf(m_Theta);
	float y = m_Radius*cosf(m_Phi);

	// 임시 테스트
	
	XMVECTOR Pos = XMVectorSet(x,y,z,1.0f);
    XMVECTOR At = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
    XMVECTOR Up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

    m_View = XMMatrixLookAtLH(Pos, At, Up);
	
}

void DrawFromFileApp::DrawScene()
{
	const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
	m_d3dDeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_d3dDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	{
		// 셰이더 설정
		m_d3dDeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		m_d3dDeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
		// 상수버퍼 설정
		m_d3dDeviceContext->VSSetConstantBuffers(0, 1, m_FrameConstantBuffer.GetAddressOf());
		m_d3dDeviceContext->VSSetConstantBuffers(1, 1, m_ObjConstantBuffer.GetAddressOf());

		// 인풋 레이아웃
		m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());
		m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Frame 상수 버퍼 설정
		FrameConstantBuffer fcb;
		fcb.View = XMMatrixTranspose(m_View);
		fcb.Projection = XMMatrixTranspose(m_Proj);
		m_d3dDeviceContext->UpdateSubresource(m_FrameConstantBuffer.Get(), 0, nullptr, &fcb, 0, 0);
		
		// 오브젝트 Draw
		// Obj 상수 버퍼 설정
		ObjConstantBuffer ocb;
		float scaleSize = 2.0f;
		XMMATRIX world = m_World * XMMatrixScaling(scaleSize,scaleSize,scaleSize);
		ocb.World = XMMatrixTranspose(world);
		m_d3dDeviceContext->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);

		// 버텍스 버퍼에 맞춰 오브젝트 드로우
		
		for(int vertexCount = 0; vertexCount < m_ModelVertexBuffer.size(); ++vertexCount)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			m_d3dDeviceContext->IASetVertexBuffers(0, 1, m_ModelVertexBuffer[vertexCount].GetAddressOf(), &stride, &offset);
			m_d3dDeviceContext->IASetIndexBuffer(m_ModelIndexBuffer[vertexCount].Get(), DXGI_FORMAT_R32_UINT, 0);
		
			D3D11_BUFFER_DESC indexBufferDesc;
			m_ModelIndexBuffer[vertexCount]->GetDesc(&indexBufferDesc);
			UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
			m_d3dDeviceContext->DrawIndexed(indexSize, 0, 0);
		}
		

	}

	HR(m_SwapChain->Present(0, 0));
}


void DrawFromFileApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void DrawFromFileApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void DrawFromFileApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta += dx;
		m_Phi   += dy;

		// Restrict the angle mPhi.
		m_Phi = std::clamp(m_Phi, 0.1f, XM_PI-0.1f);
		
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = std::clamp(m_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DrawFromFileApp::TestLoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
	    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded );
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	    return;
	}

	std::vector<std::vector<Vertex>> allVertices;
	std::vector<std::vector<UINT>> allIndices;

	ProcessScene(scene, allVertices, allIndices);
	
	// 결과 출력
    for (size_t i = 0; i < allVertices.size(); i++) {
        std::cout << "Mesh " << i << ":\n";
        std::cout << "  Vertices: " << allVertices[i].size() << "\n";
        std::cout << "  Indices: " << allIndices[i].size() << "\n";
    }
	

    // 모델 로드 성공
    std::cout << "Model loaded successfully: " << path << std::endl;

	for(int i = 0; i < scene->mNumMeshes; ++i)
	{
		// 버텍스 버퍼
		ComPtr<ID3D11Buffer> vertexBuffer;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * allVertices[i].size();
		D3D11_SUBRESOURCE_DATA initVertexData = {};
		initVertexData.pSysMem = allVertices[i].data();
		HR(m_d3dDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
		m_ModelVertexBuffer.push_back(vertexBuffer);

		// 인덱스 버퍼
		ComPtr<ID3D11Buffer> indexBuffer;
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * allIndices[i].size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initIndexData = {};
		initIndexData.pSysMem = allIndices[i].data();
		HR(m_d3dDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
		m_ModelIndexBuffer.push_back(indexBuffer);
	}
}

void DrawFromFileApp::ProcessMesh(aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	// 1. 버텍스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
		
        // 정점 위치
        vertex.Pos.x = mesh->mVertices[i].x;
        vertex.Pos.y = mesh->mVertices[i].y;
        vertex.Pos.z = mesh->mVertices[i].z;

        // 법선 벡터 (존재 여부 확인)
        if (mesh->HasNormals()) {
            //vertex.normal[0] = mesh->mNormals[i].x;
            //vertex.normal[1] = mesh->mNormals[i].y;
            //vertex.normal[2] = mesh->mNormals[i].z;

			//test. 노말정보로 색상을 표현
			vertex.Color.x = mesh->mNormals[i].x;
            vertex.Color.y = mesh->mNormals[i].y;
            vertex.Color.z = mesh->mNormals[i].z;
			vertex.Color.w = 1.0f;
        }

        // 텍스처 좌표 (첫 번째 텍스처 채널만 사용)
        //if (mesh->mTextureCoords[0]) { // 텍스처 좌표가 존재하는지 확인
        //    vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
        //    vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
        //} else {
        //    vertex.texCoords[0] = 0.0f;
        //    vertex.texCoords[1] = 0.0f;
        //}

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

void DrawFromFileApp::ProcessScene(const aiScene* scene, std::vector<std::vector<Vertex>>& allVertices,
	std::vector<std::vector<unsigned int>>& allIndices)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // 각 메쉬의 버텍스와 인덱스 데이터를 저장할 벡터
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // 메쉬 데이터 처리
        ProcessMesh(mesh, vertices, indices);

        // 결과를 전체 리스트에 추가
        allVertices.push_back(vertices);
        allIndices.push_back(indices);
    }
}

void DrawFromFileApp::BuildShader()
{
	ComPtr<ID3DBlob> pVSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/color.fx", "VS", "vs_4_0", pVSBlob.GetAddressOf()));
	HR(m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(inputLayout);

	HR(m_d3dDevice->CreateInputLayout(inputLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_InputLayout.GetAddressOf()));
	m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());

	ComPtr<ID3DBlob> pPSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/color.fx", "PS", "ps_4_0", pPSBlob.GetAddressOf()));

	HR(m_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));


	// Constant Buffer 생성
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof( FrameConstantBuffer );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
	HR(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_FrameConstantBuffer.GetAddressOf()));

	bufferDesc.ByteWidth = sizeof(ObjConstantBuffer);
	HR(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_ObjConstantBuffer.GetAddressOf()));

}


