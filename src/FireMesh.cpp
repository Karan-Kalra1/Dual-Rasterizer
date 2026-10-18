#include "FireMesh.h"
#include <cassert>

using namespace dae;

FireMesh::FireMesh(ID3D11Device* pDevice,
    const std::vector<Vertex_Fire>& vertices,
    const std::vector<uint32_t>& indices)
{
    m_pFireEffect = new FireEffect(pDevice, L"Resources/FireEffect.fx");

    m_pFireTexture = new Texture(pDevice, "Resources/fireFX_diffuse.png");
    m_pFireEffect->SetDiffuseMap(m_pFireTexture->GetSRV());

    m_NumIndices = static_cast<uint32_t>(indices.size());

    // -------- Vertex Buffer --------
    D3D11_BUFFER_DESC vb{};
    vb.Usage = D3D11_USAGE_IMMUTABLE;
    vb.ByteWidth = sizeof(Vertex_Fire) * (uint32_t)vertices.size();
    vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();

    HRESULT hr = pDevice->CreateBuffer(&vb, &vbData, &m_pVertexBuffer);
    assert(SUCCEEDED(hr));

    // -------- Index Buffer --------
    D3D11_BUFFER_DESC ib{};
    ib.Usage = D3D11_USAGE_IMMUTABLE;
    ib.ByteWidth = sizeof(uint32_t) * m_NumIndices;
    ib.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = indices.data();

    hr = pDevice->CreateBuffer(&ib, &ibData, &m_pIndexBuffer);
    assert(SUCCEEDED(hr));

    // -------- Input Layout --------
    D3D11_INPUT_ELEMENT_DESC layout[2]{};

    layout[0].SemanticName = "POSITION";
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    layout[1].SemanticName = "TEXCOORD";
    layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    layout[1].AlignedByteOffset = 12;
    layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    D3DX11_PASS_DESC passDesc{};
    m_pFireEffect->GetFireTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

    hr = pDevice->CreateInputLayout(
        layout,
        2,
        passDesc.pIAInputSignature,
        passDesc.IAInputSignatureSize,
        &m_pInputLayout
    );
    assert(SUCCEEDED(hr));
}

FireMesh::~FireMesh()
{
    if (m_pVertexBuffer) m_pVertexBuffer->Release();
    if (m_pIndexBuffer) m_pIndexBuffer->Release();
    if (m_pInputLayout) m_pInputLayout->Release();

    delete m_pFireEffect;
    delete m_pFireTexture;
}

void FireMesh::Update(Camera* camera, const Timer* pTimer, Matrix& world)
{
    auto view = camera->GetViewMatrix();
    auto proj = camera->GetProjectionMatrix();


    Matrix wvp = world * view * proj;

    m_pFireEffect->GetWVP()->SetMatrix((float*)&wvp);
}

void FireMesh::Render(ID3D11DeviceContext* pContext )
{
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->IASetInputLayout(m_pInputLayout);

    UINT stride = sizeof(Vertex_Fire);
    UINT offset = 0;

    pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3DX11_TECHNIQUE_DESC techDesc{};
    m_pFireEffect->GetFireTechnique()->GetDesc(&techDesc);


    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_pFireEffect->GetFireTechnique()->GetPassByIndex(p)->Apply(0, pContext);
        pContext->DrawIndexed(m_NumIndices, 0, 0);
    }
}
