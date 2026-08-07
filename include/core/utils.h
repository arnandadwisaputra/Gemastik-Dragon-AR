#pragma once
#include <string>

namespace Utils {
    // Returns the correct path for an asset based on category and filename.
    std::string getAssetPath(const std::string& category, const std::string& filename);
}
