#pragma once
#include "LoadEffects.h"

namespace dae
{
    class FireEffect final : public LoadEffects
    {
    public:
        FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

        ID3DX11EffectTechnique* GetFireTechnique() const { return m_pTechnique; }
        ID3DX11EffectMatrixVariable* GetWVP() const { return m_pWVP; }

        void SetDiffuseMap(ID3D11ShaderResourceView* pSRV);

    private:
        ID3DX11EffectTechnique* m_pTechnique{};
        ID3DX11EffectShaderResourceVariable* m_pDiffuse{};
        ID3DX11EffectMatrixVariable* m_pWVP{};
    };
}
