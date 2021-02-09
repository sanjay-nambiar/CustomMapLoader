#include "CustomMapSelectionUI.h"

#include "imgui.h"

void CustomMapSelectionUI::SetImGuiContext(std::uintptr_t aContext)
{
	ImGuiContext* context = reinterpret_cast<ImGuiContext*>(aContext);
	ImGui::SetCurrentContext(context);
}

void CustomMapSelectionUI::OnOpen()
{
}

void CustomMapSelectionUI::OnClose()
{
}

void CustomMapSelectionUI::Render()
{
}
