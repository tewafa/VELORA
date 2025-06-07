#pragma once
#include "imgui.h"
#include <d3d11.h>
#include <windows.h>

extern bool showSettings;
extern HWND g_hWnd;

void DrawSettingsButtonAndPopup(
    ImVec2 winPos, ImVec2 winSize, float btnSize, float iconSize,
    ImTextureID settingsIconSRV, ImVec4 barColor);
