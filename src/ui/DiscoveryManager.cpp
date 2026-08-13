#include "ui/DiscoveryManager.h"
#include <iostream>
#include <algorithm>

DiscoveryManager::DiscoveryManager() {
    init();
}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::init() {
    discoveries.clear();
    for (int i = 0; i < 9; ++i) {
        Discovery d;
        d.id = i + 1;
        d.discovered = false;
        
        // Default correctAnswerIndex matching original logic
        if (i == 0) d.correctAnswerIndex = 1;      // B
        else if (i == 1) d.correctAnswerIndex = 0; // A
        else if (i == 2) d.correctAnswerIndex = 2; // C
        else if (i == 3) d.correctAnswerIndex = 1; // B
        else if (i == 4) d.correctAnswerIndex = 3; // D
        else if (i == 5) d.correctAnswerIndex = 1; // B
        else if (i == 6) d.correctAnswerIndex = 3; // D
        else if (i == 7) d.correctAnswerIndex = 0; // A
        else if (i == 8) d.correctAnswerIndex = 1; // B
        
        d.options.resize(4);
        discoveries.push_back(d);
    }
    localize(Language::ENGLISH);
}

void DiscoveryManager::localize(Language lang) {
    if (discoveries.size() < 9) return;
    
    if (lang == Language::ENGLISH) {
        // 1. ASTEROID
        discoveries[0].name = "ASTEROID";
        discoveries[0].shortDescription = "ASTEROID: A small rocky body orbiting the Sun.";
        discoveries[0].longDescription = "Asteroids are rocky, airless remnants left over from the early formation of our solar system about 4.6 billion years ago. Most of this ancient space rubble can be found orbiting the Sun between Mars and Jupiter.";
        discoveries[0].question = "Where is the main Asteroid Belt located?";
        discoveries[0].options = {"Between Earth and Mars", "Between Mars and Jupiter", "Between Jupiter and Saturn", "Beyond Neptune"};
        
        // 2. SATELLITE
        discoveries[1].name = "SATELLITE";
        discoveries[1].shortDescription = "SATELLITE: An artificial object placed in orbit.";
        discoveries[1].longDescription = "Artificial satellites are human-made machines launched into space to orbit Earth or other celestial bodies. They are used for communication, weather monitoring, navigation, and scientific research.";
        discoveries[1].question = "What is the primary function of GPS satellites?";
        discoveries[1].options = {"Global navigation", "Asteroid mining", "Solar flare shield", "Weather control"};
        
        // 3. SPACE DEBRIS
        discoveries[2].name = "SPACE_DEBRIS";
        discoveries[2].shortDescription = "SPACE DEBRIS: Defunct human-made objects in space.";
        discoveries[2].longDescription = "Space debris, also known as space junk, consists of artificial objects orbiting Earth that no longer serve any useful function. This includes derelict spacecraft, non-functional satellites, and rocket stages.";
        discoveries[2].question = "Why is space debris extremely dangerous?";
        discoveries[2].options = {"It burns up instantly", "It blocks all sunlight", "It travels at extremely high speeds", "It contains alien viruses"};
        
        // 4. SOLAR FLARE
        discoveries[3].name = "SOLAR_FLARE";
        discoveries[3].shortDescription = "SOLAR FLARE: A sudden eruption of solar radiation.";
        discoveries[3].longDescription = "A solar flare is an intense burst of radiation coming from the release of magnetic energy associated with sunspots. Flares are our solar system's largest explosive events, releasing energy equivalent to millions of hydrogen bombs.";
        discoveries[3].question = "What causes solar flares on the Sun?";
        discoveries[3].options = {"Asteroid collisions", "Release of magnetic energy", "Nuclear waste dumping", "Cooling of the Sun's core"};
        
        // 5. COMET
        discoveries[4].name = "COMET";
        discoveries[4].shortDescription = "COMET: An icy, dusty body with a visible tail.";
        discoveries[4].longDescription = "Comets are cosmic snowballs of frozen gases, rock, and dust that orbit the Sun. When a comet's orbit brings it close to the Sun, it heats up and spews dust and gases into a giant glowing head and a long tail.";
        discoveries[4].question = "What forms a comet's tail when it approaches the Sun?";
        discoveries[4].options = {"Atmospheric friction", "Gravitational friction", "Engine exhaust", "Solar wind and heat evaporating ice"};
        
        // 6. METEOROID
        discoveries[5].name = "METEOROID";
        discoveries[5].shortDescription = "METEOROID: A small rocky or metallic body in space.";
        discoveries[5].longDescription = "Meteoroids are objects in space that range in size from dust grains to small asteroids. When a meteoroid enters Earth's atmosphere at high speed and burns up, it is called a meteor or 'shooting star'.";
        discoveries[5].question = "What is a meteoroid called when it enters the atmosphere and burns?";
        discoveries[5].options = {"Meteorite", "Meteor", "Comet", "Pulsar"};
        
        // 7. PULSAR
        discoveries[6].name = "PULSAR";
        discoveries[6].shortDescription = "PULSAR: A highly magnetized rotating neutron star.";
        discoveries[6].longDescription = "Pulsars are rotating neutron stars that emit beams of electromagnetic radiation out of their magnetic poles. Because their radiation beams sweep past Earth at regular intervals, they appear to pulse.";
        discoveries[6].question = "Pulsars are rotating remnants of what type of star?";
        discoveries[6].options = {"White dwarf", "Red giant", "Yellow dwarf", "Neutron star"};
        
        // 8. GRAVITY
        discoveries[7].name = "GRAVITY";
        discoveries[7].shortDescription = "GRAVITY: The force pulling objects together.";
        discoveries[7].longDescription = "Gravity is a fundamental force of attraction that acts between all matter. Massive objects like stars and planets warp spacetime, creating a gravitational pull that attracts other objects.";
        discoveries[7].question = "According to physics, gravitational force decreases with:";
        discoveries[7].options = {"Increasing distance squared", "Decreasing distance", "Increasing size of target", "Time elapsed"};
        
        // 9. BLACK HOLE
        discoveries[8].name = "BLACK_HOLE";
        discoveries[8].shortDescription = "BLACK HOLE: A region where gravity prevents escape of light.";
        discoveries[8].longDescription = "A black hole is a region of spacetime where gravity is so strong that nothing, not even light, can escape from it. It is formed when a massive star collapses under its own gravity at the end of its life.";
        discoveries[8].question = "What is the boundary around a black hole called?";
        discoveries[8].options = {"Gravity well", "Event horizon", "Aura limit", "Nebula boundary"};
        
    } else {
        // 1. ASTEROID
        discoveries[0].name = "ASTEROID";
        discoveries[0].shortDescription = "ASTEROID: Benda berbatu kecil yang mengorbit Matahari.";
        discoveries[0].longDescription = "Asteroid adalah sisa-sisa berbatu tanpa udara yang tersisa dari formasi awal tata surya kita sekitar 4,6 miliar tahun lalu. Sebagian besar puing antariksa kuno ini dapat ditemukan mengorbit Matahari di antara Mars dan Jupiter.";
        discoveries[0].question = "Di mana Sabuk Asteroid utama terletak?";
        discoveries[0].options = {"Antara Bumi dan Mars", "Antara Mars dan Jupiter", "Antara Jupiter dan Saturnus", "Di luar Neptunus"};
        
        // 2. SATELLITE
        discoveries[1].name = "SATELLITE";
        discoveries[1].shortDescription = "SATELIT: Objek buatan yang ditempatkan di orbit.";
        discoveries[1].longDescription = "Satelit buatan adalah mesin buatan manusia yang diluncurkan ke luar angkasa untuk mengorbit Bumi atau benda langit lainnya. Mereka digunakan untuk komunikasi, pemantauan cuaca, navigasi, dan penelitian ilmiah.";
        discoveries[1].question = "Apa fungsi utama dari satelit GPS?";
        discoveries[1].options = {"Navigasi global", "Penambangan asteroid", "Perisai suar surya", "Pengendalian cuaca"};
        
        // 3. SPACE DEBRIS
        discoveries[2].name = "SPACE_DEBRIS";
        discoveries[2].shortDescription = "PUING ANTARIKSA: Objek mati buatan manusia di luar angkasa.";
        discoveries[2].longDescription = "Puing antariksa, juga dikenal sebagai sampah antariksa, terdiri dari objek buatan yang mengorbit Bumi yang tidak lagi berfungsi. Ini termasuk pesawat luar angkasa yang ditinggalkan, satelit yang tidak berfungsi, dan tahapan roket.";
        discoveries[2].question = "Mengapa puing antariksa sangat berbahaya?";
        discoveries[2].options = {"Terbakar secara instan", "Menghalangi semua sinar matahari", "Bergerak dengan kecepatan sangat tinggi", "Mengandung virus alien"};
        
        // 4. SOLAR FLARE
        discoveries[3].name = "SOLAR_FLARE";
        discoveries[3].shortDescription = "SUAR MATAHARI: Letusan radiasi matahari yang tiba-tiba.";
        discoveries[3].longDescription = "Suar matahari adalah ledakan radiasi intens yang berasal dari pelepasan energi magnetik yang terkait dengan bintik matahari. Suar adalah peristiwa ledakan terbesar di tata surya kita, melepaskan energi yang setara dengan jutaan bom hidrogen.";
        discoveries[3].question = "Apa yang menyebabkan suar matahari pada Matahari?";
        discoveries[3].options = {"Tabrakan asteroid", "Pelepasan energi magnetik", "Pembuangan limbah nuklir", "Pendinginan inti Matahari"};
        
        // 5. COMET
        discoveries[4].name = "COMET";
        discoveries[4].shortDescription = "KOMET: Benda es berdebu dengan ekor yang terlihat.";
        discoveries[4].longDescription = "Komet adalah bola salju kosmik dari gas beku, batu, dan debu yang mengorbit Matahari. Ketika orbit komet membawanya dekat dengan Matahari, ia memanas dan menyemburkan debu serta gas menjadi kepala bercahaya raksasa dan ekor yang panjang.";
        discoveries[4].question = "Apa yang membentuk ekor komet saat mendekati Matahari?";
        discoveries[4].options = {"Gesekan atmosfer", "Gesekan gravitasi", "Knalpot mesin", "Angin matahari dan panas yang menguapkan es"};
        
        // 6. METEOROID
        discoveries[5].name = "METEOROID";
        discoveries[5].shortDescription = "METEOROID: Benda berbatu atau logam kecil di luar angkasa.";
        discoveries[5].longDescription = "Meteoroid adalah objek di luar angkasa yang ukurannya berkisar dari butiran debu hingga asteroid kecil. Ketika meteoroid memasuki atmosfer Bumi dengan kecepatan tinggi dan terbakar, itu disebut meteor atau 'bintang jatuh'.";
        discoveries[5].question = "Apa nama meteoroid ketika memasuki atmosfer dan terbakar?";
        discoveries[5].options = {"Meteorit", "Meteor", "Komet", "Pulsar"};
        
        // 7. PULSAR
        discoveries[6].name = "PULSAR";
        discoveries[6].shortDescription = "PULSAR: Bintang neutron berputar yang sangat termagnetisasi.";
        discoveries[6].longDescription = "Pulsar adalah bintang neutron berputar yang memancarkan berkas radiasi elektromagnetik dari kutub magnetnya. Karena berkas radiasi mereka melewati Bumi pada interval yang teratur, mereka tampak berdenyut.";
        discoveries[6].question = "Pulsar adalah sisa-sisa berputar dari jenis bintang apa?";
        discoveries[6].options = {"Katai putih", "Raksasa merah", "Katai kuning", "Bintang neutron"};
        
        // 8. GRAVITY
        discoveries[7].name = "GRAVITY";
        discoveries[7].shortDescription = "GRAVITASI: Gaya yang menarik objek bersama-sama.";
        discoveries[7].longDescription = "Gravitasi adalah gaya tarik mendasar yang bekerja di antara semua materi. Objek masif seperti bintang dan planet melengkungkan ruang-waktu, menciptakan tarikan gravitasi yang menarik objek lain.";
        discoveries[7].question = "Menurut fisika, gaya gravitasi berkurang dengan:";
        discoveries[7].options = {"Kuadrat jarak yang bertambah", "Jarak yang berkurang", "Ukuran target yang bertambah", "Waktu yang berlalu"};
        
        // 9. BLACK HOLE
        discoveries[8].name = "BLACK_HOLE";
        discoveries[8].shortDescription = "LUBANG HITAM: Wilayah di mana gravitasi mencegah lolosnya cahaya.";
        discoveries[8].longDescription = "Lubang hitam adalah wilayah ruang-waktu di mana gravitasi sangat kuat sehingga tidak ada apa pun, bahkan cahaya, yang dapat melarikan diri darinya. Ini terbentuk ketika bintang masif runtuh di bawah gravitasinya sendiri di akhir masa hidupnya.";
        discoveries[8].question = "Apa nama batas di sekitar lubang hitam?";
        discoveries[8].options = {"Sumur gravitasi", "Horizon peristiwa", "Batas aura", "Batas nebula"};
    }
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
