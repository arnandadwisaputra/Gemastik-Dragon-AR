#include "core/Localization.h"
#include <unordered_map>

namespace Loc {
    static Language currentLanguage = Language::ENGLISH;

    // A map of key -> (English, Indonesian)
    static std::unordered_map<std::string, std::pair<std::string, std::string>> dictionary = {
        {"menu.start", {"START GAME", "MULAI GAME"}},
        {"menu.encyclopedia", {"ENCYCLOPEDIA", "ENSIKLOPEDIA"}},
        {"menu.language", {"LANGUAGE: ENGLISH", "BAHASA: INDONESIA"}},
        {"menu.exit", {"QUIT GAME", "KELUAR GAME"}},
        {"menu.highscore", {"HIGH SCORE: ", "SKOR TERTINGGI: "}},
        {"menu.debug", {"PRESS [D] TO TOGGLE DEBUG MODE", "TEKAN [D] UNTUK MODE DEBUG"}},
        {"menu.debug_active", {"[DEBUG LEVEL TIMERS ACTIVE]", "[TIMER LEVEL DEBUG AKTIF]"}},
        
        {"pause.title", {"PAUSED", "JEDA"}},
        {"pause.resume", {"RESUME", "LANJUT"}},
        {"pause.encyclopedia", {"ENCYCLOPEDIA", "ENSIKLOPEDIA"}},
        {"pause.menu", {"MAIN MENU", "MENU UTAMA"}},
        
        {"gameover.title", {"GAME OVER", "PERMAINAN BERAKHIR"}},
        {"gameover.failed", {"FAILED IN LEVEL ", "GAGAL DI LEVEL "}},
        {"gameover.score", {"FINAL SCORE: ", "SKOR AKHIR: "}},
        {"gameover.retry", {"RETRY", "COBA LAGI"}},
        {"gameover.menu", {"MENU", "MENU"}},
        
        {"ending.title", {"MISSION COMPLETE", "MISI SELESAI"}},
        {"ending.text1", {"The Dragon was pulled deep into the core of the Black Hole anomaly. Fulfilling the final sequence, it released its life energy - a force tied to Earth's vital ecosystems.", "Sang Naga ditarik jauh ke dalam inti anomali Lubang Hitam. Memenuhi urutan terakhir, ia melepaskan energi kehidupannya - kekuatan yang terikat pada ekosistem vital Bumi."}},
        {"ending.text2", {"This energy successfully stabilized the gravitational distortions, saving Earth's system from collapse. In the story, the connection between biological life and cosmic gravity cores has been preserved.", "Energi ini berhasil menstabilkan distorsi gravitasi, menyelamatkan sistem Bumi dari kehancuran. Dalam cerita ini, hubungan antara kehidupan biologis dan inti gravitasi kosmik tetap terjaga."}},
        {"ending.score", {"FINAL SCORE: ", "SKOR AKHIR: "}},
        {"ending.return", {"PRESS [ENTER] TO RETURN TO MENU", "TEKAN [ENTER] UNTUK KEMBALI KE MENU"}},
        
        {"quiz.title", {"TRANSITION QUIZ - LEVEL ", "KUIS TRANSISI - LEVEL "}},
        {"quiz.instructions", {"PRESS [A], [B], [C], OR [D] TO CHOOSE", "TEKAN [A], [B], [C], ATAU [D] UNTUK MEMILIH"}},
        {"quiz.correct", {"CORRECT ANSWER! +100 SCORE", "JAWABAN BENAR! +100 SKOR"}},
        {"quiz.incorrect", {"INCORRECT! CORRECT ANSWER WAS: ", "SALAH! JAWABAN YANG BENAR ADALAH: "}},
        {"quiz.next", {"PRESS [ENTER] TO ENTER WORMHOLE", "TEKAN [ENTER] UNTUK MASUK WORMHOLE"}},
        
        {"level.complete", {"LEVEL COMPLETE!", "LEVEL SELESAI!"}},
        {"level.complete_log", {"LOG ENCRYPTED PHENOMENON:", "LOG FENOMENA TERENKRIPSI:"}},
        {"level.complete_next", {"PRESS [ENTER] FOR TRANSITION QUIZ", "TEKAN [ENTER] UNTUK KUIS TRANSISI"}},
        
        {"popup.new_discovery", {"NEW DISCOVERY!", "PENEMUAN BARU!"}},
        {"popup.resume", {"PRESS [ENTER] TO RESUME EXPLORATION", "TEKAN [ENTER] UNTUK MELANJUTKAN EKSPLORASI"}},
        
        {"encyclopedia.title", {"SPACE ENCYCLOPEDIA", "ENSIKLOPEDIA ANTARIKSA"}},
        {"encyclopedia.science", {"SCIENCE PHENOMENON DETECTED", "FENOMENA SAINS TERDETEKSI"}},
        {"encyclopedia.locked", {"ENTRY LOCKED", "ENTRI TERKUNCI"}},
        {"encyclopedia.locked_desc", {"Explore the space environments in Dragon Asteroid Run. Discover this phenomenon during your mission to unlock detailed scientific data here.", "Jelajahi lingkungan luar angkasa di Dragon Asteroid Run. Temukan fenomena ini selama misi Anda untuk membuka data ilmiah terperinci di sini."}},
        {"encyclopedia.footer", {"UP/DOWN: NAVIGATE  |  ESC: RETURN TO MENU", "ATAS/BAWAH: NAVIGASI  |  ESC: KEMBALI KE MENU"}},
        
        {"hud.score", {"SCORE: ", "SKOR: "}},
        {"hud.level", {"LEVEL: ", "LEVEL: "}},
        {"hud.dash_ready", {"DASH READY [SPACEBAR]", "DASH SIAP [SPACEBAR]"}},
        {"hud.dash_charging", {"DASH CHARGING...", "MENGISI DASH..."}},
        
        {"ticker.level1", {"ENVIRONMENT * NEAR EARTH: Earth's orbital environment contains artificial satellites and human-made orbital debris.", "LINGKUNGAN * DEKAT BUMI: Lingkungan orbit Bumi berisi satelit buatan dan puing-puing orbital buatan manusia."}},
        {"ticker.level2", {"ENVIRONMENT * ASTEROID BELT: The main asteroid belt lies between the orbits of Mars and Jupiter.", "LINGKUNGAN * SABUK ASTEROID: Sabuk asteroid utama terletak di antara orbit Mars dan Jupiter."}},
        {"ticker.level3", {"ENVIRONMENT * SOLAR ACTIVITY: Solar flares are sudden releases of energy from the Sun's atmosphere.", "LINGKUNGAN * AKTIVITAS MATAHARI: Suar matahari adalah pelepasan energi secara tiba-tiba dari atmosfer Matahari."}},
        {"ticker.level4", {"ENVIRONMENT * DEEP SPACE: Deep space contains cosmic comets and meteoroids traveling at high velocities.", "LINGKUNGAN * LUAR ANGKASA DALAM: Luar angkasa dalam berisi komet kosmik dan meteoroid yang bergerak sangat cepat."}},
        {"ticker.level5", {"ENVIRONMENT * GRAVITATIONAL DISTURBANCE: Nebula clouds and high-gravity pulsars warp the local space fabric.", "LINGKUNGAN * GANGGUAN GRAVITASI: Awan nebula dan pulsar gravitasi tinggi melengkungkan tatanan ruang lokal."}},
        {"ticker.level6", {"ENVIRONMENT * EXTREME GRAVITY: The black hole's singularity exerts an inescapable gravitational pull.", "LINGKUNGAN * GRAVITASI EKSTREM: Singularitas lubang hitam memberikan tarikan gravitasi yang tak terhindarkan."}}
    };

    void setLanguage(Language lang) {
        currentLanguage = lang;
    }

    Language getLanguage() {
        return currentLanguage;
    }

    std::string tr(const std::string& key) {
        auto it = dictionary.find(key);
        if (it != dictionary.end()) {
            if (currentLanguage == Language::INDONESIAN) {
                return it->second.second;
            } else {
                return it->second.first;
            }
        }
        return key;
    }
}
