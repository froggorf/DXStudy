#include "CoreMinimal.h"
#include "FDirect2DDevice.h"
using namespace Microsoft::WRL;

bool FDirect2DDevice::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device>& D3DDevice)
{
	HRESULT hr;
	D2D1_FACTORY_OPTIONS options = {};
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(m_d2dFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		assert(nullptr && "D2D1CreateFactory");
		return false;
	}
	ComPtr<IDXGIDevice> dxgiDevice;
	hr = D3DDevice.As(&dxgiDevice);
	if (FAILED(hr))
	{
		assert(nullptr && "D3DDevice.As");
		return false;
	}
	hr = m_d2dFactory->CreateDevice(dxgiDevice.Get(), m_d2dDevice.GetAddressOf());
	if (FAILED(hr))
	{
		assert(nullptr && "CreateDevice");
		return false;
	}

	hr = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dContext.GetAddressOf());
	if (FAILED(hr))
	{
		assert(nullptr && "CreateDeviceContext");
		return false;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1), reinterpret_cast<IUnknown**>(m_writeFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		assert(nullptr && "DWriteCreateFactory");
		return false;
	}

	ComPtr<ID3D11Texture2D> pBackBuffer;
#ifdef WITH_EDITOR
	pBackBuffer = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor)->GetRenderTargetTexture(0)->GetTexture();
#else
	pBackBuffer = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->GetRenderTargetTexture(0)->GetTexture();
#endif

	ComPtr<IDXGISurface> dxgiSurface;
	hr = pBackBuffer.As(&dxgiSurface);
	if (FAILED(hr))
	{
		assert(nullptr && "pBackBuffer.As");
		return false;
	}

	D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	ComPtr<ID2D1Bitmap1> d2dBitmap;
	hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, d2dBitmap.GetAddressOf());
	if (FAILED(hr))
	{
		assert(nullptr && "CreateBitmapFromDxgiSurface");
		return false;
	}
	// D2D 컨텍스트에 타겟 지정
	m_d2dContext->SetTarget(d2dBitmap.Get());

	return true;
}

void FDirect2DDevice::CreateTextFormat(const std::wstring& Font, float FontSize, Microsoft::WRL::ComPtr<IDWriteTextFormat>& TextFormat) const
{
	m_writeFactory->CreateTextFormat(
		Font.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		FontSize, L"ko-kr", TextFormat.GetAddressOf());
}
