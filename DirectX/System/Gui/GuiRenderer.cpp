#include "GuiRenderer.h"
#include "GuiContext.h"
#include "GuiVertex.h"
#include "GuiWindow.h"
#include "../AssetsManager.h"
#include "../Shader/ConstantBuffers.h"
#include "../Shader/DataTransfer.h"
#include "../Shader/ShaderBinder.h"
#include "../Texture/TextureBinder.h"
#include "../../Device/Renderer.h"
#include "../../DirectX/DirectXInclude.h"

GuiRenderer::GuiRenderer()
    : mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mVertexBufferSize(INIT_VERTEX_BUFFER_SIZE)
    , mIndexBufferSize(INIT_INDEX_BUFFER_SIZE)
    , mTextureID(AssetsManager::instance().createTexture("System\\pixel.png"))
    , mShaderID(AssetsManager::instance().createShader("Gui.hlsl"))
{
}

GuiRenderer::~GuiRenderer() = default;

void GuiRenderer::draw(const GuiContext& context) {
    //必要に応じてバッファを更新
    createVertexBuffer(context);
    createIndexBuffer(context);

    //描画設定
    drawSettings(context);

    //描画
    auto& dx = MyDirectX::DirectX::instance();
    int vtxOffset = 0;
    int idxOffset = 0;
    auto windowCount = context.getWindowCount();
    for (int i = 0; i < windowCount; ++i) {
        const auto& drawList = context.getWindow(i).getDrawList();
        const auto& cmds = drawList.getDrawCommands();

        auto cmdCount = cmds.size();
        for (int j = 0; j < cmdCount; ++j) {
            const auto& cmd = cmds[j];
            if (cmd.elemCount == 0) {
                continue;
            }

            //dx.drawIndexed(cmd.elemCount, cmd.indexOffset + idxOffset, cmd.vertexOffset + vtxOffset);
            dx.drawIndexed(cmd.elemCount, idxOffset, vtxOffset);
            idxOffset += cmd.elemCount;
        }
        vtxOffset += drawList.getVertexBuffer().size();
    }
}

void GuiRenderer::createVertexBuffer(const GuiContext& context) {
    int totalVertexCount = 0;
    auto windowCount = context.getWindowCount();
    for (size_t i = 0; i < windowCount; ++i) {
        const auto& w = context.getWindow(i);
        totalVertexCount += w.getDrawList().getVertexBuffer().size();
    }

    if (!mVertexBuffer || mVertexBufferSize < totalVertexCount) {
        mVertexBufferSize = totalVertexCount + INIT_VERTEX_BUFFER_SIZE;

        BufferDesc bd{};
        bd.oneSize = sizeof(GuiVertex);
        bd.size = mVertexBufferSize * bd.oneSize;
        bd.usage = Usage::DYNAMIC;
        bd.type = static_cast<unsigned>(BufferType::VERTEX);
        bd.cpuAccessFlags = static_cast<unsigned>(BufferCPUAccessFlag::WRITE);

        mVertexBuffer = std::make_unique<VertexBuffer>(bd);
    }
}

void GuiRenderer::createIndexBuffer(const GuiContext& context) {
    int totalIndexCount = 0;
    auto windowCount = context.getWindowCount();
    for (size_t i = 0; i < windowCount; ++i) {
        const auto& w = context.getWindow(i);
        totalIndexCount += w.getDrawList().getIndexBuffer().size();
    }

    if (!mIndexBuffer || mIndexBufferSize < totalIndexCount) {
        mIndexBufferSize = totalIndexCount + INIT_INDEX_BUFFER_SIZE;

        BufferDesc bd{};
        bd.size = mIndexBufferSize * sizeof(unsigned short);
        bd.usage = Usage::DYNAMIC;
        bd.type = static_cast<unsigned>(BufferType::INDEX);
        bd.cpuAccessFlags = static_cast<unsigned>(BufferCPUAccessFlag::WRITE);

        mIndexBuffer = std::make_unique<IndexBuffer>(bd);
    }
}

void GuiRenderer::drawSettings(const GuiContext& context) const {
    //テクスチャ・シェーダーをバインド
    TextureBinder::bind(mTextureID);
    ShaderBinder::bind(mShaderID);

    //描画設定
    Renderer renderer;
    GuiConstantBuffer cb{};
    renderer.renderGui(cb.wp);
    mVertexBuffer->setVertexBuffer();
    mIndexBuffer->setIndexBuffer();

    //定数バッファをシェーダーに転送
    DataTransfer::transferConstantBuffer(mShaderID, &cb);

    //頂点・インデックスバッファをシェーダーにデータ転送
    auto dc = MyDirectX::DirectX::instance().deviceContext();
    D3D11_MAPPED_SUBRESOURCE vtxRes = { 0 };
    D3D11_MAPPED_SUBRESOURCE idxRes = { 0 };
    dc->Map(mVertexBuffer->buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vtxRes);
    dc->Map(mIndexBuffer->buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &idxRes);

    auto vtxDst = static_cast<GuiVertex*>(vtxRes.pData);
    auto idxDst = static_cast<GuiVertex*>(idxRes.pData);
    auto windowCount = context.getWindowCount();
    for (unsigned i = 0; i < windowCount; ++i) {
        const auto& drawList = context.getWindow(i).getDrawList();
        const auto& vBuffer = drawList.getVertexBuffer();
        const auto& iBuffer = drawList.getIndexBuffer();
        memcpy(vtxDst, vBuffer.data(), vBuffer.size() * sizeof(GuiVertex));
        memcpy(idxDst, iBuffer.data(), iBuffer.size() * sizeof(unsigned short));
        vtxDst += vBuffer.size();
        idxDst += iBuffer.size();
    }

    dc->Unmap(mVertexBuffer->buffer(), 0);
    dc->Unmap(mIndexBuffer->buffer(), 0);
}
