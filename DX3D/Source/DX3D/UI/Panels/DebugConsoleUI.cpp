#include <DX3D/UI/Panels/DebugConsoleUI.h>
#include <DX3D/Core/Logger.h>
#include <imgui.h>

using namespace dx3d;

DebugConsoleUI::DebugConsoleUI(Logger& logger)
    : m_logger(logger)
{
}

void DebugConsoleUI::render()
{
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;
    float halfWidth = windowWidth * 0.5f;
    float halfHeight = windowHeight * 0.5f;
    float debugHeight = halfHeight * 0.4f;

    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 120));

    ImGui::Begin("Debug Console", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto logEntries = m_logger.getRecentLogs(100);

    if (ImGui::Button("Clear Logs"))
    {
        m_logger.clearLogs();
    }

    ImGui::SameLine();
    ImGui::Text("Log Entries: %zu", logEntries.size());

    ImGui::Separator();

    ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& entry : logEntries)
    {
        ImVec4 color;
        const char* levelText;

        switch (entry.level)
        {
        case LogEntry::Level::Error:
            color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
            levelText = "[DG ERROR]";
            break;
        case LogEntry::Level::Warning:
            color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            levelText = "[DG WARNING]";
            break;
        case LogEntry::Level::Info:
            color = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
            levelText = "[DB INFO]";
            break;
        default:
            color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            levelText = "[DB LOG]";
            break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("%s %s %s", entry.timestamp.c_str(), levelText, entry.message.c_str());
        ImGui::PopStyleColor();
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
}