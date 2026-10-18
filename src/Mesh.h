#pragma once
#include "Utils.h"
#include <vector>
#include <d3d11.h>
#include "LoadEffects.h"
#include "Camera.h"
#include "Texture.h"
#include "Vector3.h"
#include "Vertex.h"





    

    namespace dae {

    class Mesh final
    {

        

    public:
        Mesh(ID3D11Device* pDevice,
            const std::vector<Vertex_PosCol>& vertices,
            const std::vector<uint32_t>& indices);

        ~Mesh();

        void Update(Camera* camera, const Timer* pTimer, Matrix& world);

        void Render(ID3D11DeviceContext* pContext, int FilterMode, ID3D11RasterizerState* m_pNoneRasterizerState,
            ID3D11RasterizerState* m_pBackRasterizerState,
            ID3D11RasterizerState* m_pFrontRasterizerState);

        void CycleCullMode();

        // Software.
        enum class PrimitiveTopology
        {
            TriangleList,
            TriangleStrip
        };

        std::vector<Vertex_PosCol> m_VerticesIn{};
        std::vector<Vertex_Out> m_VerticesOut{};
        std::vector<uint32_t> m_Indices{};
        PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleList };
        Matrix m_world{};



    private:
        
        dae::Culling m_CurrentCullingMode{}; 

        LoadEffects* m_pLoadEffect{};
        ID3DX11Effect* m_pEffect{};
        ID3DX11EffectTechnique* m_pTechnique{};
        ID3D11Buffer* m_pVertexBuffer{};
        ID3D11Buffer* m_pIndexBuffer{};
        UINT m_NumIndices{};
        ID3D11InputLayout* m_pInputLayout{};
        ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVar{};
        ID3DX11EffectMatrixVariable* m_pWorldMatrixVar{};
        ID3DX11EffectVectorVariable* m_pCameraVar{};
        Texture* m_pDiffuseTexture{};
        Texture* m_pGlossTexture{};
        Texture* m_pSpecularTexture{};
        Texture* m_pNormalMap{};
      

    };
}