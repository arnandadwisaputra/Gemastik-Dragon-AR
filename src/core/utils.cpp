#include "core/utils.h"

namespace Utils {
    std::string getAssetPath(const std::string& category, const std::string& filename) {
        if (category == "dragon" || category == "asteroid" || category == "number" || category == "ui" || category == "obstacle") {
            return "assets/sprites/" + category + "/" + filename;
        } else if (category == "background") {
            return "assets/background/" + filename;
        } else if (category == "audio") {
            return "assets/audio/" + filename;
        } else {
            return "assets/" + category + "/" + filename;
        }
    }
}
