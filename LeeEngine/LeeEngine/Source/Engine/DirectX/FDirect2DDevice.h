// 08.20
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


// Direct2D + DirectWrite를 활용하여 텍스트를 렌더링 하기 위한 디바이스 클래스
#pragma once
#include "CoreMinimal.h"

class FDirect2DDevice
{
public:
	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device>& D3DDevice);

	const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& Get2DDeviceContext() const {return m_d2dContext;}

	const Microsoft::WRL::ComPtr<IDWriteFactory1>& Test_WriteFactory() const {return m_writeFactory;}
	const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& Test_RenderTarget() const {return m_d2dRenderTarget;}
private:
	Microsoft::WRL::ComPtr<ID2D1Factory1>      m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>        m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_d2dContext;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>  m_d2dRenderTarget;
	Microsoft::WRL::ComPtr<IDWriteFactory1>    m_writeFactory;

};