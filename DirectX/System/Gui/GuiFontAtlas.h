#pragma once

class GuiFontAtlas {
public:
    GuiFontAtlas();
    ~GuiFontAtlas();
    GuiFontAtlas(const GuiFontAtlas&) = delete;
    GuiFontAtlas& operator=(const GuiFontAtlas&) = delete;

private:
    void createFontTexture();

private:
    int mTexID;
};
