# Dragon Asteroid Run

Dragon Asteroid Run is a space-themed arcade game built in C++ using the SIGIL (Simple Graphics Interface Library) graphics framework. The goal is to fly a dragon, avoid waves of incoming asteroids, and survive for as long as possible.

This project has been modernized from Microsoft Visual Studio (vcxproj format) to a cross-platform CMake build system and is configured for development in Visual Studio Code.

---

## Folder Structure

```text
DragonAsteroidRun/
├── .vscode/               # VSCode launch, task, and workspace settings
├── assets/                # Game assets directory
│   ├── sprites/           # Character, UI, and animation sprites
│   ├── background/        # Parallax background textures
│   └── audio/             # Sound effects (SFX) and background music (BGM)
├── docs/                  # Documentation
├── include/               # Public headers
│   ├── core/              # Game engine and core systems (utils, game, managers)
│   ├── entity/            # Game entities (dragon, asteroid)
│   └── level/             # Level structure definitions
├── lib/
│   └── SIGIL/             # Vendorized SIGIL SDK (include, lib, and DLLs)
├── src/                   # Source files matching include/ organization
│   ├── core/
│   ├── entity/
│   ├── level/
│   └── ui/
├── save/                  # Game save location
├── CMakeLists.txt         # Root build configuration
└── README.md              # Project documentation
```

---

## Requirements

To build and run this project, make sure you have the following installed on your system:
- **Operating System**: Windows (tested on Windows 10/11)
- **Compiler**: Visual Studio Build Tools (MSVC Compiler)
- **Build System**: CMake (v3.15 or newer)
- **IDE**: Visual Studio Code
- **C++ Version**: C++17

---

## Build Guide

### 1. Build via VSCode (Recommended)
1. Open the project folder in Visual Studio Code.
2. Ensure you have the **CMake Tools** and **C/C++** extensions installed.
3. Open the Command Palette (`Ctrl+Shift+P`) and run **`CMake: Configure`**.
4. Set the compiler kit to **`Visual Studio Community 2022 Release - amd64`** (or your MSVC equivalent).
5. Build by pressing `F7` (or via status bar build button).
6. Run/Debug by pressing `F5` (runs via `.vscode/launch.json`).

### 2. Build via Command Line
If you prefer building from the terminal, open PowerShell and run:
```powershell
# 1. Create a build directory
mkdir build

# 2. Configure the project using MSVC generator
cmake -B build -S . -G "Visual Studio 17 2022" -A x64

# 3. Compile the executable
cmake --build build --config Release
```
Once compilation completes, the output executable and its DLL dependencies will be located under:
`build/Release/DragonAsteroidRun.exe`

---

## Game Controls

- **Enter**: Start game (from Main Menu)
- **Up Arrow**: Fly Up
- **Down Arrow**: Fly Down
- **Escape**: Pause / Resume game
- **Space**: Minimize window (during Pause) / Restart game (from Game Over screen)
- **Q**: Exit game
- **Left Mouse Click**: Interact with UI buttons (Start, Retry, Exit)

---

## Future Architecture Roadmap

This project prepared architecture placeholders to support future features:
- **SceneManager**: Handles switching screen states.
- **AudioManager**: Handles playing music loops and positional sound effects.
- **SaveManager**: Manages saving and loading high-scores.
- **LevelManager & MissionManager**: Coordinates gameplay phases and challenges.
- **EncyclopediaManager & DiscoveryManager**: Handles space phenomenon lore unlock system.

---

## Contributors

- **Bestics Team** (Original Authors & Migration Guide)
