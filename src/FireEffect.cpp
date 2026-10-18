#include "FireEffect.h"
#include <iostream>

using namespace dae;

FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
    : LoadEffects(pDevice, assetFile,false)
{
    if (!m_pEffect)
    {
        std::wcout << L"FireEffect failed to load!\n";
        return;
    }

    m_pTechnique = m_pEffect->GetTechniqueByName("FireTech");
    if (!m_pTechnique->IsValid())
        std::wcout << L"FireTech NOT VALID\n";

    m_pDiffuse =
        m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

    m_pWVP =
        m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void FireEffect::SetDiffuseMap(ID3D11ShaderResourceView* pSRV)
{
    if (m_pDiffuse)
        m_pDiffuse->SetResource(pSRV);
}

