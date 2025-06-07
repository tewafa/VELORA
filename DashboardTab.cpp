#include "DashboardTab.h"
#include "imgui.h"

void RenderDashboardTab(int activeSub) {
    if (activeSub == 0) {
        ImGui::Text("Dashboard Overview goes here.");
    }
    else if (activeSub == 1) {
        ImGui::Text("Dashboard Logs go here.");
    }
}
