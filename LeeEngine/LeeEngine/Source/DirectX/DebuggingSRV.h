#pragma once

// 그림자맵, 렌더링 버퍼 등 ShaderResourceView에 정상적으로 렌더링 되어있는지 확인을 하기 위한 클래스

// 디버깅 모드에서만 사용 가능하며,
// Direct3D 초기화 후 DebuggingSRV::InitializeDebuggingSRV(..) 필요
#ifndef DEBUGGING_SRV
#define DEBUGGING_SRV

#if defined(DEBUG) || defined(_DEBUG)  
#include <wrl/client.h>
#include <d3d11_1.h>

struct ID3D11SamplerState;
class DebuggingSRV
{
public:
	static void InitializeDebuggingSRV(const Microsoft::WRL::ComPtr<ID3D11Device>& deviceContext);
	static void DrawDebuggingTexture(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext ,float topX, float topY,float width, float height, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);

private:
	static bool bIsInitialized;
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	static Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
};


#endif

#endif
