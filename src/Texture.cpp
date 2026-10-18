#include "Texture.h"


namespace dae
{
    Texture::Texture(ID3D11Device* pDevice, const std::string& filePath)
    {
        SDL_Surface* pSurface = IMG_Load(filePath.c_str());

        m_pSurface = pSurface;
        m_pSurfacePixels = static_cast<uint32_t*>(pSurface->pixels);

        if (!pSurface)
        {
            printf("IMG_Load failed: %s\n", IMG_GetError());
            return;
        }

        // ---- Ensure 32-bit RGBA Format ----
        SDL_Surface* pConverted =
            SDL_ConvertSurfaceFormat(pSurface, SDL_PIXELFORMAT_ABGR8888, 0);

        pSurface = pConverted;

        if (!pSurface)
        {
            printf("Surface conversion failed!\n");
            return;
        }

        // ---- Describe Texture2D ----
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = pSurface->w;
        desc.Height = pSurface->h;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        // ---- Fill Texture Data ----
        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = pSurface->pixels;
        initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
        initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

        HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);

        if (FAILED(hr))
        {
            printf("CreateTexture2D FAILED!\n");
            return;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
        SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = 1;

        // ---- Create Shader Resource View ----
        hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV);
        if (FAILED(hr))
        {
            printf("CreateShaderResourceView FAILED!\n");
            m_pTexture->Release();
            m_pTexture = nullptr;
            SDL_FreeSurface(pSurface);
            return;
        }

       
    }

    Texture::~Texture()
    {
        if (m_pSRV)
        {
            m_pSRV->Release();
            m_pSRV = nullptr;
        }

        if (m_pTexture)
        {
            m_pTexture->Release();
            m_pTexture = nullptr;
        }

        if (m_pSurface)
        {
            SDL_FreeSurface(m_pSurface);
            m_pSurface = nullptr;
        }
    }

    dae::ColorRGB Texture::Sample(const dae::Vector2& uv) const
    {
        float u{ uv.x * m_pSurface->w };
        float v{ uv.y * m_pSurface->h };

        Uint8 r{};
        Uint8 g{};
        Uint8 b{};

        SDL_GetRGB(m_pSurfacePixels[(int)u + ((int)v * m_pSurface->w)], m_pSurface->format, &r, &g, &b);

        dae::ColorRGB color{ (float)r / 255.f,(float)g / 255.f,(float)b / 255.f };

        return color;

    }
}