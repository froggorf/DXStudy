#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class ShadowMap
{
public:
	ShadowMap(Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height);
	~ShadowMap();

	ID3D11ShaderResourceView*	GetShaderResourceView() const {return m_DepthMapSRV.Get();}
	ID3D11DepthStencilView*		GetDepthStencilView() const {return m_DepthMapDSV.Get();}
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  GetShaderResourceViewComPtr() const {return m_DepthMapSRV; }

	void BindDsvAndSetNullRenderTarget(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext);

private:
	UINT m_Width;
	UINT m_Height;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthMapDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_DepthMapSRV;

	D3D11_VIEWPORT m_Viewport;
};
