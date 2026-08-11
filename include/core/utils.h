#pragma once
#include <string>
using std::string;

namespace Utils {
// Returns the correct path for an asset based on category and filename.
string getAssetPath(const string &category, const string &filename);
} // namespace Utils
