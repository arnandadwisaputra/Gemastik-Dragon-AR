# Dragon Asteroid Run

Dragon Asteroid Run adalah game arcade bertema luar angkasa yang dibangun menggunakan bahasa C++ dan library grafis SIGIL (Simple Graphics Interface Library). Pemain mengontrol seekor naga untuk menghindari asteroid dan bertahan hidup selama mungkin.

Proyek ini telah dimigrasi dari format Microsoft Visual Studio (.vcxproj) lama ke sistem build CMake yang modern dan dikonfigurasi untuk pengembangan menggunakan Visual Studio Code.

---

## Struktur Folder

```text
DragonAsteroidRun/
├── .vscode/               # Pengaturan workspace, task, dan debug VSCode
├── assets/                # Aset game (gambar, suara, dll)
│   ├── sprites/           # Sprite karakter (naga, asteroid, ui, angka)
│   ├── background/        # Tekstur latar belakang parallax
│   └── audio/             # Efek suara (SFX) dan musik (BGM)
├── docs/                  # Dokumentasi proyek
├── include/               # File header (.h)
│   ├── core/              # Inti game (utils, game, manager arsitektur)
│   ├── entity/            # Entitas game (naga, asteroid)
│   └── level/             # Definisi level/misi
├── lib/
│   └── SIGIL/             # SDK SIGIL (file include, lib, dan DLL)
├── src/                   # File source code (.cpp)
│   ├── core/
│   ├── entity/
│   ├── level/
│   └── ui/
├── save/                  # Lokasi penyimpanan game (save game)
├── CMakeLists.txt         # Konfigurasi build utama CMake
└── README.md              # Dokumentasi proyek (Bahasa Indonesia)
```

---

## Persyaratan Sistem

Untuk melakukan build (compile) dan menjalankan game ini dari source code, pastikan Anda memiliki:
- **Sistem Operasi**: Windows 10/11
- **Kompiler**: Visual Studio Build Tools (MSVC Compiler)
- **Build System**: CMake (versi 3.15 atau terbaru)
- **IDE**: Visual Studio Code (opsional untuk editing kode)
- **Standar C++**: C++17

---

## Panduan Menjalankan Game

### 1. Tanpa Menggunakan VSCode (Cara Langsung)
Anda **tidak perlu** menginstal ekstensi VSCode apa pun jika hanya ingin menjalankan game yang sudah dicompile.
1. Buka File Explorer dan masuk ke folder berikut:
   `Gemastik - Dragon Asteroid Run\Dragon-Asteroid-Run\build\Release`
2. Klik ganda pada file **`DragonAsteroidRun.exe`** untuk langsung memainkan game.

*Alternatif via PowerShell:*
```powershell
cd "Gemastik - Dragon Asteroid Run\Dragon-Asteroid-Run\build\Release"
.\DragonAsteroidRun.exe
```

### 2. Menggunakan VSCode (Untuk Pengembang / Development)
Jika Anda ingin memodifikasi kode atau melakukan compile ulang, Anda perlu menginstal beberapa ekstensi di VSCode:
1. Instal ekstensi **C/C++** dan **CMake Tools** dari marketplace VSCode.
2. Buka folder proyek `Dragon-Asteroid-Run` di VSCode.
3. Jalankan konfigurasi CMake lewat Command Palette (`Ctrl+Shift+P` -> pilih **`CMake: Configure`**).
4. Tekan tombol **`F7`** untuk melakukan compile ulang (Build).
5. Tekan tombol **`F5`** untuk menjalankan game dalam mode Debug/Run.

---

## Kontrol Game

- **Enter**: Mulai Game (dari Menu Utama)
- **Panah Atas**: Terbang ke Atas
- **Panah Bawah**: Terbang ke Bawah
- **Escape**: Pause / Resume Game
- **Spasi**: Minimize Window (saat Pause) / Restart Game (saat Game Over)
- **Q**: Keluar dari Game
- **Klik Kiri Mouse**: Interaksi dengan tombol UI (Start, Retry, Exit)

---

## Rencana Arsitektur Masa Depan (Stubs)

Proyek ini telah menyiapkan struktur placeholder (stubs) untuk pengembangan fitur berikutnya:
- **SceneManager**: Pengatur perpindahan layar/scene game.
- **AudioManager**: Pengatur background music dan efek suara.
- **SaveManager**: Pengatur penyimpanan skor tertinggi (High Score).
- **LevelManager & MissionManager**: Pengatur fase permainan dan misi.
- **EncyclopediaManager & DiscoveryManager**: Pengatur ensiklopedia fenomena luar angkasa.

---

## Kontributor

- **GHZ**
