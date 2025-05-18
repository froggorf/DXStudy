#include "CoreMinimal.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height)
	: m_Width{width}, m_Height{height}, m_DepthMapDSV{nullptr}, m_DepthMapSRV{nullptr}
{
	// 그림자 맵에 맞춰 뷰포트 설정
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width    = static_cast<float>(width);
	m_Viewport.Height   = static_cast<float>(height);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// DSV 에서 뎁스만 사용하므로 DXGI_FORMAT_R24G8_TYPELESS 사용
	// 원래는 DXGI_FORMAT_D24_UNORM_S8_UINT // DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.Height             = m_Height;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	// 텍스쳐 생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthMap{};
	HR(device->CreateTexture2D(&texDesc, nullptr, depthMap.GetAddressOf()));

	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags              = 0;
	dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(depthMap.Get(), &dsvDesc, m_DepthMapDSV.GetAddressOf()));

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(depthMap.Get(), &srvDesc, m_DepthMapSRV.GetAddressOf()));
}

ShadowMap::~ShadowMap()
{
	// ComPtr
}

void ShadowMap::BindDsvAndSetNullRenderTarget(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext)
{
	deviceContext->RSSetViewports(1, &m_Viewport);

	// 뎁스 버퍼만을 그리기 위해 렌더타겟 null 설정
	ID3D11RenderTargetView* renderTargets[1] = {nullptr};
	deviceContext->OMSetRenderTargets(1, renderTargets, m_DepthMapDSV.Get());

	deviceContext->ClearDepthStencilView(m_DepthMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
