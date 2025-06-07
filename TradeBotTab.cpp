#include "TradeBotTab.h"

void RenderTradeBotTab(int activeSub) {
    if (activeSub == 0) {
        ImGui::Text("Trade Bot Overview goes here.");
    }
    else if (activeSub == 1) {
        ImGui::Text("Trade Bot History goes here.");
    }
    else if (activeSub == 2) {
        ImGui::Text("Trade Bot Config goes here.");
    }
}