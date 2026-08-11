#pragma once
#include <string>
#include <unordered_map>

class BitmapFont {
public:
    BitmapFont();
    ~BitmapFont();

    void load();
    void drawText(const std::string& text, float x, float y, float sizeX = 16.0f, float sizeY = 20.0f, float spacing = 12.0f);
    void drawWrappedText(const std::string& text, float x, float y, float maxWidth, float sizeX = 16.0f, float sizeY = 20.0f, float spacing = 12.0f, float lineSpacing = 26.0f);

private:
    std::unordered_map<char, int> charTextures;
};
