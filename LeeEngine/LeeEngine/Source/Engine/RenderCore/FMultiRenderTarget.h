// 07.26
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/Class/UTexture.h"

enum class EMultiRenderTargetType
{
    Deferred,
    Decal,
    Light,
    Editor,
    SwapChain,	
    Count,
};

class FMultiRenderTarget
{
private:
    std::shared_ptr<UTexture>   RenderTarget[8];     
    std::shared_ptr<UTexture>   DepthStencilTexture; 
    UINT                        RenderTargetCount{};

    XMFLOAT4                    ClearColor[8]{};
    D3D11_VIEWPORT              Viewport{};     

public:
    void Create(std::shared_ptr<UTexture>* InRTTex, UINT InRTCount, std::shared_ptr<UTexture> InDSTex);
    void SetClearColor(XMFLOAT4* InClearColors, UINT InRTCount);
    void OMSet();
    void ClearRenderTarget();
    void ClearDepthStencilTarget();

    std::shared_ptr<UTexture> GetRenderTargetTexture(UINT Idx) {  assert(Idx < 8); return RenderTarget[Idx]; }
    std::shared_ptr<UTexture> GetDepthStencilTexture() { return DepthStencilTexture; }

};
