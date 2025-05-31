#include <windows.h>
#include <d3d11.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_theme.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "d3d11.lib")

// --- Clamp helper for C++14/11 compatibility ---
template<typename T>
T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

HWND g_hWnd = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
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
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    ID3D11Texture2D* pBackBuffer = NULL;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
    return true;
}

void CleanupDeviceD3D() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer = NULL;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

ID3D11ShaderResourceView* LoadTextureFromFile(const char* filename, int* out_width, int* out_height, ID3D11Device* device)
{
    int image_width = 0;
    int image_height = 0;
    int channels = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &channels, 4);
    if (image_data == NULL)
        return NULL;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    ID3D11Texture2D* pTexture = NULL;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &pTexture);

    if (FAILED(hr)) {
        stbi_image_free(image_data);
        return NULL;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    ID3D11ShaderResourceView* out_srv = NULL;
    hr = device->CreateShaderResourceView(pTexture, &srvDesc, &out_srv);
    pTexture->Release();

    stbi_image_free(image_data);

    *out_width = image_width;
    *out_height = image_height;

    return out_srv;
}

struct SidebarTab {
    std::string name;
    std::vector<std::string> subs;
    bool open = false;
    int selectedSub = 0;
    float anim_progress = 0.0f; // For dropdown animation
};
std::vector<SidebarTab> sidebarTabs = {
    {"Account", {"Info", "Notes"}, false, 0, 0.0f},
    {"Settings", {"General", "Security"}, false, 0, 0.0f}
};
int selectedMainTab = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Overlay", NULL };
    RegisterClassEx(&wc);

    RECT rc;
    GetClientRect(GetDesktopWindow(), &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        wc.lpszClassName, L"ImGui Overlay",
        WS_POPUP, 0, 0, width, height,
        NULL, NULL, wc.hInstance, NULL);

    SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);

    if (!CreateDeviceD3D(g_hWnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    SetModernImGuiStyle();

    ImFont* logoFont = io.Fonts->AddFontFromFileTTF(
        "C:\\Users\\scemmaz\\Desktop\\VELORA\\VELORA\\resources\\fonts\\OvercameDemoBold.ttf", 38.0f
    );
    if (!logoFont) {
        MessageBoxA(NULL, "Could not load OvercameDemoBold.ttf!", "Font Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ImFont* mainFont = io.Fonts->AddFontFromFileTTF(
        "C:\\Users\\scemmaz\\Desktop\\VELORA\\VELORA\\resources\\fonts\\Unageo-Light.ttf", 18.0f
    );
    if (!mainFont) {
        MessageBoxA(NULL, "Could not load Unageo-Light.ttf!", "Font Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ImFont* bottombarFont = io.Fonts->AddFontFromFileTTF(
        "C:\\Users\\scemmaz\\Desktop\\VELORA\\VELORA\\resources\\fonts\\Unageo-Light.ttf", 16.0f
    );

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 customBg = ImVec4(0.098f, 0.098f, 0.114f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = customBg;
    style.Colors[ImGuiCol_ChildBg] = customBg;
    ImVec4 borderColor = ImVec4(32.0f / 255.0f, 32.0f / 255.0f, 38.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_Border] = borderColor;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    style.WindowBorderSize = 1.0f;

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    float overlay_alpha = 0.0f;
    const float overlay_fade_duration = 0.6f;
    double overlay_start_time = ImGui::GetTime();

    ImVec2 mainSize = ImVec2(700, 440);
    ImVec2 sidebarSize = ImVec2(220, 440);
    const float sidebarPadding = 10.0f;
    const float topbarHeight = 40.0f;
    const float bottombarHeight = 40.0f;
    ImVec2 mainPos = ImVec2(width / 2 - mainSize.x / 2 + sidebarSize.x / 2, height / 2 - mainSize.y / 2);

    ImVec4 barColor = ImVec4(0.114f, 0.114f, 0.133f, 1.0f);

    ImVec4 glowPurple = ImVec4(168.0f / 255.0f, 99.0f / 255.0f, 255.0f / 255.0f, 1.0f);

    int settingsIconW = 0, settingsIconH = 0;
    ID3D11ShaderResourceView* settingsIconSRV = LoadTextureFromFile(
        "C:\\Users\\scemmaz\\Desktop\\VELORA\\VELORA\\resources\\icons\\settings.png",
        &settingsIconW, &settingsIconH, g_pd3dDevice
    );
    if (!settingsIconSRV) {
        MessageBoxA(NULL, "Could not load settings.png!", "Icon Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    const char* todayStr = "2025-05-31";

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    bool running = true;
    bool firstFrame = true;
    bool dragging = false;
    ImVec2 dragOffset = ImVec2(0, 0);

    // Animation config
    const float dropdown_speed = 8.0f;
    const float subtab_height = 32.0f;

    while (running) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }
        if (!running) break;

        if (overlay_alpha < 1.0f) {
            double now = ImGui::GetTime();
            overlay_alpha = float((now - overlay_start_time) / overlay_fade_duration);
            if (overlay_alpha > 1.0f) overlay_alpha = 1.0f;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Animate dropdowns
        float dt = ImGui::GetIO().DeltaTime;
        for (auto& tab : sidebarTabs) {
            float target = tab.open ? 1.0f : 0.0f;
            if (tab.anim_progress != target) {
                tab.anim_progress += (target - tab.anim_progress) * clamp(dt * dropdown_speed, 0.0f, 1.0f);
                if (std::abs(tab.anim_progress - target) < 0.01f)
                    tab.anim_progress = target;
            }
        }

        // --- Manual drag for the main panel and sidebar ---
        ImGui::SetNextWindowPos(mainPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(mainSize, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(overlay_alpha);
        ImGui::PushFont(mainFont);
        ImGui::Begin("MainPanel", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);

        // --- Topbar background and settings button ---
        {
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 winSize = ImGui::GetWindowSize();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            draw_list->AddRectFilled(
                ImVec2(winPos.x, winPos.y),
                ImVec2(winPos.x + winSize.x, winPos.y + topbarHeight),
                ImGui::ColorConvertFloat4ToU32(barColor),
                0.0f
            );
            draw_list->AddRect(
                ImVec2(winPos.x, winPos.y),
                ImVec2(winPos.x + winSize.x, winPos.y + topbarHeight),
                ImGui::ColorConvertFloat4ToU32(borderColor),
                0.0f,
                0,
                style.WindowBorderSize
            );

            // --- DRAGGING HANDLER ---
            ImGui::SetCursorScreenPos(winPos);
            ImGui::InvisibleButton("##dragMainWin", ImVec2(winSize.x - 40.0f, topbarHeight));
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                if (!dragging) {
                    dragging = true;
                    dragOffset = ImVec2(ImGui::GetMousePos().x - winPos.x, ImGui::GetMousePos().y - winPos.y);
                }
                mainPos = ImVec2(ImGui::GetMousePos().x - dragOffset.x, ImGui::GetMousePos().y - dragOffset.y);
            }
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                dragging = false;

            float btnSize = 40.0f;
            float iconSize = 16.0f; // Icon size in button
            ImVec2 btnPos = ImVec2(winPos.x + winSize.x - btnSize, winPos.y);
            ImGui::SetCursorScreenPos(btnPos);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, barColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, barColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, barColor);

            ImGui::Button("##settings", ImVec2(btnSize, btnSize));
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);

            draw_list->AddRect(btnPos, ImVec2(btnPos.x + btnSize, btnPos.y + btnSize),
                ImGui::ColorConvertFloat4ToU32(borderColor), 0.0f, 0, style.WindowBorderSize);

            ImVec2 iconPos = ImVec2(
                btnPos.x + (btnSize - iconSize) * 0.5f,
                btnPos.y + (btnSize - iconSize) * 0.5f
            );
            draw_list->AddImage(
                reinterpret_cast<ImTextureID>(settingsIconSRV),
                iconPos,
                ImVec2(iconPos.x + iconSize, iconPos.y + iconSize)
            );

            ImGui::SetCursorScreenPos(ImVec2(winPos.x, winPos.y + topbarHeight));
            ImGui::Dummy(ImVec2(winSize.x, topbarHeight));
        }

        // --- MainPanel Content Area ---
        ImGui::BeginChild("ContentArea", ImVec2(0, -1), false);
        {
            SidebarTab& activeTab = sidebarTabs[selectedMainTab];
            int activeSub = activeTab.selectedSub;

            ImGui::Text("%s > %s", activeTab.name.c_str(), activeTab.subs[activeSub].c_str());
            ImGui::Separator();

            if (activeTab.name == "Account") {
                if (activeSub == 0) {
                    ImGui::Text("This is your account info. (You can add real content here.)");
                }
                else if (activeSub == 1) {
                    ImGui::Text("These are your account notes. (You can add real content here.)");
                }
            }
            else if (activeTab.name == "Settings") {
                if (activeSub == 0) {
                    ImGui::Text("General settings shown here.");
                }
                else if (activeSub == 1) {
                    ImGui::Text("Security settings for your account.");
                }
            }
        }
        ImGui::EndChild();

        ImGui::End(); // MainPanel
        ImGui::PopFont();

        // --- Sidebar with animated dropdown tabs ---
        ImVec2 sidebarTargetPos = ImVec2(mainPos.x - sidebarSize.x - sidebarPadding, mainPos.y);
        ImGui::SetNextWindowPos(sidebarTargetPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(sidebarSize, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(overlay_alpha);
        ImGui::PushFont(mainFont);
        ImGui::Begin("Sidebar", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);

        // --- Sidebar Topbar with Logo ---
        {
            ImVec2 sbarWinPos = ImGui::GetWindowPos();
            ImVec2 sbarWinSize = ImGui::GetWindowSize();
            ImDrawList* sbar_draw_list = ImGui::GetWindowDrawList();

            sbar_draw_list->AddRectFilled(
                ImVec2(sbarWinPos.x, sbarWinPos.y),
                ImVec2(sbarWinPos.x + sbarWinSize.x, sbarWinPos.y + topbarHeight),
                ImGui::ColorConvertFloat4ToU32(barColor),
                style.WindowRounding,
                ImDrawFlags_RoundCornersTop
            );
            sbar_draw_list->AddRect(
                ImVec2(sbarWinPos.x, sbarWinPos.y),
                ImVec2(sbarWinPos.x + sbarWinSize.x, sbarWinPos.y + topbarHeight),
                ImGui::ColorConvertFloat4ToU32(borderColor),
                style.WindowRounding,
                ImDrawFlags_RoundCornersTop,
                style.WindowBorderSize
            );

            float fontSize = 38.0f;
            ImFont* font = logoFont;
            const char* letters = "VELORA";
            int letterCount = 6;
            float letterSpacing = 4.0f;

            float totalWidth = 0.0f;
            float maxLetterHeight = 0.0f;
            ImVec2 letterSizes[6];
            for (int i = 0; i < letterCount; ++i) {
                letterSizes[i] = font->CalcTextSizeA(fontSize, FLT_MAX, 0, &letters[i], &letters[i + 1]);
                totalWidth += letterSizes[i].x;
                if (letterSizes[i].y > maxLetterHeight) maxLetterHeight = letterSizes[i].y;
                if (i < letterCount - 1) totalWidth += letterSpacing;
            }

            float startX = sbarWinPos.x + (sbarWinSize.x - totalWidth) * 0.5f;
            float ascent = font->Ascent * fontSize / font->FontSize;
            float textY = sbarWinPos.y + (topbarHeight + ascent - maxLetterHeight) * 0.5f;

            ImGui::PushFont(font);
            float x = startX;
            for (int i = 0; i < letterCount; ++i) {
                char c[2] = { letters[i], 0 };
                ImU32 color = (i == 0)
                    ? ImGui::ColorConvertFloat4ToU32(glowPurple)
                    : ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1));
                sbar_draw_list->AddText(font, fontSize, ImVec2(x, textY), color, c);
                x += letterSizes[i].x + letterSpacing;
            }
            ImGui::PopFont();
            ImGui::Dummy(ImVec2(sbarWinSize.x, topbarHeight));
        }

        // --- Sidebar Tabs with Animated Dropdowns ---
        ImGui::Spacing();
        for (size_t i = 0; i < sidebarTabs.size(); ++i) {
            SidebarTab& tab = sidebarTabs[i];
            ImGui::PushID((int)i);
            if (ImGui::Button(tab.name.c_str(), ImVec2(-FLT_MIN, 32))) {
                bool wasOpen = tab.open;
                // Close all, then open the clicked one if it wasn't open already
                for (auto& t : sidebarTabs) t.open = false;
                tab.open = !wasOpen;
                if (tab.open) selectedMainTab = (int)i;
            }

            float anim = tab.anim_progress;
            if (anim > 0.0f) {
                float total_height = subtab_height * tab.subs.size();
                float shown_height = total_height * anim;
                ImGui::Indent(12.0f);
                ImGui::BeginChild("dropdown", ImVec2(-FLT_MIN, shown_height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                int visible_items = clamp(int(tab.subs.size() * anim + 0.999f), 0, (int)tab.subs.size());
                for (int sub = 0; sub < visible_items; ++sub) {
                    bool isSelected = (tab.selectedSub == sub);
                    if (ImGui::Selectable(tab.subs[sub].c_str(), isSelected, 0, ImVec2(-FLT_MIN, subtab_height))) {
                        tab.selectedSub = sub;
                        selectedMainTab = (int)i;
                    }
                }
                ImGui::EndChild();
                ImGui::Unindent(12.0f);
            }
            ImGui::PopID();
            ImGui::Spacing();
        }

        // --- Sidebar Bottombar ---
        {
            ImVec2 sbarWinPos = ImGui::GetWindowPos();
            ImVec2 sbarWinSize = ImGui::GetWindowSize();
            ImDrawList* sbar_draw_list = ImGui::GetWindowDrawList();

            float y_bottombar = sbarWinPos.y + sbarWinSize.y - bottombarHeight;
            sbar_draw_list->AddRectFilled(
                ImVec2(sbarWinPos.x, y_bottombar),
                ImVec2(sbarWinPos.x + sbarWinSize.x, sbarWinPos.y + sbarWinSize.y),
                ImGui::ColorConvertFloat4ToU32(barColor),
                style.WindowRounding,
                ImDrawFlags_RoundCornersBottom
            );
            sbar_draw_list->AddRect(
                ImVec2(sbarWinPos.x, y_bottombar),
                ImVec2(sbarWinPos.x + sbarWinSize.x, sbarWinPos.y + sbarWinSize.y),
                ImGui::ColorConvertFloat4ToU32(borderColor),
                style.WindowRounding,
                ImDrawFlags_RoundCornersBottom,
                style.WindowBorderSize
            );

            float nameFontSize = 16.0f;
            float dateFontSize = 13.0f;
            ImFont* font = bottombarFont;

            const char* leftText = u8"Virex ©";
            const char* rightText = todayStr;

            ImU32 leftColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0xF1 / 255.0f, 0xF1 / 255.0f, 0xF1 / 255.0f, 1.0f));
            ImU32 rightColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0x52 / 255.0f, 0x52 / 255.0f, 0x5E / 255.0f, 1.0f));

            ImGui::PushFont(font);
            ImVec2 leftSize = font->CalcTextSizeA(nameFontSize, FLT_MAX, 0, leftText);
            ImVec2 rightSize = font->CalcTextSizeA(dateFontSize, FLT_MAX, 0, rightText);
            float leftY = y_bottombar + (bottombarHeight - leftSize.y) * 0.5f;
            float rightY = y_bottombar + (bottombarHeight - rightSize.y) * 0.5f;
            float leftX = sbarWinPos.x + 16.0f;
            float rightX = sbarWinPos.x + sbarWinSize.x - rightSize.x - 16.0f;

            sbar_draw_list->AddText(font, nameFontSize, ImVec2(leftX, leftY), leftColor, leftText);
            sbar_draw_list->AddText(font, dateFontSize, ImVec2(rightX, rightY), rightColor, rightText);
            ImGui::PopFont();

            ImGui::SetCursorPosY(sbarWinSize.y - bottombarHeight);
            ImGui::Dummy(ImVec2(sbarWinSize.x, bottombarHeight));
        }

        ImGui::End(); // Sidebar
        ImGui::PopFont();

        firstFrame = false;

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    if (settingsIconSRV) settingsIconSRV->Release();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(g_hWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}