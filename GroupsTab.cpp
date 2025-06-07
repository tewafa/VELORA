#include "GroupsTab.h"

void RenderGroupsTab(int activeSub, const std::string& groupName) {
    ImGui::Text("Group: %s", groupName.c_str());
}
