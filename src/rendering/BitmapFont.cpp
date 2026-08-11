#include "rendering/BitmapFont.h"
#include "core/utils.h"
#include <sl.h>
#include <algorithm>
#include <cctype>

BitmapFont::BitmapFont() {}

BitmapFont::~BitmapFont() {}

void BitmapFont::load() {
    // Load digits 0-9
    for (char c = '0'; c <= '9'; ++c) {
        std::string filename = std::string(1, c) + ".png";
        std::string path = Utils::getAssetPath("fonts", filename);
        charTextures[c] = slLoadTexture(path.c_str());
    }

    // Load uppercase letters A-Z
    for (char c = 'A'; c <= 'Z'; ++c) {
        std::string filename = std::string(1, c) + ".png";
        std::string path = Utils::getAssetPath("fonts", filename);
        charTextures[c] = slLoadTexture(path.c_str());
    }

    // Load special characters
    std::unordered_map<char, std::string> specialChars = {
        {'.', "period.png"},
        {',', "comma.png"},
        {':', "colon.png"},
        {'!', "exclamation.png"},
        {'?', "question.png"},
        {'-', "hyphen.png"},
        {'(', "left_paren.png"},
        {')', "right_paren.png"}
    };

    for (auto const& [ch, filename] : specialChars) {
        std::string path = Utils::getAssetPath("fonts", filename);
        charTextures[ch] = slLoadTexture(path.c_str());
    }
}

void BitmapFont::drawText(const std::string& text, float x, float y, float sizeX, float sizeY, float spacing) {
    float currentX = x;
    for (char c : text) {
        // Convert to uppercase
        char upperC = std::toupper(static_cast<unsigned char>(c));

        if (upperC == ' ') {
            currentX += spacing;
            continue;
        }

        auto it = charTextures.find(upperC);
        if (it != charTextures.end() && it->second != 0) {
            slSprite(it->second, currentX, y, sizeX, sizeY);
        }
        currentX += spacing;
    }
}

void BitmapFont::drawWrappedText(const std::string& text, float x, float y, float maxWidth, float sizeX, float sizeY, float spacing, float lineSpacing) {
    float currentX = x;
    float currentY = y;
    std::string currentWord = "";
    
    auto drawWord = [&](const std::string& word) {
        float wordWidth = word.length() * spacing;
        if (currentX + wordWidth > x + maxWidth) {
            currentX = x;
            currentY -= lineSpacing;
        }
        for (char c : word) {
            char upperC = std::toupper(static_cast<unsigned char>(c));
            auto it = charTextures.find(upperC);
            if (it != charTextures.end() && it->second != 0) {
                slSprite(it->second, currentX, currentY, sizeX, sizeY);
            }
            currentX += spacing;
        }
    };

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == '\n') {
            if (!currentWord.empty()) {
                drawWord(currentWord);
                currentWord = "";
            }
            currentX = x;
            currentY -= lineSpacing;
        } else if (c == ' ') {
            if (!currentWord.empty()) {
                drawWord(currentWord);
                currentWord = "";
            }
            currentX += spacing;
        } else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        drawWord(currentWord);
    }
}

