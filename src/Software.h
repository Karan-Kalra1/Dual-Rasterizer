#pragma once
#include <cstdint>
#include <vector>
#include "Camera.h"
#include "Vertex.h"
#include "Vector4.h"
#include "Utils.h"
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;


namespace dae
{
	class Texture;
	class Mesh;
	class Timer;
	class Scene;

	class Software final
	{
	public:
		Software(SDL_Window* pWindow, int width, int height);
		~Software();

		Software(const Software&) = delete;
		Software(Software&&) noexcept = delete;
		Software& operator=(const Software&) = delete;
		Software& operator=(Software&&) noexcept = delete;

		void Render(const Camera* camera);

		bool SaveBufferToImage() const;

		void VertexTransformationFunction(const Camera* camera);
		void CycleLightingMode();
		void TriangleStripRender();
		void TriangleListRender();
		void ToggleNormalMap() { m_NormalMap = !m_NormalMap; }
		void ToggleDepth() { m_DepthEnabled = !m_DepthEnabled; }
		void ToggleUniformBg();
		void ToggleBoundingBox();
		void SetMesh(Mesh* pMesh);
		ColorRGB PixelShading(const Vertex_Out& v);
		void SetTextures(Texture* pDiffuse, Texture* pNormal, Texture* pGloss, Texture* pSpecular);
		void CycleCullMode();

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		enum class LightingMode
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined

		};

		LightingMode m_LightingMode{ LightingMode::Combined };

		Culling m_CurrentCullingMode{ Culling::None };

		bool m_NormalMap{ false };
		bool m_DepthEnabled{ false };
		bool m_UniformBg{ false };
		bool m_ToggleBoundingBox{ false };
		Camera m_Camera{};
		Texture* m_pTexture{};
		Texture* m_pNormalMap{};
		Texture* m_pGloss{};
		Texture* m_pSpecular{};
		int m_Width{};
		int m_Height{};
		Mesh* m_pVehicleMesh{};
		


		

	};
}