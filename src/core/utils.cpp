#include "core/utils.h"
#include <cstdio>

namespace Utils {
    std::string getAssetPath(const std::string& category, const std::string& filename) {
        if (category == "dragon" || category == "asteroid" || category == "number" || category == "ui" || category == "obstacle" || category == "effects") {
            return "assets/sprites/" + category + "/" + filename;
        } else if (category == "background") {
            return "assets/background/" + filename;
        } else if (category == "audio") {
            return "assets/audio/" + filename;
        } else if (category == "video") {
            return "assets/video/" + filename;
        } else {
            return "assets/" + category + "/" + filename;
        }
    }

    bool getImageDimensions(const std::string& path, int& width, int& height) {
        FILE* file = fopen(path.c_str(), "rb");
        if (!file) return false;

        unsigned char header[24] = {};
        if (fread(header, 1, 24, file) < 24) {
            fclose(file);
            return false;
        }

        if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
            width = (header[16] << 24) | (header[17] << 16) | (header[18] << 8) | header[19];
            height = (header[20] << 24) | (header[21] << 16) | (header[22] << 8) | header[23];
            fclose(file);
            return width > 0 && height > 0;
        }

        if (header[0] == 0xFF && header[1] == 0xD8) {
            fseek(file, 2, SEEK_SET);
            while (!feof(file)) {
                unsigned char marker[2] = {};
                if (fread(marker, 1, 2, file) < 2) break;
                if (marker[0] != 0xFF) break;

                if (marker[1] >= 0xC0 && marker[1] <= 0xCF &&
                    marker[1] != 0xC4 && marker[1] != 0xC8 && marker[1] != 0xCC) {
                    unsigned char size[2] = {};
                    unsigned char dim[4] = {};
                    if (fread(size, 1, 2, file) < 2) break;
                    if (fread(dim, 1, 4, file) < 4) break;
                    height = (dim[0] << 8) | dim[1];
                    width = (dim[2] << 8) | dim[3];
                    fclose(file);
                    return width > 0 && height > 0;
                }

                unsigned char size[2] = {};
                if (fread(size, 1, 2, file) < 2) break;
                int segmentSize = (size[0] << 8) | size[1];
                if (segmentSize < 2) break;
                fseek(file, segmentSize - 2, SEEK_CUR);
            }
        }

        fclose(file);
        return false;
    }
}
