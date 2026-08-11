#include "ui/DiscoveryManager.h"
#include <iostream>
#include <algorithm>

DiscoveryManager::DiscoveryManager() {
    init();
}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::init() {
    discoveries.clear();

    // 1. ASTEROID
    discoveries.push_back({
        1,
        "ASTEROID",
        "ASTEROID: A small rocky body orbiting the Sun.",
        "Asteroids are rocky, airless remnants left over from the early formation of our solar system about 4.6 billion years ago. Most of this ancient space rubble can be found orbiting the Sun between Mars and Jupiter.",
        "Where is the main Asteroid Belt located?",
        {"Between Earth and Mars", "Between Mars and Jupiter", "Between Jupiter and Saturn", "Beyond Neptune"},
        1, // B
        false
    });

    // 2. SATELLITE
    discoveries.push_back({
        2,
        "SATELLITE",
        "SATELLITE: An artificial object placed in orbit.",
        "Artificial satellites are human-made machines launched into space to orbit Earth or other celestial bodies. They are used for communication, weather monitoring, navigation, and scientific research.",
        "What is the primary function of GPS satellites?",
        {"Global navigation", "Asteroid mining", "Solar flare shield", "Weather control"},
        0, // A
        false
    });

    // 3. SPACE DEBRIS
    discoveries.push_back({
        3,
        "SPACE DEBRIS",
        "SPACE DEBRIS: Defunct human-made objects in space.",
        "Space debris, also known as space junk, consists of artificial objects orbiting Earth that no longer serve any useful function. This includes derelict spacecraft, non-functional satellites, and rocket stages.",
        "Why is space debris extremely dangerous?",
        {"It burns up instantly", "It blocks all sunlight", "It travels at extremely high speeds", "It contains alien viruses"},
        2, // C
        false
    });

    // 4. SOLAR FLARE
    discoveries.push_back({
        4,
        "SOLAR FLARE",
        "SOLAR FLARE: A sudden eruption of solar radiation.",
        "A solar flare is an intense burst of radiation coming from the release of magnetic energy associated with sunspots. Flares are our solar system's largest explosive events, releasing energy equivalent to millions of hydrogen bombs.",
        "What causes solar flares on the Sun?",
        {"Asteroid collisions", "Release of magnetic energy", "Nuclear waste dumping", "Cooling of the Sun's core"},
        1, // B
        false
    });

    // 5. COMET
    discoveries.push_back({
        5,
        "COMET",
        "COMET: An icy, dusty body with a visible tail.",
        "Comets are cosmic snowballs of frozen gases, rock, and dust that orbit the Sun. When a comet's orbit brings it close to the Sun, it heats up and spews dust and gases into a giant glowing head and a long tail.",
        "What forms a comet's tail when it approaches the Sun?",
        {"Atmospheric friction", "Gravitational friction", "Engine exhaust", "Solar wind and heat evaporating ice"},
        3, // D
        false
    });

    // 6. METEOROID
    discoveries.push_back({
        6,
        "METEOROID",
        "METEOROID: A small rocky or metallic body in space.",
        "Meteoroids are objects in space that range in size from dust grains to small asteroids. When a meteoroid enters Earth's atmosphere at high speed and burns up, it is called a meteor or 'shooting star'.",
        "What is a meteoroid called when it enters the atmosphere and burns?",
        {"Meteorite", "Meteor", "Comet", "Pulsar"},
        1, // B
        false
    });

    // 7. PULSAR
    discoveries.push_back({
        7,
        "PULSAR",
        "PULSAR: A highly magnetized rotating neutron star.",
        "Pulsars are rotating neutron stars that emit beams of electromagnetic radiation out of their magnetic poles. Because their radiation beams sweep past Earth at regular intervals, they appear to pulse.",
        "Pulsars are rotating remnants of what type of star?",
        {"White dwarf", "Red giant", "Yellow dwarf", "Neutron star"},
        3, // D
        false
    });

    // 8. GRAVITY
    discoveries.push_back({
        8,
        "GRAVITY",
        "GRAVITY: The force pulling objects together.",
        "Gravity is a fundamental force of attraction that acts between all matter. Massive objects like stars and planets warp spacetime, creating a gravitational pull that attracts other objects.",
        "According to physics, gravitational force decreases with:",
        {"Increasing distance squared", "Decreasing distance", "Increasing size of target", "Time elapsed"},
        0, // A
        false
    });

    // 9. BLACK HOLE
    discoveries.push_back({
        9,
        "BLACK HOLE",
        "BLACK HOLE: A region where gravity prevents escape of light.",
        "A black hole is a region of spacetime where gravity is so strong that nothing, not even light, can escape from it. It is formed when a massive star collapses under its own gravity at the end of its life.",
        "What is the boundary around a black hole called?",
        {"Gravity well", "Event horizon", "Aura limit", "Nebula boundary"},
        1, // B
        false
    });
}

bool DiscoveryManager::unlockPhenomenon(const std::string& name, bool& isNew) {
    isNew = false;
    for (auto& d : discoveries) {
        if (d.name == name) {
            if (!d.discovered) {
                d.discovered = true;
                isNew = true;
                std::cout << "Unlocked: " << name << std::endl;
                return true;
            }
            return false;
        }
    }
    return false;
}

bool DiscoveryManager::isDiscovered(const std::string& name) const {
    for (auto const& d : discoveries) {
        if (d.name == name) {
            return d.discovered;
        }
    }
    return false;
}

const Discovery* DiscoveryManager::getDiscovery(const std::string& name) const {
    for (auto const& d : discoveries) {
        if (d.name == name) {
            return &d;
        }
    }
    return nullptr;
}

const Discovery* DiscoveryManager::getDiscoveryById(int id) const {
    for (auto const& d : discoveries) {
        if (d.id == id) {
            return &d;
        }
    }
    return nullptr;
}

std::vector<Discovery>& DiscoveryManager::getAllDiscoveries() {
    return discoveries;
}

void DiscoveryManager::reset() {
    for (auto& d : discoveries) {
        d.discovered = false;
    }
}
