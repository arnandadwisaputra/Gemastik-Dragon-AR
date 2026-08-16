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

static void RegisterVideoChildClass() {
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXA wcx = { 0 };
        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW;
        wcx.lpfnWndProc = DefWindowProcA;
        wcx.hInstance = GetModuleHandleA(nullptr);
        wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcx.lpszClassName = "DragonVideoPlayerChildClass";
        RegisterClassExA(&wcx);
        registered = true;
    }
}

struct VideoPlayer::Impl {
    IMFMediaEngine* engine = nullptr;
    bool finished = false;
    bool opened = false;
    bool comInitialized = false;
    bool mfStarted = false;
    HWND parentHwnd = nullptr;
    HWND videoHwnd = nullptr;

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

    RegisterVideoChildClass();
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    impl->videoHwnd = CreateWindowExA(
        0,
        "DragonVideoPlayerChildClass",
        nullptr,
        WS_CHILD | WS_VISIBLE,
        0, 0, width, height,
        hwnd,
        nullptr,
        GetModuleHandleA(nullptr),
        nullptr
    );

    if (!impl->videoHwnd) {
        std::cerr << "VideoPlayer: failed to create child video window" << std::endl;
        return false;
    }

    IMFAttributes* attrs = nullptr;
    hr = MFCreateAttributes(&attrs, 4);
    if (FAILED(hr)) {
        DestroyWindow(impl->videoHwnd);
        impl->videoHwnd = nullptr;
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
        std::cerr << "VideoPlayer: failed to create media engine factory" << std::endl;
        return false;
    }

    hr = factory->CreateInstance(0, attrs, &impl->engine);
    factory->Release();
    attrs->Release();
    callback->Release();

    if (FAILED(hr) || !impl->engine) {
        std::cerr << "VideoPlayer: failed to create media engine" << std::endl;
        return false;
    }

    char absPath[MAX_PATH];
    if (!_fullpath(absPath, path.c_str(), MAX_PATH)) {
        std::cerr << "VideoPlayer: invalid path " << path << std::endl;
        return false;
    }

    std::wstring wpath(absPath, absPath + strlen(absPath));
    BSTR bstr = SysAllocString(wpath.c_str());
    if (!bstr) return false;

    hr = impl->engine->SetSource(bstr);
    SysFreeString(bstr);
    if (FAILED(hr)) {
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
    if (!impl->engine || !impl->opened || impl->finished || !impl->videoHwnd || !impl->parentHwnd) return;

    double currentTime = impl->engine->GetCurrentTime();
    double duration = impl->engine->GetDuration();

    // Ensure the child window follows the parent window size dynamically
    RECT parentRect;
    GetClientRect(impl->parentHwnd, &parentRect);
    int pW = parentRect.right - parentRect.left;
    int pH = parentRect.bottom - parentRect.top;
    SetWindowPos(impl->videoHwnd, HWND_TOP, 0, 0, pW, pH, SWP_NOACTIVATE);

    // Preserve aspect ratio and fit the video into the child window
    DWORD nativeW = 0, nativeH = 0;
    if (SUCCEEDED(impl->engine->GetNativeVideoSize(&nativeW, &nativeH)) && nativeW > 0 && nativeH > 0) {
        float videoAspect = (float)nativeW / (float)nativeH;
        float windowAspect = (float)pW / (float)pH;

        RECT destRect;
        if (videoAspect > windowAspect) {
            // Video is wider than window: letterbox
            int destH = (int)(pW / videoAspect);
            int top = (pH - destH) / 2;
            destRect.left = 0;
            destRect.right = pW;
            destRect.top = top;
            destRect.bottom = top + destH;
        } else {
            // Video is taller than window: pillarbox
            int destW = (int)(pH * videoAspect);
            int left = (pW - destW) / 2;
            destRect.left = left;
            destRect.right = left + destW;
            destRect.top = 0;
            destRect.bottom = pH;
        }

        IMFMediaEngineEx* engineEx = nullptr;
        if (SUCCEEDED(impl->engine->QueryInterface(IID_PPV_ARGS(&engineEx)))) {
            MFVideoNormalizedRect sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
            MFARGB borderColor = { 0, 0, 0, 255 }; // Solid black borders
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
    impl->close();
    HWND hwnd = FindWindowA(nullptr, "Dragon Asteroid Run");
    if (hwnd) {
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
    }
}
