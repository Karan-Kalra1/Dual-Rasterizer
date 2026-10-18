#include "Mesh.h"
#include <d3dx11effect.h>
#include <cassert>
#include "Matrix.h"

dae::Mesh::Mesh(ID3D11Device* pDevice,
    const std::vector<Vertex_PosCol>& vertices,
    const std::vector<uint32_t>& indices):m_VerticesIn(vertices), m_Indices(indices)
{
    m_pLoadEffect = new LoadEffects(pDevice, L"Resources/PosCol3D.fx");

  
    m_pTechnique = m_pLoadEffect->GetPointTechnique();
    m_pEffect = m_pLoadEffect->GetEffect();
    m_pMatWorldViewProjVar = m_pLoadEffect->GetWorldViewMatrixVariable();
    m_pWorldMatrixVar = m_pLoadEffect->GetWorldMatrixVariable();
    m_pCameraVar = m_pLoadEffect->GetCameraVariable();
    m_pDiffuseTexture = new Texture(pDevice, "Resources/vehicle_diffuse.png");
    m_pGlossTexture = new Texture(pDevice, "Resources/vehicle_gloss.png");
    m_pSpecularTexture = new Texture(pDevice, "Resources/vehicle_specular.png");
    m_pNormalMap = new Texture(pDevice, "Resources/vehicle_normal.png");
 
    m_pLoadEffect->SetTextureMap(m_pDiffuseTexture, m_pNormalMap, m_pSpecularTexture, m_pGlossTexture);

//Create Vertex Layout 

    static constexpr uint32_t numElements{ 5 };
    D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

    vertexDesc[0].SemanticName = "POSITION";
    vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexDesc[0].AlignedByteOffset = 0;
    vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    vertexDesc[1].SemanticName = "TEXCOORD";
    vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    vertexDesc[1].AlignedByteOffset = 12;
    vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    vertexDesc[2].SemanticName = "NORMAL";
    vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexDesc[2].AlignedByteOffset = 20;
    vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    vertexDesc[3].SemanticName = "TANGENT";
    vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexDesc[3].AlignedByteOffset = 32;
    vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


    vertexDesc[4].SemanticName = "COLOR";
    vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexDesc[4].AlignedByteOffset = 44;
    vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


    //Create Input Layout
    D3DX11_PASS_DESC passDesc{};
    m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);


     HRESULT result = pDevice->CreateInputLayout(
        vertexDesc,
        numElements,
        passDesc.pIAInputSignature,
        passDesc.IAInputSignatureSize,
        &m_pInputLayout);

    if (FAILED(result))
        assert(false);

    //Create Vertex Buffer

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;


    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

     result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

    if (FAILED(result))
        assert(false);


    //Create Input Buffer

    m_NumIndices = static_cast<uint32_t>(indices.size());
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    initData.pSysMem = indices.data();
    result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

    if (FAILED(result))
        assert(false);

   
}

dae::Mesh::~Mesh()
{
    if (m_pIndexBuffer)
    {
        m_pIndexBuffer->Release();
        m_pIndexBuffer = nullptr;
    }


    if (m_pVertexBuffer)
    {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = nullptr;
    }
    
    if (m_pInputLayout)
    {
        m_pInputLayout->Release();
        m_pInputLayout = nullptr;
    }
   

    
    delete m_pLoadEffect;
 
    delete m_pDiffuseTexture;
    delete m_pNormalMap;
    delete m_pGlossTexture;
    delete m_pSpecularTexture;

    m_pLoadEffect = nullptr;
    m_pDiffuseTexture = nullptr;
    m_pNormalMap = nullptr;
    m_pGlossTexture = nullptr;
    m_pSpecularTexture = nullptr;
  
}

void dae::Mesh::Update(Camera* camera, const Timer* pTimer, Matrix& world)
{
    m_world = world;

    Matrix wvp{ m_world * camera->GetViewMatrix() * camera->GetProjectionMatrix() };

    m_pMatWorldViewProjVar->SetMatrix(reinterpret_cast<float*>(&wvp));
    m_pWorldMatrixVar->SetMatrix((float*)&m_world);

    Vector3 camPos = camera->getCameraPos();
    float camData[4] = { camPos.x, camPos.y, camPos.z, 1.f };
    m_pCameraVar->SetFloatVector(camData);

    

}

void dae::Mesh::Render(ID3D11DeviceContext* pContext, int FilterMode, ID3D11RasterizerState* m_pNoneRasterizerState, ID3D11RasterizerState* m_pBackRasterizerState, ID3D11RasterizerState* m_pFrontRasterizerState)
{
    //Set Topology

    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Set Input Layout

    pContext->IASetInputLayout(m_pInputLayout);


    // Set VertexBuffer
    UINT stride = sizeof(Vertex_PosCol);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);


    // Set IndexBuffer
    pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


    switch (FilterMode)
    {
    case 0: m_pTechnique = m_pLoadEffect->GetPointTechnique();  break;
    case 1: m_pTechnique = m_pLoadEffect->GetLinearTechnique(); break;
    case 2: m_pTechnique = m_pLoadEffect->GetAnisoTechnique();  break;

    }

    // Draw
    D3DX11_TECHNIQUE_DESC techDesc{};
    m_pTechnique->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_pTechnique->GetPassByIndex(p)->Apply(0, pContext);

         switch (m_CurrentCullingMode)
          {
          case Culling::None:  pContext->RSSetState(m_pNoneRasterizerState);
              
              break;
          case Culling::Back:  pContext->RSSetState(m_pBackRasterizerState);           
             break;
          case Culling::Front: pContext->RSSetState(m_pFrontRasterizerState);             
              break;
          }
          
          pContext->DrawIndexed(m_NumIndices, 0, 0);
      }
      

        pContext->DrawIndexed(m_NumIndices, 0, 0);
}



void dae::Mesh::CycleCullMode()
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

        std::cout << "Culling Mode: None\n";
        break;
    case Culling::Back: 
        std::cout << "Culling Mode: Back\n";
        break;
    case Culling::Front:
        std::cout << "Culling Mode: Front\n";
        break;
    }
}


