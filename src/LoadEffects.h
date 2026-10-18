
#pragma once
// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <sstream>
#include<iostream>
#include "Texture.h"

#include <string>

namespace dae {
	class LoadEffects 
	{
	public:
		LoadEffects(ID3D11Device* pDevice, const std::wstring& assetFile, bool isbase = true)
		{
			HRESULT result;
			ID3D10Blob* pErrorBlob{ nullptr };



			DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
			shaderFlags |= D3DCOMPILE_DEBUG;
			shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			result = D3DX11CompileEffectFromFile(assetFile.c_str(),
				nullptr,
				nullptr,
				shaderFlags,
				0,
				pDevice,
				&m_pEffect,
				&pErrorBlob);

			if (FAILED(result))
			{
				if (pErrorBlob != nullptr)
				{
					const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

					std::wstringstream ss;
					for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
						ss << pErrors[i];

					OutputDebugStringW(ss.str().c_str());
					pErrorBlob->Release();
					pErrorBlob = nullptr;

					std::wcout << ss.str() << std::endl;


				}
				else
				{
					std::wstringstream ss;
					ss << "EffectLoader: Failed to CreateEffectFromFile! \nPath: " << assetFile;
					std::wcout << ss.str() << std::endl;

				}
			}



			


			if (isbase)
			{
				m_pTechPoint = m_pEffect->GetTechniqueByName("PointTechnique");
				m_pTechLinear = m_pEffect->GetTechniqueByName("LinearTechnique");
				m_pTechAniso = m_pEffect->GetTechniqueByName("AnisoTechnique");



				if (!m_pTechPoint->IsValid())  std::wcout << L"PointTechnique invalid\n";
				if (!m_pTechLinear->IsValid()) std::wcout << L"LinearTechnique invalid\n";
				if (!m_pTechAniso->IsValid())  std::wcout << L"AnisoTechnique invalid\n";


				m_pNormalVar = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();



				m_pSpecularVar = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();

				m_pGlossVar = m_pEffect->GetVariableByName("gGlossMap")->AsShaderResource();


				m_pCameraVar = m_pEffect->GetVariableByName("gCameraPos")->AsVector();


				m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();



				m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();


				m_pWorldMatrixVar = m_pEffect->GetVariableByName("gWorld")->AsMatrix();


				if (!m_pCameraVar->IsValid())
					std::wcout << L"m_pCameraVar not valid! \n";

				if (!m_pNormalVar->IsValid())
					std::wcout << L"m_pNormalVar not valid! \n";

				if (!m_pSpecularVar->IsValid())
					std::wcout << L"m_pSpecularVar not valid! \n";

				if (!m_pGlossVar->IsValid())
					std::wcout << L"m_pGlossVar not valid! \n";

				if (!m_pMatWorldViewProjVariable->IsValid())
					std::wcout << L"m_pMatWorldViewProjVariable not valid! \n";

				if (!m_pDiffuseMapVariable->IsValid())
					std::wcout << L"m_pDiffuseMapVariable not valid! \n";

				if (!m_pWorldMatrixVar->IsValid())
					std::wcout << L"m_pWorldMatrixVar not valid! \n";



			}


			
		}


		~LoadEffects()
		{
			if (m_pTechPoint)
			{
				m_pTechPoint->Release();
			}



			if (m_pTechLinear)
			{
				m_pTechLinear->Release();
			}

			if (m_pTechAniso)
			{
				m_pTechAniso->Release();
			}

			if (m_pMatWorldViewProjVariable)
			{
				m_pMatWorldViewProjVariable->Release();
			}
			if (m_pWorldMatrixVar)
			{
				m_pWorldMatrixVar->Release();
			}
			if (m_pCameraVar)
			{
				m_pCameraVar->Release();
			}
		
			if (m_pEffect)
			{
				m_pEffect->Release();
			}

		}


		void SetTextureMap(Texture* pDiffuseTexture, Texture* pNormaMap, Texture* pSpecularTexture, Texture* pGloss)
		{
			if (m_pDiffuseMapVariable)
				m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());

			if (m_pNormalVar)
				m_pNormalVar->SetResource(pNormaMap->GetSRV());

			if (m_pSpecularVar)
				m_pSpecularVar->SetResource(pSpecularTexture->GetSRV());

			if (m_pGlossVar)
				m_pGlossVar->SetResource(pGloss->GetSRV());


		}

		ID3DX11Effect* GetEffect() const { return m_pEffect; }
		ID3DX11EffectTechnique* GetPointTechnique()  const { return m_pTechPoint; }
		ID3DX11EffectTechnique* GetLinearTechnique() const { return m_pTechLinear; }
		ID3DX11EffectTechnique* GetAnisoTechnique()  const { return m_pTechAniso; }
		ID3DX11EffectMatrixVariable* GetWorldViewMatrixVariable() { return m_pMatWorldViewProjVariable; }
		ID3DX11EffectMatrixVariable* GetWorldMatrixVariable() { return m_pWorldMatrixVar; }
		ID3DX11EffectVectorVariable* GetCameraVariable() { return m_pCameraVar; }


		protected : 
			ID3DX11Effect* m_pEffect{};

	private:

		
		ID3DX11EffectTechnique* m_pTechPoint{};
		ID3DX11EffectTechnique* m_pTechLinear{};
		ID3DX11EffectTechnique* m_pTechAniso{};
		


		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
		ID3DX11EffectMatrixVariable* m_pWorldMatrixVar{};
		ID3DX11EffectVectorVariable* m_pCameraVar{};
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalVar{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularVar{};
		ID3DX11EffectShaderResourceVariable* m_pGlossVar{};

	};
}