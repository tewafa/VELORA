#pragma once
#include "imgui.h"

inline void SetModernImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Opaque colors for UI, window background included!
    colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.10f, 0.13f, 1.00f); // Opaque
    colors[ImGuiCol_Border] = ImVec4(0.13f, 0.20f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.39f, 0.68f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.22f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.17f, 0.23f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.23f, 0.32f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.16f, 0.24f, 0.36f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.94f, 0.95f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.60f, 1.00f);
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
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(12, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(24, 20);
    style.FramePadding = ImVec2(12, 8);
}