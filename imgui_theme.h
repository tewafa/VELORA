#pragma once
#include "imgui.h"

inline void SetModernImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Main window background color : #19191D
    colors[ImGuiCol_WindowBg] = ImVec4(0.098f, 0.098f, 0.114f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.098f, 0.098f, 0.114f, 1.0f);

    // Border color : #23232A
    colors[ImGuiCol_Border] = ImVec4(0.137f, 0.137f, 0.164f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    // Sidebar Button backgrounds: transparent (not active), border color (active)
    colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0.0f); // transparent
    colors[ImGuiCol_ButtonHovered] = ImVec4(0, 0, 0, 0.0f); // transparent
    colors[ImGuiCol_ButtonActive] = ImVec4(0.137f, 0.137f, 0.164f, 1.0f); // #23232A

    // Text color: default #52525E, active #E3E3E3 handled in UI
    colors[ImGuiCol_Text] = ImVec4(0.32f, 0.32f, 0.37f, 1.0f); // #52525E
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.60f, 1.0f);

    colors[ImGuiCol_Header] = ImVec4(0, 0, 0, 0.0f); // normal background
    colors[ImGuiCol_HeaderHovered] = ImVec4(0, 0, 0, 0.0f); // hover background
    colors[ImGuiCol_HeaderActive] = ImVec4(0, 0, 0, 0.0f); // selected/active background

    // No border on buttons
    style.FrameBorderSize = 0.0f;

    // ...rest as before...
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.17f, 0.23f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.23f, 0.32f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.16f, 0.24f, 0.36f, 1.00f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.13f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.17f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.25f, 0.33f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.32f, 0.42f, 1.00f);



    style.WindowRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.TabRounding = 7.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f; // No border on buttons
    style.PopupBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(12, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(24, 20);
    style.FramePadding = ImVec2(12, 8);
}
