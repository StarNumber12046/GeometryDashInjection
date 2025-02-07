#include "gui.h"
#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include "util.h"
#include "memory.h"

bool NoClipEnabled = getNoClipEnabled();
bool IconBypassEnabled = getiBypassEnabled();
bool ColorBypassEnabled = getcBypassEnabled();
static bool shown = false;

void drawImGui() {
    ImGui::Begin("Player");
    ImGui::Checkbox("NoClip", &NoClipEnabled);
    ImGui::End();
    ImGui::Begin("Bypass");
    ImGui::Checkbox("Icon Bypass", &IconBypassEnabled);
    ImGui::Checkbox("Color Bypass", &ColorBypassEnabled);
    ImGui::End();
}

static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



// Main code
int createWindow()
{
    DWORD PID = util::FindProcessId(L"GeometryDash.exe");
    printf("PID: %d\n", PID);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return 1;
    }


    

    RECT rect;
    if (!GetWindowRect(util::FindMainWindow(PID), &rect)) {
        std::cerr << "Error getting rect: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "Game Window Rect: "
        << "Left: " << rect.left << ", "
        << "Top: " << rect.top << ", "
        << "Right: " << rect.right << ", "
        << "Bottom: " << rect.bottom << std::endl;


    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Geometry Dash External", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(
        WS_EX_LAYERED,  // Extended style for layered window
        wc.lpszClassName,
        L"Geometry Dash",
        WS_POPUP,
        rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_DeviceLost) {
            HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST) {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET)
                ResetDevice();
            g_DeviceLost = false;
        }

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);


        if (g_pd3dDevice->BeginScene() >= 0) {
            if ((GetKeyState(VK_RSHIFT) & 0x8000) && !get_RSHIFT_PRESSED()) {
                if (!shown) {
                    ShowWindow(hwnd, SW_SHOWDEFAULT);
                    shown = true;
                } else {
                    ShowWindow(hwnd, SW_HIDE);
                    shown = false;
                }
            }
            set_RSHIFT((bool)(GetKeyState(VK_RSHIFT) & 0x8000));

            drawImGui();
            toggleColorBypass(ColorBypassEnabled);
            toggleNoClipEnabled(NoClipEnabled);
            toggleIconBypass(IconBypassEnabled);

            ImGui::EndFrame();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST)
            g_DeviceLost = true;
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;  // Use a format with alpha
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;  // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void CleanupDeviceD3D() {
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
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
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
