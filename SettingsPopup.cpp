#include "SettingsPopup.h"

bool showSettings = false;

void DrawSettingsButtonAndPopup(
    ImVec2 winPos, ImVec2 winSize, float btnSize, float iconSize,
    ImTextureID settingsIconSRV, ImVec4 barColor)
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
    if (clicked) showSettings = !showSettings;

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

    if (showSettings) {
        float popupWidth = 240.0f, popupHeight = 200.0f;
        float padding = 14.0f;
        ImVec2 popupPos = ImVec2(winPos.x + winSize.x - popupWidth - padding, winPos.y + 40.0f + padding);

        ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        ImGui::Begin("##SettingsPanel",
            &showSettings,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

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
    }
}
