#pragma once

#include <memory>

class GuiContext;
class VertexBuffer;
class IndexBuffer;

class GuiRenderer {
public:
    GuiRenderer();
    ~GuiRenderer();
    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer& operator=(const GuiRenderer&) = delete;

    void draw(const GuiContext& context);

private:
    void createVertexBuffer(const GuiContext& context);
    void createIndexBuffer(const GuiContext& context);
    void drawSettings(const GuiContext& context) const;

private:
    std::unique_ptr<VertexBuffer> mVertexBuffer;
    std::unique_ptr<IndexBuffer> mIndexBuffer;
    int mVertexBufferSize;
    int mIndexBufferSize;
    int mTextureID;
    int mShaderID;

    static constexpr int INIT_VERTEX_BUFFER_SIZE = 5000;
    static constexpr int INIT_INDEX_BUFFER_SIZE = 10000;
};
