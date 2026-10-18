
#pragma once
#include <vector>
#include <d3d11.h>
#include "Camera.h"
#include "Texture.h"
#include "Vector3.h"
#include "FireEffect.h"
#include "Vertex.h"




namespace dae {

    class FireMesh final
    {
    public:
        FireMesh(ID3D11Device* pDevice,
            const std::vector<Vertex_Fire>& vertices,
            const std::vector<uint32_t>& indices);

        ~FireMesh();

        void Update(Camera* camera, const Timer* pTimer, Matrix& world);
        void Render(ID3D11DeviceContext* pContext);

    private:
        ID3D11Buffer* m_pVertexBuffer{};
        ID3D11Buffer* m_pIndexBuffer{};
        ID3D11InputLayout* m_pInputLayout{};
        uint32_t m_NumIndices{};

        FireEffect* m_pFireEffect{};
        Texture* m_pFireTexture{};

        float m_RotationAngle{};

    };

}