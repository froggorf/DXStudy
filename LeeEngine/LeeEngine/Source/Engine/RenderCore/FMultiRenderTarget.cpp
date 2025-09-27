#include "CoreMinimal.h"
#include "FMultiRenderTarget.h"

void FMultiRenderTarget::Create(std::shared_ptr<UTexture>* InRTTex, UINT InRTCount, std::shared_ptr<UTexture> InDSTex)
{
	RenderTargetCount = InRTCount;
	for (UINT i = 0; i < RenderTargetCount; ++i)
	{
		RenderTarget[i] = InRTTex[i];
	}

	DepthStencilTexture = InDSTex;
}

void FMultiRenderTarget::SetClearColor(XMFLOAT4* InClearColors, UINT InRTCount)
{
	for (UINT i = 0; i < InRTCount; ++i)
	{
		ClearColor[i] = InClearColors[i];
	}
}

void FMultiRenderTarget::OMSet()
{
	ID3D11RenderTargetView* RTV[8] = {};

	for (UINT i = 0; i < RenderTargetCount; ++i)
	{
		RTV[i] = RenderTarget[i]->GetRTV().Get();
	}

	if(nullptr != DepthStencilTexture)
		GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(RenderTargetCount, RTV, DepthStencilTexture->GetDSV().Get());
	else
		GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(RenderTargetCount, RTV, nullptr);

	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, &Viewport);
}

void FMultiRenderTarget::ClearRenderTarget()
{
	for (UINT i = 0; i < RenderTargetCount; ++i)
	{
		const float CurClearColor[4] = {ClearColor[i].x,ClearColor[i].y,ClearColor[i].z,ClearColor[i].w};
		GDirectXDevice->GetDeviceContext()->ClearRenderTargetView(RenderTarget[i]->GetRTV().Get(), CurClearColor);
	}
}

void FMultiRenderTarget::ClearDepthStencilTarget()
{
	if (nullptr == DepthStencilTexture)
		return;

	GDirectXDevice->GetDeviceContext()->ClearDepthStencilView(DepthStencilTexture->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
