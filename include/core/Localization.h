#pragma once
#include <string>

enum class Language {
    ENGLISH = 0,
    INDONESIAN
};

namespace Loc {
    void setLanguage(Language lang);
    Language getLanguage();
    std::string tr(const std::string& key);
}
