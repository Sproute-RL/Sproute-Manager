#pragma once
#include <imgui.h>

namespace ImGui
{
    inline void HelpMarker(const char *desc)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    inline void TextCentered(const char *text)
    {
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
        ImGui::Text(text);
    }
    inline void MakeWindowThingy(float offsetX, float offsetY)
    {
        ImVec2 availableSize = ImGui::GetWindowSize();

        ImVec2 windowPos = ImVec2((availableSize.x - availableSize.x / offsetX) / 2,
                                  (availableSize.y - availableSize.y / offsetY) / 2);

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(availableSize.x / offsetX, availableSize.y / offsetY), ImGuiCond_Once);
    }
    inline void MakeCurrentWindowThingy(float offsetX, float offsetY)
    {
        ImVec2 availableSize = ImGui::GetWindowSize();

        ImVec2 windowPos = ImVec2((availableSize.x - availableSize.x / offsetX) / 2,
                                  (availableSize.y - availableSize.y / offsetY) / 2);

        ImGui::SetWindowPos(windowPos, ImGuiCond_Once);
        ImGui::SetWindowSize(ImVec2(availableSize.x / offsetX, availableSize.y / offsetY), ImGuiCond_Once);
    }

    enum class Position
    {
        Top,
        Bottom,
    };

    inline void PositionizeItem(Position position, float width)
    {
        ImVec2 availSize = ImGui::GetContentRegionAvail(); // Available content region

        // Center horizontally
        ImGui::SetCursorPosX((availSize.x - width) / 2.0f);

        // Position vertically based on the enum
        if (position == Position::Top)
        {
            ImGui::SetCursorPosY(0); // Align at the top
        }
        else if (position == Position::Bottom)
        {
            float buttonHeight = ImGui::GetFrameHeight();     // Height of the button
            ImGui::SetCursorPosY(availSize.y - buttonHeight); // Align at the bottom
        }

        // Set the width of the next item
        ImGui::SetNextItemWidth(width);
    }

}