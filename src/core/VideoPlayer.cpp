#include "core/VideoPlayer.h"
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfmediaengine.h>
#include <mferror.h>
#include <iostream>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")

struct VideoRect {
    int x;
    int y;
    int width;
    int height;
};

static VideoRect CalculateVideoRect(int screenW, int screenH, int videoW, int videoH) {
    float videoAspect = (float)videoW / (float)videoH;
    float screenAspect = (float)screenW / (float)screenH;

    VideoRect r;
    if (videoAspect > screenAspect) {
        // Video is wider than screen: letterbox
        r.width = screenW;
        r.height = (int)(screenW / videoAspect);
        r.x = 0;
        r.y = (screenH - r.height) / 2;
    } else {
        // Video is taller than screen: pillarbox
        r.height = screenH;
        r.width = (int)(screenH * videoAspect);
        r.x = (screenW - r.width) / 2;
        r.y = 0;
    }
    return r;
}

static bool g_IsProgrammaticClose = false;

static LRESULT CALLBACK CutsceneWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CLOSE) {
        DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        if (!g_IsProgrammaticClose) {
            PostQuitMessage(0);
        }
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

static void RegisterCutsceneWindowClass() {
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXA wcx = { 0 };
        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW;
        wcx.lpfnWndProc = CutsceneWndProc;
        wcx.hInstance = GetModuleHandleA(nullptr);
        wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcx.lpszClassName = "DragonCutsceneWindowClass";
        RegisterClassExA(&wcx);
        registered = true;
    }
}

static void GetPhysicalClientRect(HWND hwnd, int& width, int& height) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (!IsProcessDPIAware()) {
        float scale = 1.0f;
        HMODULE hUser32 = GetModuleHandleA("user32.dll");
        if (hUser32) {
            typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
            GetDpiForWindowProc getDpi = (GetDpiForWindowProc)GetProcAddress(hUser32, "GetDpiForWindow");
            if (getDpi) {
                scale = (float)getDpi(hwnd) / 96.0f;
            } else {
                HDC hdc = GetDC(hwnd);
                if (hdc) {
                    scale = (float)GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
                    ReleaseDC(hwnd, hdc);
                }
            }
        }
        w = (int)(w * scale + 0.5f);
        h = (int)(h * scale + 0.5f);
    }
    width = w;
    height = h;
}

struct VideoPlayer::Impl {
    IMFMediaEngine* engine = nullptr;
    bool finished = false;
    bool opened = false;
    bool comInitialized = false;
    bool mfStarted = false;
    HWND parentHwnd = nullptr;
    HWND videoHwnd = nullptr;
    bool loggedDiagnostics = false;

    ~Impl() {
        close();
        if (mfStarted) {
            MFShutdown();
            mfStarted = false;
        }
        if (comInitialized) {
            CoUninitialize();
            comInitialized = false;
        }
    }

    void close() {
        finished = false;
        opened = false;
        loggedDiagnostics = false;
        if (engine) {
            engine->Shutdown();
            engine->Release();
            engine = nullptr;
        }
        if (videoHwnd) {
            DestroyWindow(videoHwnd);
            videoHwnd = nullptr;
        }
    }
};

class MediaEngineEvents : public IMFMediaEngineNotify {
public:
    MediaEngineEvents(VideoPlayer::Impl* ownerImpl) : refCount(1), owner(ownerImpl) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == __uuidof(IMFMediaEngineNotify)) {
            *ppv = static_cast<IMFMediaEngineNotify*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&refCount); }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG count = InterlockedDecrement(&refCount);
        if (count == 0) delete this;
        return count;
    }

    STDMETHODIMP EventNotify(DWORD event, DWORD_PTR param1, DWORD param2) override {
        (void)param1;
        (void)param2;
        if (!owner) return S_OK;
        if (event == MF_MEDIA_ENGINE_EVENT_ENDED || event == MF_MEDIA_ENGINE_EVENT_ERROR) {
            owner->finished = true;
        }
        return S_OK;
    }

private:
    long refCount;
    VideoPlayer::Impl* owner;
};

VideoPlayer::VideoPlayer() : impl(new Impl()) {}

VideoPlayer::~VideoPlayer() {
    delete impl;
}

bool VideoPlayer::open(const std::string& path) {
    close();

    HRESULT hr = S_OK;
    if (!impl->mfStarted) {
        hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (SUCCEEDED(hr)) {
            impl->comInitialized = true;
        } else if (hr != RPC_E_CHANGED_MODE) {
            std::cerr << "VideoPlayer: CoInitializeEx failed" << std::endl;
            return false;
        }

        hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) {
            std::cerr << "VideoPlayer: MFStartup failed" << std::endl;
            return false;
        }
        impl->mfStarted = true;
    }

    HWND hwnd = FindWindowA(nullptr, "Dragon Asteroid Run");
    if (!hwnd) {
        std::cerr << "VideoPlayer: game window not found" << std::endl;
        return false;
    }
    impl->parentHwnd = hwnd;

    // Query monitor info of the screen where the game window is currently running
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
    if (!GetMonitorInfoA(hMonitor, &monitorInfo)) {
        // Fallback to primary screen bounds
        monitorInfo.rcMonitor.left = 0;
        monitorInfo.rcMonitor.top = 0;
        monitorInfo.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        monitorInfo.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    int x = monitorInfo.rcMonitor.left;
    int y = monitorInfo.rcMonitor.top;
    int w = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    int h = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    RegisterCutsceneWindowClass();
    impl->videoHwnd = CreateWindowExA(
        WS_EX_TOPMOST, // Topmost window to cover everything including taskbars
        "DragonCutsceneWindowClass",
        nullptr,
        WS_POPUP | WS_VISIBLE,
        x, y, w, h,
        hwnd, // Set game window as owner
        nullptr,
        GetModuleHandleA(nullptr),
        nullptr
    );

    if (!impl->videoHwnd) {
        std::cerr << "VideoPlayer: failed to create cutscene window" << std::endl;
        return false;
    }

    // Make the window topmost, force size and position, and transfer focus
    SetWindowPos(impl->videoHwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    SetForegroundWindow(impl->videoHwnd);
    SetFocus(impl->videoHwnd);
    UpdateWindow(impl->videoHwnd);

    // Hide the mouse cursor during video playback
    ShowCursor(FALSE);

    // Hide the game window to prevent it from overlaying or conflicting with the cutscene rendering
    ShowWindow(hwnd, SW_HIDE);

    IMFAttributes* attrs = nullptr;
    hr = MFCreateAttributes(&attrs, 4);
    if (FAILED(hr)) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        return false;
    }

    MediaEngineEvents* callback = new MediaEngineEvents(impl);
    attrs->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, callback);
    attrs->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND, (UINT64)impl->videoHwnd);

    IMFMediaEngineClassFactory* factory = nullptr;
    hr = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        callback->Release();
        attrs->Release();
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        std::cerr << "VideoPlayer: failed to create media engine factory" << std::endl;
        return false;
    }

    hr = factory->CreateInstance(0, attrs, &impl->engine);
    factory->Release();
    attrs->Release();
    callback->Release();

    if (FAILED(hr) || !impl->engine) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        std::cerr << "VideoPlayer: failed to create media engine" << std::endl;
        return false;
    }

    char absPath[MAX_PATH];
    if (!_fullpath(absPath, path.c_str(), MAX_PATH)) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        std::cerr << "VideoPlayer: invalid path " << path << std::endl;
        return false;
    }

    std::wstring wpath(absPath, absPath + strlen(absPath));
    BSTR bstr = SysAllocString(wpath.c_str());
    if (!bstr) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        return false;
    }

    hr = impl->engine->SetSource(bstr);
    SysFreeString(bstr);
    if (FAILED(hr)) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
        ShowCursor(TRUE);
        ShowWindow(hwnd, SW_SHOW);
        std::cerr << "VideoPlayer: SetSource failed for " << path << std::endl;
        return false;
    }

    impl->opened = true;
    impl->finished = false;
    return true;
}

void VideoPlayer::play() {
    if (!impl->engine || !impl->opened) return;
    impl->engine->Load();
    impl->engine->Play();
}

void VideoPlayer::update() {
    if (!impl->engine || !impl->opened || impl->finished || !impl->videoHwnd) return;

    // Process Win32 window messages for the cutscene window and thread level (nullptr filter)
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            impl->finished = true;
            // Clean up resources and terminate process immediately
            exit(0);
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    double currentTime = impl->engine->GetCurrentTime();
    double duration = impl->engine->GetDuration();

    // Preserve aspect ratio and fit the video into the cutscene window physical client area
    DWORD nativeW = 0, nativeH = 0;
    if (SUCCEEDED(impl->engine->GetNativeVideoSize(&nativeW, &nativeH)) && nativeW > 0 && nativeH > 0) {
        int winW = 0, winH = 0;
        GetPhysicalClientRect(impl->videoHwnd, winW, winH);

        VideoRect rect = CalculateVideoRect(winW, winH, nativeW, nativeH);

        RECT destRect;
        destRect.left = rect.x;
        destRect.top = rect.y;
        destRect.right = rect.x + rect.width;
        destRect.bottom = rect.y + rect.height;

        if (!impl->loggedDiagnostics) {
            RECT monitorRect;
            HMONITOR hMonitor = MonitorFromWindow(impl->videoHwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo = { sizeof(monitorInfo) };
            if (GetMonitorInfoA(hMonitor, &monitorInfo)) {
                monitorRect = monitorInfo.rcMonitor;
            } else {
                monitorRect.left = 0;
                monitorRect.top = 0;
                monitorRect.right = GetSystemMetrics(SM_CXSCREEN);
                monitorRect.bottom = GetSystemMetrics(SM_CYSCREEN);
            }

            std::cout << "SCREEN: " << (monitorRect.right - monitorRect.left) << "x" << (monitorRect.bottom - monitorRect.top) << std::endl;
            std::cout << "MONITOR: left=" << monitorRect.left << " top=" << monitorRect.top << " right=" << monitorRect.right << " bottom=" << monitorRect.bottom << std::endl;
            std::cout << "CLIENT: " << winW << "x" << winH << std::endl;
            std::cout << "VIDEO: " << nativeW << "x" << nativeH << std::endl;
            std::cout << "VIDEO RECT: x=" << destRect.left << " y=" << destRect.top << " width=" << rect.width << " height=" << rect.height << std::endl;

            impl->loggedDiagnostics = true;
        }

        IMFMediaEngineEx* engineEx = nullptr;
        if (SUCCEEDED(impl->engine->QueryInterface(IID_PPV_ARGS(&engineEx)))) {
            MFVideoNormalizedRect sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
            MFARGB borderColor;
            borderColor.rgbBlue = 0;
            borderColor.rgbGreen = 0;
            borderColor.rgbRed = 0;
            borderColor.rgbAlpha = 255;
            engineEx->UpdateVideoStream(&sourceRect, &destRect, &borderColor);
            engineEx->Release();
        }
    }

    if (duration > 0.0 && currentTime >= duration - 0.05) {
        impl->finished = true;
    }
}

bool VideoPlayer::isFinished() const {
    return impl->finished;
}

bool VideoPlayer::isOpen() const {
    return impl->opened;
}

void VideoPlayer::close() {
    if (!impl->opened) {
        return;
    }
    ShowCursor(TRUE);
    HWND gameHwnd = impl->parentHwnd;

    // Log before close
    if (gameHwnd) {
        std::cout << "[VIDEO] Before close" << std::endl;
        std::cout << "[WINDOW] game hwnd = " << gameHwnd << std::endl;
        std::cout << "[WINDOW] IsWindow = " << (IsWindow(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsVisible = " << (IsWindowVisible(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsIconic = " << (IsIconic(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsZoomed = " << (IsZoomed(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] video hwnd = " << impl->videoHwnd << std::endl;
    }

    // Restore and show game window BEFORE closing the video
    if (gameHwnd && IsWindow(gameHwnd)) {
        if (IsIconic(gameHwnd)) {
            ShowWindow(gameHwnd, SW_RESTORE);
        }
        ShowWindow(gameHwnd, SW_SHOW);
        SetForegroundWindow(gameHwnd);
        SetFocus(gameHwnd);
        SetActiveWindow(gameHwnd);
    }

    g_IsProgrammaticClose = true;
    impl->close();
    g_IsProgrammaticClose = false;

    // Log after close
    if (gameHwnd) {
        std::cout << "[VIDEO] After close" << std::endl;
        std::cout << "[WINDOW] game hwnd = " << gameHwnd << std::endl;
        std::cout << "[WINDOW] IsWindow = " << (IsWindow(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsVisible = " << (IsWindowVisible(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsIconic = " << (IsIconic(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
        std::cout << "[WINDOW] IsZoomed = " << (IsZoomed(gameHwnd) ? "TRUE" : "FALSE") << std::endl;
    }

    // Force focus again after closing the video window just in case focus was lost during destruction
    if (gameHwnd && IsWindow(gameHwnd)) {
        if (IsIconic(gameHwnd)) {
            ShowWindow(gameHwnd, SW_RESTORE);
        }
        ShowWindow(gameHwnd, SW_SHOW);
        SetForegroundWindow(gameHwnd);
        SetFocus(gameHwnd);
        SetActiveWindow(gameHwnd);
    }
}
