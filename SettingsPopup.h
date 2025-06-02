#include <windows.h>
#include "imgui.h"

extern HWND g_hWnd;

static bool showSettings = false;
static float settingsFade = 0.0f;
static double fadeStart = 0.0;
static bool lastShowSettings = false;

inline void DrawSettingsButtonAndPopup(
    ImVec2 winPos, ImVec2 winSize, float btnSize, float iconSize, ImTextureID settingsIconSRV, ImVec4 barColor)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 btnPos = ImVec2(winPos.x + winSize.x - btnSize, winPos.y);

    ImGui::SetCursorScreenPos(btnPos);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, barColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, barColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, barColor);

    bool clicked = ImGui::Button("##settings", ImVec2(btnSize, btnSize));
    if (clicked) {
        showSettings = !showSettings;
        fadeStart = ImGui::GetTime();
        lastShowSettings = showSettings;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    // Draw icon over button
    ImVec2 iconPos = ImVec2(
        btnPos.x + (btnSize - iconSize) * 0.5f,
        btnPos.y + (btnSize - iconSize) * 0.5f
    );
    draw_list->AddImage(
        settingsIconSRV,
        iconPos,
        ImVec2(iconPos.x + iconSize, iconPos.y + iconSize)
    );

    // Draw button border
    draw_list->AddRect(btnPos, ImVec2(btnPos.x + btnSize, btnPos.y + btnSize),
        ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Border]), 0.0f, 0, ImGui::GetStyle().WindowBorderSize);

    // --- Fade animation logic ---
    const float fadeDuration = 0.25f;

    if (showSettings && settingsFade < 1.0f) {
        float t = float(ImGui::GetTime() - fadeStart) / fadeDuration;
        settingsFade = (t > 1.0f) ? 1.0f : t;
    }
    if (!showSettings && settingsFade > 0.0f) {
        float t = float(ImGui::GetTime() - fadeStart) / fadeDuration;
        settingsFade = 1.0f - ((t > 1.0f) ? 1.0f : t);
    }

    // Only draw popup if visible at all
    if (settingsFade > 0.01f) {
        float popupWidth = 240.0f, popupHeight = 200.0f;
        float padding = 14.0f;
        ImVec2 popupPos = ImVec2(winPos.x + winSize.x - popupWidth - padding, winPos.y + 40.0f + padding);

        ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(settingsFade * 1.0f); // fade alpha

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
        if (settingsFade < 0.98f)
            windowFlags |= ImGuiWindowFlags_NoInputs;

        ImGui::Begin("##SettingsPanel", nullptr, windowFlags);

        ImGui::Text("Settings");
        ImGui::Separator();
        ImGui::Spacing();

        float buttonHeight = 36.0f;
        float spaceToBottom = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - buttonHeight - 8.0f;
        if (spaceToBottom > 0)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spaceToBottom);

        float buttonWidth = ImGui::GetContentRegionAvail().x;
        if (ImGui::Button("End Session", ImVec2(buttonWidth, buttonHeight))) {
            PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        }

        ImGui::End();
        ImGui::PopStyleVar(3);

        // Close if fully faded in and click outside
        if (settingsFade > 0.98f && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0)) {
            showSettings = false;
            fadeStart = ImGui::GetTime();
            lastShowSettings = false;
        }
    }
}