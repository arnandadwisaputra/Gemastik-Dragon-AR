#include "ui/DiscoveryManager.h"
#include <iostream>

DiscoveryManager::DiscoveryManager() {}
DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::unlockPhenomenon(const std::string& name) {
    std::cout << "Unlocking space phenomenon: " << name << std::endl;
}
