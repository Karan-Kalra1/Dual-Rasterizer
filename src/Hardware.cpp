//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>


// Standard includes
#include <iostream>

//Project includes
#include "Hardware.h"

using namespace dae;

Hardware::Hardware(SDL_Window* pWindow, int width, int height)
	: m_pWindow(pWindow)
	, m_Width(width)
	, m_Height(height)
{
	//Initialize
	SDL_GetWindowSize(m_pWindow, &m_Width, &m_Height);

	//Initialize DirectX pipeline
	const HRESULT result = InitializeDirectX();
	if (result == S_OK)
	{
		m_IsInitialized = true;
		std::cout << "DirectX is initialized and ready!\n";
	}
	else
	{
		std::cout << "DirectX initialization failed!\n";
	}
	
}

Hardware::~Hardware()
{

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}


	if (m_pRenderTargetBuffer)
	{
		m_pRenderTargetBuffer->Release();
		m_pRenderTargetBuffer = nullptr;
	}

	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}

	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pNoneRasterizerState)
	{
		m_pNoneRasterizerState->Release();
	}

	if (m_pBackRasterizerState)
	{
		m_pBackRasterizerState->Release();
	}

	if (m_pFrontRasterizerState)
	{
		m_pFrontRasterizerState->Release();
	}

	delete m_camera;
	m_camera = nullptr;

	delete m_mesh;
	m_mesh = nullptr;

	delete m_firemesh;
	m_firemesh = nullptr;




}

ID3D11Device* Hardware::GetDevice() const
{
	return m_pDevice;
}

void Hardware::CycleCullMode()
{
	m_mesh->CycleCullMode();
}

void Hardware::Render() const
{
	if (!m_IsInitialized)
		return;

	ColorRGB clearColor{};
	m_UniformBg ? clearColor = ColorRGB{ 0.1f, 0.1f, 0.1f } : clearColor = ColorRGB{ 0.39f, 0.59f, 0.93f };

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_mesh->Render(m_pDeviceContext, m_FilterMode , m_pNoneRasterizerState,m_pBackRasterizerState,m_pFrontRasterizerState);

	if (m_ToggleFireMesh)
	{
		m_firemesh->Render(m_pDeviceContext);
	}
	m_pSwapChain->Present(0, 0);


}

void dae::Hardware::CycleTextures()
{

	m_FilterMode = (m_FilterMode + 1) % 3;

}

void dae::Hardware::SetMeshs(Mesh* pMesh1, FireMesh* pMesh2)
{
	m_mesh = pMesh1;
	m_firemesh = pMesh2;
}


void Hardware::ToggleFireMesh()
{
	m_ToggleFireMesh = !m_ToggleFireMesh;
	std::cout << (m_ToggleFireMesh ? "Toggle Fire Mesh ON.\n" : "Toggle Fire Mesh OFF.\n");
}

void Hardware::ToggleUniformBg()
{
	m_UniformBg = !m_UniformBg;
	std::cout << (m_UniformBg ? "Uniform background ON.\n" : "Uniform background OFF.\n");
}


HRESULT Hardware::InitializeDirectX()
{
	//1. Create Device & DeviceContext
//=====================================================
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	uint32_t createDeviceFlags = 0;


#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

	if (FAILED(result))
		return result;


	IDXGIFactory1* pDxgiFactory{};
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));

	if (FAILED(result))
		return result;

	IDXGIAdapter* adapter = nullptr;
	IDXGIAdapter* selectedAdapter = nullptr;

	for (UINT i = 0; pDxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc{};
		adapter->GetDesc(&desc);
		std::wcout << L"Adapter" << i << L":" << desc.Description << L"\n";

		if (desc.VendorId != 0x1022)
		{
			selectedAdapter = adapter;
			break;
		}

	}

	if (selectedAdapter == nullptr)
	{
		//error Handling
	}

	result = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

	if (FAILED(result))
		return result;


	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	SDL_SysWMinfo sysWMInfo{};
	SDL_GetVersion(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

	if (FAILED(result))
		return result;

	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;



	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;


	result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return result;


	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
	if (FAILED(result))
		return result;


	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);



	// Rasterizer States.
	// None Culling.
	D3D11_RASTERIZER_DESC descNone{};
	descNone.FillMode = D3D11_FILL_SOLID;
	descNone.CullMode = D3D11_CULL_NONE;
	descNone.FrontCounterClockwise = FALSE;
	descNone.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	descNone.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	descNone.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descNone.DepthClipEnable = TRUE;
	descNone.ScissorEnable = FALSE;
	descNone.MultisampleEnable = FALSE;
	descNone.AntialiasedLineEnable = FALSE;

	result = m_pDevice->CreateRasterizerState(&descNone, &m_pNoneRasterizerState);
	if (FAILED(result))
	{
		return result;
	}

	// Back Culling.
	D3D11_RASTERIZER_DESC descBack{};
	descBack.FillMode = D3D11_FILL_SOLID;
	descBack.CullMode = D3D11_CULL_BACK;
	descBack.FrontCounterClockwise = FALSE;
	descBack.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	descBack.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	descBack.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descBack.DepthClipEnable = TRUE;
	descBack.ScissorEnable = FALSE;
	descBack.MultisampleEnable = FALSE;
	descBack.AntialiasedLineEnable = FALSE;

	result = m_pDevice->CreateRasterizerState(&descBack, &m_pBackRasterizerState);
	if (FAILED(result))
	{
		return result;
	}

	// Front Culling.
	D3D11_RASTERIZER_DESC descFront{};
	descFront.FillMode = D3D11_FILL_SOLID;
	descFront.CullMode = D3D11_CULL_FRONT;
	descFront.FrontCounterClockwise = FALSE;
	descFront.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	descFront.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	descFront.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descFront.DepthClipEnable = TRUE;
	descFront.ScissorEnable = FALSE;
	descFront.MultisampleEnable = FALSE;
	descFront.AntialiasedLineEnable = FALSE;

	result = m_pDevice->CreateRasterizerState(&descFront, &m_pFrontRasterizerState);
	if (FAILED(result))
	{
		return result;
	}

	// Set None culling as default.
	m_pDeviceContext->RSSetState(m_pNoneRasterizerState);

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &viewport);


	if (pDxgiFactory)
	{
		pDxgiFactory->Release();
		pDxgiFactory = nullptr;
	}


	return S_OK;
}