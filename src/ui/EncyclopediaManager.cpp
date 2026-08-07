#include "ui/EncyclopediaManager.h"
#include <iostream>

EncyclopediaManager::EncyclopediaManager() {}
EncyclopediaManager::~EncyclopediaManager() {}

void EncyclopediaManager::showEntry(const std::string& entryName) {
    std::cout << "Displaying encyclopedia entry for: " << entryName << std::endl;
}
