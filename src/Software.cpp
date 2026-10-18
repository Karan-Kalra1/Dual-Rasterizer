
#define NOMINMAX

#include "Software.h"
//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Software.h"
#include "Math.h"
#include "Texture.h"
#include "Utils.h"
#include <iostream>
#include <ppl.h>
#include <array>

using namespace dae;

Software::Software(SDL_Window* pWindow, int width, int height)
	: m_pWindow(pWindow)
	, m_Width(width)
	, m_Height(height)
{


	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	

	const int noOfPixels{ m_Width * m_Height };

	m_pDepthBufferPixels = new float[noOfPixels];

	


}

Software::~Software()
{
	delete[] m_pDepthBufferPixels;
	
}


void Software::Render(const Camera* camera)
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);


	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);

	UINT8 color;
	m_UniformBg ? color = 25 : color = 100;

	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, color, color, color));


	VertexTransformationFunction(camera);

	


	for (auto& v : m_pVehicleMesh->m_VerticesOut)
	{
		v.position.x = ((v.position.x + 1) * 0.5f) * m_Width;
		v.position.y = ((1 - v.position.y) * 0.5f) * m_Height;



	}

		TriangleListRender();


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
	


	
}


void Software::ToggleUniformBg()
{
	m_UniformBg = !m_UniformBg;
}

void Software::ToggleBoundingBox()
{
	m_ToggleBoundingBox = !m_ToggleBoundingBox;
	std::cout << (m_ToggleBoundingBox ? "Bounding Box ON.\n" : "Bounding Box OFF.\n");
}


bool Software::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

void Software::VertexTransformationFunction(const Camera* camera)
{
	Matrix world = m_pVehicleMesh->m_world;
	Matrix WorldViewProjectionMatrix = world * camera->GetViewMatrix() * camera->GetProjectionMatrix();


	const size_t verticesSize = m_pVehicleMesh->m_VerticesIn.size();

	

	concurrency::parallel_for(static_cast<size_t>(0), verticesSize, [=](const size_t i)
		{


		// Projection.
			Vector4 projectedVertex{ WorldViewProjectionMatrix.TransformPoint(m_pVehicleMesh->m_VerticesIn.at(i).position.x, m_pVehicleMesh->m_VerticesIn.at(i).position.y, m_pVehicleMesh->m_VerticesIn.at(i).position.z, 1) };

			// World space normal calculation.
			const Vector3 worldSpaceNormal{ world.TransformVector(m_pVehicleMesh->m_VerticesIn.at(i).normal).Normalized() };

			// Tangent calculation.
			const Vector3 tangent{ world.TransformVector(m_pVehicleMesh->m_VerticesIn.at(i).tangent).Normalized() };

			// View Direction Calculation.
			const Vector3 viewDirection{ camera->getCameraPos() - world.TransformPoint(m_pVehicleMesh->m_VerticesIn.at(i).position)};

			// Perspective Divide.
			projectedVertex.x /= projectedVertex.w;
			projectedVertex.y /= projectedVertex.w;
			projectedVertex.z /= projectedVertex.w;

			const Vertex_Out temp{ projectedVertex,m_pVehicleMesh->m_VerticesIn.at(i).color, m_pVehicleMesh->m_VerticesIn.at(i).uv, worldSpaceNormal, tangent, viewDirection };


			m_pVehicleMesh->m_VerticesOut.at(i) = temp;

				
	});

}

void dae::Software::TriangleListRender()
{


	concurrency::parallel_for(static_cast<size_t>(0), m_pVehicleMesh->m_Indices.size(), static_cast<size_t>(3), [=](const size_t i)
		{

		 Vertex_Out vertice_0{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i]] };
		 Vertex_Out vertice_1{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i + 1]] };
		 Vertex_Out vertice_2{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i + 2]] };


		


		const Vector2& point_0{ vertice_0.position.GetXY() };
		const Vector2& point_1{ vertice_1.position.GetXY() };
		const Vector2& point_2{ vertice_2.position.GetXY() };

		float areaOfTriangle{ Vector2::Cross(point_1 - point_0, point_2 - point_0) / 2.f};


		auto x_min{ std::min({ static_cast<int>(point_0.x), static_cast<int>(point_1.x), static_cast<int>(point_2.x) }) };
		auto x_max{ std::max({ static_cast<int>(point_0.x), static_cast<int>(point_1.x), static_cast<int>(point_2.x) }) };
		auto y_min{ std::min({ static_cast<int>(point_0.y), static_cast<int>(point_1.y), static_cast<int>(point_2.y) }) };
		auto y_max{ std::max({ static_cast<int>(point_0.y), static_cast<int>(point_1.y), static_cast<int>(point_2.y) }) };

		x_min = std::max(x_min, 0);
		y_min = std::max(y_min, 0);
		x_max = std::min(x_max, m_Width - 1);
		y_max = std::min(y_max, m_Height - 1);

			


		if (x_min <= 0 || x_max >= (m_Width - 1))
			return;

		if (y_min <= 0 || y_max >= (m_Height - 1))
			return;
			

		for (int py{ y_min }; py <= y_max; ++py)
		{
			for (int px{ x_min }; px <= x_max; ++px)
			{

				if (m_ToggleBoundingBox)
				{
					m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(255),
						static_cast<uint8_t>(255),
						static_cast<uint8_t>(255));


				}
				else
				{
					const Vector2 pixel{ px + 0.5f , py + 0.5f };

					ColorRGB finalColor{};



					const Vector2 edge{ point_1 - point_0 };
					const Vector2 edge2{ point_2 - point_1 };
					const Vector2 edge3{ point_0 - point_2 };



					Vector2 p{ pixel - point_0 };


					float e0{ Vector2::Cross(edge , p) };

					float w2 = (e0 / 2.f);


					p = { pixel - point_1 };

					float e1{ Vector2::Cross(edge2 , p) };

					float w0 = (e1 / 2.f);



					p = { pixel - point_2 };

					float e2{ Vector2::Cross(edge3 , p) };

					float w1 = (e2 / 2.f);


					bool isBackCulling{ e1 > 0 && e2 > 0 && e0 > 0 };
					bool isFrontCulling{ e1 < 0 && e2 < 0 && e0 < 0 };


					if ((m_CurrentCullingMode == Culling::Back && isBackCulling) || (m_CurrentCullingMode == Culling::Front
						&& isFrontCulling) || (m_CurrentCullingMode == Culling::None && (isFrontCulling || isBackCulling)))
					{

						w0 = w0 / areaOfTriangle;
						w1 = w1 / areaOfTriangle;
						w2 = w2 / areaOfTriangle;




						const float z_depth_1{ w0 * (1 / vertice_0.position.z) };
						const float z_depth_2{ w1 * (1 / vertice_1.position.z) };
						const float z_depth_3{ w2 * (1 / vertice_2.position.z) };
						const	float z_depth{ 1 / (z_depth_1 + z_depth_2 + z_depth_3) };






						if (z_depth <= 1.f && z_depth >= 0.f)
						{

							if (m_pDepthBufferPixels[px + (py * m_Width)] > z_depth)
							{
								m_pDepthBufferPixels[px + (py * m_Width)] = z_depth;


								const	float w_depth_1{ w0 * (1 / vertice_0.position.w) };
								const float w_depth_2{ w1 * (1 / vertice_1.position.w) };
								const float w_depth_3{ w2 * (1 / vertice_2.position.w) };
								const float w_depth{ 1 / (w_depth_1 + w_depth_2 + w_depth_3) };


								const Vector2 uv_1{ (vertice_0.uv / vertice_0.position.w) * w0 };
								const Vector2 uv_2{ (vertice_1.uv / vertice_1.position.w) * w1 };
								const Vector2 uv_3{ (vertice_2.uv / vertice_2.position.w) * w2 };


								const Vector2 interpolatedUV = (uv_1 + uv_2 + uv_3) * w_depth;


								const Vector3 normal_1{ (vertice_0.normal / vertice_0.position.w) * w0 };
								const Vector3 normal_2{ (vertice_1.normal / vertice_1.position.w) * w1 };
								const Vector3 normal_3{ (vertice_2.normal / vertice_2.position.w) * w2 };


								const Vector3 interpolatedNormal = (normal_1 + normal_2 + normal_3) * w_depth;

								const Vector3 tangent_1{ (vertice_0.tangent / vertice_0.position.w) * w0 };
								const Vector3 tangent_2{ (vertice_1.tangent / vertice_1.position.w) * w1 };
								const Vector3 tangent_3{ (vertice_2.tangent / vertice_2.position.w) * w2 };


								const Vector3 interpolatedTangent = (tangent_1 + tangent_2 + tangent_3) * w_depth;


								const Vector3 viewDirection_1{ (vertice_0.viewDirection / vertice_0.position.w) * w0 };
								const Vector3 viewDirection_2{ (vertice_1.viewDirection / vertice_1.position.w) * w1 };
								const Vector3 viewDirection_3{ (vertice_2.viewDirection / vertice_2.position.w) * w2 };


								const Vector3 interpolatedviewDirection = (viewDirection_1 + viewDirection_2 + viewDirection_3) * w_depth;

								Vertex_Out pixel{};
								pixel.position = { p.x, p.y, z_depth , w_depth };
								pixel.uv = interpolatedUV;
								pixel.normal = interpolatedNormal;
								pixel.tangent = interpolatedTangent;
								pixel.viewDirection = interpolatedviewDirection;

								if (m_DepthEnabled)
								{
									float depth{ Clamp(z_depth, 0.985f, 1.f) };


									finalColor = ColorRGB(depth, depth, depth);
								}

								else {

									finalColor = PixelShading(pixel);
								}



								finalColor.MaxToOne();

								m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
									static_cast<uint8_t>(finalColor.r * 255),
									static_cast<uint8_t>(finalColor.g * 255),
									static_cast<uint8_t>(finalColor.b * 255));


							}
						}

					}

				}
				

			}

		}



	}

	);
}


void Software::CycleCullMode()
{
	int count{ static_cast<int>(m_CurrentCullingMode) };
	count++;
	if (count > 2)
	{
		count = 0;
	}
	const auto castEnum = static_cast<Culling>(count);
	m_CurrentCullingMode = castEnum;

	switch (m_CurrentCullingMode)
	{
	case Culling::None:
		m_CurrentCullingMode = Culling::None;
		break;
	case Culling::Back:
		m_CurrentCullingMode = Culling::Back;
		break;
	case Culling::Front:
		m_CurrentCullingMode = Culling::Front;
		break;
	}
}



void dae::Software::SetMesh(Mesh* pMesh)
{
	m_pVehicleMesh = pMesh;

	for (int i = 0; i < m_pVehicleMesh->m_VerticesIn.size(); ++i)
	{
		m_pVehicleMesh->m_VerticesOut.push_back(Vertex_Out{});
	}
}

ColorRGB dae::Software::PixelShading(const Vertex_Out& v)
{
	const Vector3 lightDirection{ 0.577f,-0.577f, 0.577f };

	const float shininess{ 25.f };

	const float kd{ 7.f };

	const ColorRGB ambient{ 0.25f,0.25f,0.25f };


	const Vector3 normal = v.normal.Normalized();
	const Vector3 tangent = v.tangent.Normalized();
	const Vector3 viewDirection = v.viewDirection.Normalized();

	Vector3 sampleNormalWorld{};

	if (!m_NormalMap)
	{
		const Vector3 binormal{ Vector3::Cross(normal,tangent) };

		Matrix MatrixtangentSpaceAxis{ tangent,binormal,normal,{0,0,0} };

		ColorRGB normalColor{ m_pNormalMap->Sample(v.uv) };

		Vector3 sampleNormal
		{
			(normalColor.r * 2.f) - 1.f,
			(normalColor.g * 2.f) - 1.f,
			(normalColor.b * 2.f) - 1.f
		};

		sampleNormalWorld = MatrixtangentSpaceAxis.TransformVector(sampleNormal);
	}

	else
	{
		sampleNormalWorld = normal;
	}

	switch (m_LightingMode)
	{
	case LightingMode::ObservedArea: {

		const float cosine = std::max(Vector3::Dot(sampleNormalWorld, -lightDirection), 0.f);

		ColorRGB observedArea{ cosine,cosine,cosine };

		return observedArea;

	}
	case LightingMode::Diffuse: {

		const ColorRGB lambertDiffuse{ (m_pTexture->Sample(v.uv) * kd) / PI };

		return lambertDiffuse;
	}
	case LightingMode::Specular: {

		const ColorRGB specularColor{ m_pSpecular->Sample(v.uv) };

		float phong_exponent{ m_pGloss->Sample(v.uv).r };

		phong_exponent *= shininess;

		const Vector3 reflect{ lightDirection - 2 * (Vector3::Dot(lightDirection,sampleNormalWorld)) * sampleNormalWorld };

		const float phong_cosine{ std::max(Vector3::Dot(reflect,viewDirection),0.f) };

		ColorRGB Phong{ specularColor * pow(phong_cosine , phong_exponent) };

		return Phong;
	}
	case LightingMode::Combined: {

		const ColorRGB lambertDiffuse{ (m_pTexture->Sample(v.uv) * kd) / PI };

		const float cosine = std::max(Vector3::Dot(sampleNormalWorld, -lightDirection), 0.f);

		ColorRGB observedArea{ cosine,cosine,cosine };

		const ColorRGB specularColor{ m_pSpecular->Sample(v.uv) };

		float phong_exponent{ m_pGloss->Sample(v.uv).r };

		phong_exponent *= shininess;

		const Vector3 reflect{ lightDirection - 2 * (Vector3::Dot(lightDirection,sampleNormalWorld)) * sampleNormalWorld };

		const float phong_cosine{ std::max(Vector3::Dot(reflect,viewDirection),0.f) };

		ColorRGB Phong{ specularColor * pow(phong_cosine , phong_exponent) };

		return observedArea * (Phong + lambertDiffuse + ambient);
	}
	}
	return {};
}

void Software::SetTextures(Texture* pDiffuse, Texture* pNormal, Texture* pGloss, Texture* pSpecular)
{
	m_pTexture = pDiffuse;
	m_pNormalMap = pNormal;
	m_pGloss = pGloss;
	m_pSpecular = pSpecular;
}


void Software::TriangleStripRender()
{

	for (auto i = 0; i < m_pVehicleMesh->m_Indices.size() - 2; i++)
	{
		if (!(i % 2 == 0))
		{
			std::swap(m_pVehicleMesh->m_Indices[i + 1], m_pVehicleMesh->m_Indices[i + 2]);
		}


		const Vertex_Out vertice_0{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i]] };
		const Vertex_Out vertice_1{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i + 1]] };
		const Vertex_Out vertice_2{ m_pVehicleMesh->m_VerticesOut[m_pVehicleMesh->m_Indices[i + 2]] };


		if (!(i % 2 == 0))
		{
			std::swap(m_pVehicleMesh->m_Indices[i + 1], m_pVehicleMesh->m_Indices[i + 2]);
		}

		const Vector2& point_0{ vertice_0.position.GetXY() };
		const Vector2& point_1{ vertice_1.position.GetXY() };
		const Vector2& point_2{ vertice_2.position.GetXY() };

		float areaOfTriangle{ Vector2::Cross(point_1 - point_0, point_2 - point_0) / 2.f };


		auto x_min{ std::min({ static_cast<int>(point_0.x), static_cast<int>(point_1.x), static_cast<int>(point_2.x) }) };
		auto x_max{ std::max({ static_cast<int>(point_0.x), static_cast<int>(point_1.x), static_cast<int>(point_2.x) }) };
		auto y_min{ std::min({ static_cast<int>(point_0.y), static_cast<int>(point_1.y), static_cast<int>(point_2.y) }) };
		auto y_max{ std::max({ static_cast<int>(point_0.y), static_cast<int>(point_1.y), static_cast<int>(point_2.y) }) };

		x_min = std::max(x_min, 0);
		y_min = std::max(y_min, 0);
		x_max = std::min(x_max, m_Width - 1);
		y_max = std::min(y_max, m_Height - 1);





		if (x_min <= 0 || x_max >= (m_Width - 1))
			continue;

		if (y_min <= 0 || y_max >= (m_Height - 1))
			continue;


		for (int py{ y_min }; py <= y_max; ++py)
		{
			for (int px{ x_min }; px <= x_max; ++px)
			{



				const Vector2 pixel{ px + 0.5f , py + 0.5f };

				ColorRGB finalColor{};



				const Vector2 edge{ point_1 - point_0 };
				const Vector2 edge2{ point_2 - point_1 };
				const Vector2 edge3{ point_0 - point_2 };



				Vector2 p{ pixel - point_0 };


				float e0{ Vector2::Cross(edge , p) };
				if (e0 < 0)
					continue;
				float w2 = (e0 / 2.f);


				p = { pixel - point_1 };

				float e1{ Vector2::Cross(edge2 , p) };
				if (e1 < 0)
					continue;
				float w0 = (e1 / 2.f);



				p = { pixel - point_2 };

				float e2{ Vector2::Cross(edge3 , p) };
				if (e2 < 0)
					continue;
				float w1 = (e2 / 2.f);




				w0 = w0 / areaOfTriangle;
				w1 = w1 / areaOfTriangle;
				w2 = w2 / areaOfTriangle;



				const float z_depth_1{ w0 * (1 / vertice_0.position.z) };
				const float z_depth_2{ w1 * (1 / vertice_1.position.z) };
				const float z_depth_3{ w2 * (1 / vertice_2.position.z) };
				const	float z_depth{ 1 / (z_depth_1 + z_depth_2 + z_depth_3) };


				if (z_depth <= 1.f && z_depth >= 0.f)
				{

					if (m_pDepthBufferPixels[px + (py * m_Width)] > z_depth)
					{
						m_pDepthBufferPixels[px + (py * m_Width)] = z_depth;


						const	float w_depth_1{ w0 * (1 / vertice_0.position.w) };
						const float w_depth_2{ w1 * (1 / vertice_1.position.w) };
						const float w_depth_3{ w2 * (1 / vertice_2.position.w) };
						const float w_depth{ 1 / (w_depth_1 + w_depth_2 + w_depth_3) };


						const Vector2 uv_1{ (vertice_0.uv / vertice_0.position.w) * w0 };
						const Vector2 uv_2{ (vertice_1.uv / vertice_1.position.w) * w1 };
						const Vector2 uv_3{ (vertice_2.uv / vertice_2.position.w) * w2 };


						const Vector2 interpolatedUV = (uv_1 + uv_2 + uv_3) * w_depth;


						const Vector3 normal_1{ (vertice_0.normal / vertice_0.position.w) * w0 };
						const Vector3 normal_2{ (vertice_1.normal / vertice_1.position.w) * w1 };
						const Vector3 normal_3{ (vertice_2.normal / vertice_2.position.w) * w2 };


						const Vector3 interpolatedNormal = (normal_1 + normal_2 + normal_3) * w_depth;

						const Vector3 tangent_1{ (vertice_0.tangent / vertice_0.position.w) * w0 };
						const Vector3 tangent_2{ (vertice_1.tangent / vertice_1.position.w) * w1 };
						const Vector3 tangent_3{ (vertice_2.tangent / vertice_2.position.w) * w2 };


						const Vector3 interpolatedTangent = (tangent_1 + tangent_2 + tangent_3) * w_depth;


						const Vector3 viewDirection_1{ (vertice_0.viewDirection / vertice_0.position.w) * w0 };
						const Vector3 viewDirection_2{ (vertice_1.viewDirection / vertice_1.position.w) * w1 };
						const Vector3 viewDirection_3{ (vertice_2.viewDirection / vertice_2.position.w) * w2 };


						const Vector3 interpolatedviewDirection = (viewDirection_1 + viewDirection_2 + viewDirection_3) * w_depth;

						Vertex_Out pixel{};
						pixel.position = { p.x, p.y, z_depth , w_depth };
						pixel.uv = interpolatedUV;
						pixel.normal = interpolatedNormal;
						pixel.tangent = interpolatedTangent;
						pixel.viewDirection = interpolatedviewDirection;

						if (m_DepthEnabled)
						{
							float depth{ Clamp(z_depth, 0.985f, 1.f) };


							finalColor = ColorRGB(depth, depth, depth);
						}

						else {

							finalColor = PixelShading(pixel);
						}

						finalColor.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));


					}
				}




			}



		}



	}


}

void Software::CycleLightingMode()
{
	switch (m_LightingMode)
	{
	case LightingMode::ObservedArea:
		m_LightingMode = LightingMode::Diffuse;

		break;

	case LightingMode::Diffuse:

		m_LightingMode = LightingMode::Specular;
		break;

	case LightingMode::Specular:

		m_LightingMode = LightingMode::Combined;
		break;

	case LightingMode::Combined:

		m_LightingMode = LightingMode::ObservedArea;
		break;
	}

}