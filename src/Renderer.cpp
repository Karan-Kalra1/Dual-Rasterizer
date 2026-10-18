//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Texture.h"


// Standard includes
#include <iostream>

//Project includes
#include "Renderer.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) 
	
{
	//Initialize
	int width{}, height{};
	SDL_GetWindowSize(pWindow, &width, &height);

	m_pSoftware = new Software{ pWindow, width, height };
	m_pHardware = new Hardware{ pWindow, width, height };

	Keybindings();

	const auto device = m_pHardware->GetDevice();

	

	Utils::ParseOBJ("Resources//vehicle.obj",m_vertices, m_indices,true);

	Utils::ParseOBJFire("Resources//fireFX.obj", m_firevertices, m_fireindices);

	
	m_pVehicleMesh = new Mesh(device, m_vertices, m_indices);
	m_pFireMesh = new FireMesh(device, m_firevertices, m_fireindices);
	m_camera = new Camera();

	m_pSoftware->SetMesh(m_pVehicleMesh);
	m_pHardware->SetMeshs(m_pVehicleMesh, m_pFireMesh);

	m_pDiffuseVehicle = new Texture( device ,"Resources/vehicle_diffuse.png"  );
	m_pNormalVehicle = new Texture(device ,"Resources/vehicle_normal.png"  );
	m_pGlossVehicle = new Texture( device ,"Resources/vehicle_gloss.png"  );
	m_pSpecularVehicle = new Texture( device ,"Resources/vehicle_specular.png" );

	m_pSoftware->SetTextures(m_pDiffuseVehicle, m_pNormalVehicle, m_pGlossVehicle, m_pSpecularVehicle);

	m_camera->Initialize(45.f, Vector3{ 0.f,0.f, 0.f }, width, height);

}

Renderer::~Renderer()
{

	delete m_pVehicleMesh;
	m_pVehicleMesh = nullptr;

	delete m_pFireMesh;
	m_pFireMesh = nullptr;

	delete m_pDiffuseVehicle;
	m_pDiffuseVehicle = nullptr;

	delete m_pNormalVehicle;
	m_pNormalVehicle = nullptr;

	delete m_pGlossVehicle;
	m_pGlossVehicle = nullptr;

	delete m_pSpecularVehicle;
	m_pSpecularVehicle = nullptr;


	delete m_camera;
	m_camera = nullptr;
	
	delete m_mesh;
	m_mesh = nullptr;

	delete m_firemesh;
	m_firemesh = nullptr;
	
	delete m_pSoftware;
	m_pSoftware = nullptr;

	delete m_pHardware;
	m_pHardware = nullptr;
	

}

void Renderer::Update(const Timer* pTimer)
{

	if (m_isRotating)
	{
		m_RotationAngle += pTimer->GetElapsed() * 45.0f * TO_RADIANS;
	}

	Vector3 pos{ 0,0, 50 };
	Vector3 rot{ 0,m_RotationAngle,0 };
	Vector3 scale{ 1,1,1 };

	

	m_worldMatrix =
		Matrix::CreateScale(scale) *
		Matrix::CreateRotation(rot) *
		Matrix::CreateTranslation(pos);


	m_camera->Update(pTimer);
	m_pVehicleMesh->Update(m_camera, pTimer, m_worldMatrix);
	m_pFireMesh->Update(m_camera,  pTimer, m_worldMatrix);
	

	
}


void Renderer::Render() const
{
	if (m_ToggleRenderModeSoftware)
	{
		m_pSoftware->Render(m_camera);
	}
	else
	{
		m_pHardware->Render();
	}


}

void Renderer::SwitchRenderMode()
{
	m_ToggleRenderModeSoftware = !m_ToggleRenderModeSoftware;
	std::cout << (m_ToggleRenderModeSoftware ? "Render Mode: Software.\n" : "Render Mode: Hardware.\n");
}

void Renderer::Keybindings() const
{
	std::cout << "[Key Bindings - SHARED]\n";
	std::cout << "[F1] Switch Render Mode (Hardware / Software).\n";
	std::cout << "[F2] Toggle Vehicle Rotation (ON / OFF).\n";
	std::cout << "[F9] Cycle CullMode (Back / None / Front).\n";
	std::cout << "[F10] Toggle Uniform ClearColor (ON / OFF).\n";
	std::cout << "[F11] Toggle Print FPS (ON / OFF).\n";
	std::cout << "\n";

	std::cout << "[Key Bindings - HARDWARE]\n";
	std::cout << "[F3] Toggle Fire Mesh (ON / OFF).\n";
	std::cout << "[F4] Cycle Filtering Mode (Point / Linear / Anisotropic).\n";
	std::cout << "\n";

	std::cout << "[Key Bindings - SOFTWARE]\n";
	std::cout << "[F5] Switch Shading Mode (Combined / Observed Area / Diffuse / Specular).\n";
	std::cout << "[F6] Toggle NormalMap (ON / OFF).\n";
	std::cout << "[F7] Toggle Depth Buffer Visualization (ON / OFF).\n";
	std::cout << "[F8] Toggle Bounding Box Visualization (ON / OFF).\n";
	std::cout << "\n";

	std::cout << "[---Extras---].\n";
	std::cout << "Implemented MultiThreading.\n";


}




void Renderer::CycleFilteringMode() const
{
	if (m_ToggleRenderModeSoftware)
	{
		std::cout << "Filtering not Supported in Software mode :(\n";
	}
	else
	{
		m_pHardware->CycleTextures();
	}
}

void Renderer::VisualizeDepthBuffer() const
{
	if (m_ToggleRenderModeSoftware)
	{
		m_pSoftware->ToggleDepth();
	}
	else
	{
		std::cout << "Depth Buffer Visualization not Supported in Hardware mode :(\n";
	}
}

void Renderer::CycleShadingMode() const
{
	if (m_ToggleRenderModeSoftware)
	{
		m_pSoftware->CycleLightingMode();
	}
	else
	{
		std::cout << "Cycle Shading Modes not Supported in Hardware mode :(\n";
	}
}

void Renderer::ToggleNormalMap() const
{
	if (m_ToggleRenderModeSoftware)
	{
		m_pSoftware->ToggleNormalMap();
	}
	else
	{
		std::cout << "Toggle Normal Map not Supported in Hardware mode :(\n";
	}
}

void Renderer::ToggleBoundingBox() const
{
	if (m_ToggleRenderModeSoftware)
	{
		m_pSoftware->ToggleBoundingBox();
	}
	else
	{
		std::cout << "Bounding Box Visualization not Supported in Hardware mode :(\n";
	}
}


void Renderer::ToggleFireMesh() const
{
	if (m_ToggleRenderModeSoftware)
	{
		std::cout << "Toggle Fire Mesh not Supported in Software mode :(\n";
	}
	else
	{
		m_pHardware->ToggleFireMesh();
	}
}

void Renderer::CycleCullMode() const
{
	m_pHardware->CycleCullMode();
	m_pSoftware->CycleCullMode();
}

void Renderer::ToggleRotation()
{
	m_isRotating = !m_isRotating;
	std::cout << (m_isRotating ? "Rotation ON.\n" : "Rotation OFF.\n");
}

void Renderer::ToggleUniformBg() const
{
	m_pHardware->ToggleUniformBg();
	m_pSoftware->ToggleUniformBg();
}




