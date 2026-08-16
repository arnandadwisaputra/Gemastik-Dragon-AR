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
        {"quiz.back_to_info", {"BACK TO INFO", "KEMBALI KE INFORMASI"}},
        
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

        {"briefing.line1", {"THE ANOMALY HAS BEGUN.", "ANOMALI TELAH DIMULAI."}},
        {"briefing.line2", {"Your journey starts here.", "Perjalananmu dimulai di sini."}},
        {"briefing.line3", {"Explore the unknown.", "Jelajahi yang belum diketahui."}},
        {"briefing.line4", {"Move carefully. Watch your surroundings.", "Bergeraklah hati-hati. Perhatikan sekitarmu."}},
        {"briefing.line5", {"Discover the phenomena you encounter.", "Temukan fenomena yang kamu temui."}},
        {"briefing.line6", {"Survive. Learn. Keep moving.", "Bertahan. Belajar. Terus bergerak."}},
        {"briefing.controls", {"MOVE: W / A / S / D  OR  ARROW KEYS", "GERAK: W / A / S / D  ATAU  PANAH"}},
        {"briefing.dash", {"DASH (LEVEL 6): SPACEBAR", "DASH (LEVEL 6): SPACEBAR"}},
        {"briefing.continue", {"CONTINUE", "LANJUTKAN"}},

        {"mission.title", {"MISSION COMPLETE", "MISI SELESAI"}},
        {"mission.subtitle", {"THE JOURNEY OF THE DRAGON", "PERJALANAN SANG NAGA"}},
        {"mission.line1", {"From the first anomaly...", "Dari anomali pertama..."}},
        {"mission.line2", {"The Dragon travelled through asteroid fields, cosmic storms, comets, nebulae and unstable gravity.", "Sang Naga melintasi sabuk asteroid, badai kosmik, komet, nebula, dan gravitasi yang tidak stabil."}},
        {"mission.line3", {"Each challenge revealed another piece of the cosmic disturbance.", "Setiap tantangan mengungkap satu bagian lagi dari gangguan kosmik."}},
        {"mission.line4", {"At the heart of the anomaly, the Dragon restored the balance.", "Di jantung anomali, Sang Naga memulihkan keseimbangan."}},
        {"mission.line5", {"THE MISSION IS COMPLETE.", "MISI TELAH SELESAI."}},
        {"mission.hook", {"BUT IS THE DRAGON'S STORY REALLY OVER?", "TAPI APAKAH CERITA SANG NAGA BENAR-BENAR BERAKHIR?"}},
        {"mission.continue", {"CONTINUE", "LANJUTKAN"}},
        
        {"hud.score", {"SCORE: ", "SKOR: "}},
        {"hud.level", {"LEVEL: ", "LEVEL: "}},
        {"hud.dash_ready", {"DASH READY [SPACEBAR]", "DASH SIAP [SPACEBAR]"}},
        {"hud.dash_charging", {"DASH CHARGING...", "MENGISI DASH..."}},
        
        {"ticker.level1", {
            "SCIENCE: Near-Earth space contains thousands of active satellites. • EXPLORATION TIP: Debris moves fast; react early! • GAME RULE: Use UP/DOWN or W/S keys to fly. • STORY: The Dragon ascends from Earth's atmosphere.",
            "SAINS: Ruang dekat Bumi berisi ribuan satelit aktif. • TIPS EKSPLORASI: Puing bergerak cepat; bersiaplah awal! • ATURAN GAME: Gunakan tombol ATAS/BAWAH atau W/S untuk terbang. • CERITA: Sang Naga lepas landas dari atmosfer Bumi."
        }},
        {"ticker.level2", {
            "SCIENCE: The Asteroid Belt contains millions of rocky bodies. • EXPLORATION TIP: Watch for canyon passages between asteroid columns! • GAME RULE: Survive the timer to complete the level. • STORY: Crossing the Mars-Jupiter boundary.",
            "SAINS: Sabuk Asteroid berisi jutaan benda berbatu. • TIPS EKSPLORASI: Perhatikan jalur celah di antara kolom asteroid! • ATURAN GAME: Bertahanlah hingga waktu level habis untuk menyelesaikannya. • CERITA: Melewati batas Mars-Jupiter."
        }},
        {"ticker.level3", {
            "SCIENCE: Solar flares release massive magnetic energy from the Sun. • EXPLORATION TIP: Flares can come from any of the 4 borders! • GAME RULE: Level 3 allows 4-direction movement. • STORY: The Sun's radiation is at its peak.",
            "SAINS: Suar matahari melepaskan energi magnetik besar dari Matahari. • TIPS EKSPLORASI: Suar dapat muncul dari salah satu dari 4 sisi layar! • ATURAN GAME: Level 3 mengizinkan gerakan ke 4 arah. • CERITA: Radiasi Matahari berada pada puncaknya."
        }},
        {"ticker.level4", {
            "SCIENCE: Comets develop a tail when heated by solar wind. • EXPLORATION TIP: Comets travel diagonally; dodge carefully! • GAME RULE: 360-degree free flight is active. • STORY: Deep space is silent but dangerous.",
            "SAINS: Komet membentuk ekor ketika dipanaskan oleh angin matahari. • TIPS EKSPLORASI: Komet bergerak secara diagonal; menghindarlah dengan hati-hati! • ATURAN GAME: Terbang bebas 360 derajat aktif. • CERITA: Luar angkasa dalam itu sunyi namun berbahaya."
        }},
        {"ticker.level5", {
            "SCIENCE: Pulsars are rotating neutron stars emitting radiation beams. • EXPLORATION TIP: Gravity Wells will pull you in; steer away! • GAME RULE: Watch for the visual shake when caught in gravity. • STORY: Approaching the strong nebula distortions.",
            "SAINS: Pulsar adalah bintang neutron berputar yang memancarkan radiasi. • TIPS EKSPLORASI: Sumur Gravitasi akan menarik Anda; terbanglah menjauh! • ATURAN GAME: Perhatikan getaran visual saat terjebak gravitasi. • CERITA: Mendekati distorsi nebula yang kuat."
        }},
        {"ticker.level6", {
            "SCIENCE: Not even light can escape a black hole's event horizon. • EXPLORATION TIP: Use dash (SPACEBAR) to temporarily escape the central pull! • GAME RULE: Normal keys are disabled; only dash works. • STORY: The final sequence: saving Earth's core.",
            "SAINS: Bahkan cahaya tidak dapat lepas dari horizon peristiwa lubang hitam. • TIPS EKSPLORASI: Gunakan dash (SPACEBAR) untuk lolos sementara dari tarikan pusat! • ATURAN GAME: Tombol gerakan normal dinonaktifkan; hanya dash yang berfungsi. • CERITA: Urutan terakhir: menyelamatkan inti Bumi."
        }}
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
