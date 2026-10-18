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
#include "Hardware.h"
#include "Software.h"

// Framework Headers
#include "Timer.h"

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void CycleFilteringMode() const;
		void VisualizeDepthBuffer() const;
		void CycleShadingMode() const;
		void ToggleNormalMap() const;
		void ToggleBoundingBox() const;
		void SwitchRenderMode();
		void ToggleFireMesh() const;
		void CycleCullMode() const;
		void ToggleRotation();
		void ToggleUniformBg() const;



		

	private:

		Software* m_pSoftware;
		Hardware* m_pHardware;

		Mesh* m_pVehicleMesh;
		FireMesh* m_pFireMesh;
		Camera m_Camera{};
		
		
		bool m_ToggleRenderModeSoftware{ false };

		bool m_ToggleRotation{ true };


		

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


		Texture* m_pDiffuseVehicle{};
		Texture* m_pNormalVehicle{};
		Texture* m_pGlossVehicle{};
		Texture* m_pSpecularVehicle{};
		Texture* m_pDiffuseFire{};


		void Keybindings() const;
		

	};
}
