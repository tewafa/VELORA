#include "SniperTab.h"

void RenderSniperTab(int activeSub) {
    if (activeSub == 0) {
        ImGui::Text("Sniper Panel goes here.");
    }
    else if (activeSub == 1) {
        ImGui::Text("Sniper Config goes here.");
    }
}