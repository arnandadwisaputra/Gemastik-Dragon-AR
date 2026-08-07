#include "level/MissionManager.h"
#include <iostream>

MissionManager::MissionManager() {}
MissionManager::~MissionManager() {}

void MissionManager::startMission(const std::string& missionName) {
    std::cout << "Starting mission: " << missionName << std::endl;
}

bool MissionManager::checkMissionStatus() {
    std::cout << "Checking mission objectives..." << std::endl;
    return false;
}
