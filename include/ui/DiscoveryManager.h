#pragma once
#include <string>
#include <vector>
#include "core/Localization.h"

struct Discovery {
    int id;
    std::string name;
    std::string shortDescription;
    std::string longDescription;
    std::string question;
    std::vector<std::string> options;
    int correctAnswerIndex; // 0=A, 1=B, 2=C, 3=D
    bool discovered = false;
};

class DiscoveryManager {
public:
    DiscoveryManager();
    ~DiscoveryManager();

    void init();
    void localize(Language lang);
    bool unlockPhenomenon(const std::string& name, bool& isNew);
    bool isDiscovered(const std::string& name) const;
    const Discovery* getDiscovery(const std::string& name) const;
    const Discovery* getDiscoveryById(int id) const;
    std::vector<Discovery>& getAllDiscoveries();
    void reset();

private:
    std::vector<Discovery> discoveries;
};
