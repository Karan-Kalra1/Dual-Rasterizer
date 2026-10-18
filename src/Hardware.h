#pragma once

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "Camera.h"
#include "Utils.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Mesh.h"
#include "FireMesh.h"
#include "Vector3.h"

// Framework Headers
#include "Timer.h"

namespace dae
{
	class Hardware final
	{
	public:
		Hardware(SDL_Window* pWindow, int width, int height);
		~Hardware();

		Hardware(const Hardware&) = delete;
		Hardware(Hardware&&) noexcept = delete;
		Hardware& operator=(const Hardware&) = delete;
		Hardware& operator=(Hardware&&) noexcept = delete;

		void Render() const;
		void CycleTextures();
		void SetMeshs(Mesh* pMesh1, FireMesh* pMesh2);
		ID3D11Device* GetDevice() const;
		void ToggleFireMesh();
		void ToggleUniformBg();
		void CycleCullMode();


	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		


		bool m_IsInitialized{ false };
		bool m_ToggleFireMesh{ true };
		bool m_UniformBg{ false };

		//DIRECTX
		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		ID3D11RasterizerState* m_pNoneRasterizerState;
		ID3D11RasterizerState* m_pBackRasterizerState;
		ID3D11RasterizerState* m_pFrontRasterizerState;
		

		//...

		std::vector<Vertex_PosCol> m_vertices{	};



		std::vector<uint32_t> m_indices{ };

		std::vector<Vertex_Fire> m_firevertices{};
		std::vector<uint32_t> m_fireindices{};

		Mesh* m_mesh{};
		FireMesh* m_firemesh{};
		bool m_isRotating{ false };
		Camera* m_camera{};
		int m_FilterMode = 0;

		float m_RotationAngle{};

		Matrix m_worldMatrix{};

	};
}
