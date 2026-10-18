#pragma once
#include <d3d11.h>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "ColorRGB.h"
#include "Vector2.h"

namespace dae
{
    class Texture
    {
    public:
        Texture(ID3D11Device* pDevice, const std::string& filePath);
        ~Texture();

        ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }


        //Software

        dae::ColorRGB Sample(const dae::Vector2& uv) const;

    private:
        ID3D11Texture2D* m_pTexture = nullptr;
        ID3D11ShaderResourceView* m_pSRV = nullptr;

        SDL_Surface* m_pSurface{ nullptr };
        uint32_t* m_pSurfacePixels{ nullptr };

    };
}