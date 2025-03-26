// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <thread>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "shinobuwidget.h"
#include "./implot/implot.h"
#include "./cubism_src/LAppDelegate.hpp"

static ID3D11Device*            g_pd3dDevice            = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext     = nullptr;
static IDXGISwapChain*          g_pSwapChain            = nullptr;
static bool                     g_SwapChainOccluded     = false;
static UINT                     g_ResizeWidth           = 0;
static UINT                     g_ResizeHeight          = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView  = nullptr;
static bool                     while_done              = false;


bool            CreateDeviceD3D(HWND hWnd);
void            CleanupDeviceD3D();
void            CreateRenderTarget();
void            CleanupRenderTarget();
void            FrontPart();
void            PosteriorPart();
void            QuitHandle();
LRESULT WINAPI  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI    CubismThread(LPVOID lpParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //---------------------------------------------------开始的处理
    // cout 输出重定向
    std::cout.rdbuf(GlobalTemp::DebugOss.rdbuf());
    // 初始化时间种子
    srand((unsigned)time(NULL));
    //---------------------------------------------------前部分
    FrontPart();
    //---------------------------------------------------中间部分
    // 全局初始化
    ::GlobalConfig::getInstance();
    // 初始化用户配置
    Su::AllConfigInit();

    DWORD cubismThreadId;
    CreateThread(NULL, 0, CubismThread, NULL, 0, &cubismThreadId);

    std::cout << u8"Cubism 进程ID" << cubismThreadId << std::endl;
    // Cubism 运行后可继续
    while (!GlobalTemp::CubismIsRunning);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    MSG msg;
    std::cout << u8"全局开始执行" << std::endl;
    do{
        if (while_done == true)break;
        //高性能绘制
        if (GlobalConfig::getInstance()->window_main_fast_id==0) {
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)){
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    QuitHandle();
            }
        }
        //节能绘制
        else {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) 
                QuitHandle();
        }
        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;
        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        //WINDOWS BEGIN--------------------------------------------------------------------------
        //Demo Window
        static bool show_demo_window = true;
        if (show_demo_window)ImGui::ShowDemoWindow(&show_demo_window);

        //MAIN WINDOWS
        static bool show_shinobu_window = true;
        ShowShinobuWindow(&show_shinobu_window);
        ShowShinobuStyleEditor();
        ShowShinobuInteractively();
        ShowShinobuErrorWindow();//Error Window
        ShowShinobuDebugWindow();//Debug Window
        ShowShinobuPlot();

        //WINDOWS END--------------------------------------------------------------------------
        if (show_shinobu_window == false)QuitHandle();
        //限制帧率
        if (ImGui::GetIO().Framerate > GlobalConfig::getInstance()->window_main_forecastfps)
            std::this_thread::sleep_for(std::chrono::milliseconds(GlobalConfig::getInstance()->window_main_addtimefps));
        // Rendering
        ImGui::Render();
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        static const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }while (GlobalConfig::getInstance()->window_main_fast_id==0 ||GetMessage(&msg, NULL, 0, 0));

    //---------------------------------------------------后部分
    PosteriorPart();

    std::cout << u8"全局终了" << std::endl;
    return 0;
}
void QuitHandle() {
    while_done = true;
    std::cout << u8"全局退出消息" << std::endl;
    LAppDelegate::GetInstance()->AppEnd();
    std::cout << u8"Cubism 退出" << std::endl;
    ::GlobalConfig::GlobalConfigSave();
    std::cout << u8"退出时自动保存全局配置" << std::endl;
}
// Cubism Thread
DWORD WINAPI CubismThread(LPVOID lpParam)
{
    // 初始化Cubism进程
    if (!LAppDelegate::GetInstance()->Initialize()) {
        //初始Cubism进程失败
        LAppDelegate::GetInstance()->Release();
        LAppDelegate::ReleaseInstance();
    }
    else {
        // 初始化Cubism模型
        std::vector<Su::UserConfig>& vu = Su::UserConfig::getUserVector();
        for (int i = 0; i < int(vu.size()); ++i) 
            if (vu[i].enable_cubism) 
                GlobalTemp::CubismModelMessage.push(std::make_pair(vu[i].user_id, vu[i].cubism_config.model_dir));
        std::cout << u8"Cubism 开始执行" << std::endl;
        GlobalTemp::CubismIsRunning = true;
        LAppDelegate::GetInstance()->Run();
    }
    std::cout << u8"Cubism 终了" << std::endl;
    return 0;
}


void FrontPart() {
    // 全局UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // 调用这个函数解决字体发虚的问题
    ImGui_ImplWin32_EnableDpiAwareness();
    // DPI 感知启用,防止Cubism界面模糊
    SetProcessDPIAware();

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Shinobu Example", nullptr };
    ::RegisterClassExW(&wc);
    static int currentX = 0, currentY = 0, resWidth = GetSystemMetrics(SM_CXSCREEN), resHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear Shinobu", WS_OVERLAPPEDWINDOW, currentX, currentY, resWidth, resHeight, nullptr, nullptr, wc.hInstance, nullptr);
    GlobalTemp::WindowMainWc = wc;
    GlobalTemp::WindowMainHandle = hwnd;
    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }
    ::ShowWindow(hwnd, SW_HIDE);
    ::UpdateWindow(hwnd);
    // Setup context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // 启用停靠
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;
    io.ConfigDockingTransparentPayload = true;                // 停靠时透明

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // 设置平台、渲染后端
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}
void PosteriorPart() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(GlobalTemp::WindowMainHandle);
    ::UnregisterClassW(GlobalTemp::WindowMainWc.lpszClassName, GlobalTemp::WindowMainWc.hInstance);
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer != nullptr) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

