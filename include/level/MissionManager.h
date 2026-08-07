#pragma once
#include <string>

class MissionManager {
public:
    MissionManager();
    ~MissionManager();
    void startMission(const std::string& missionName);
    bool checkMissionStatus();
};
