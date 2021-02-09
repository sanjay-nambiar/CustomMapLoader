#include "CustomMapSelectionUI.h"

#include "imgui.h"

CustomMapSelectionUI::CustomMapSelectionUI()
	: myIsWindowOpen(false)
{
}

void CustomMapSelectionUI::SetImGuiContext(std::uintptr_t aContext)
{
	ImGuiContext* context = reinterpret_cast<ImGuiContext*>(aContext);
	ImGui::SetCurrentContext(context);
}

void CustomMapSelectionUI::OnOpen()
{
	myIsWindowOpen = true;
}

void CustomMapSelectionUI::OnClose()
{
	myIsWindowOpen = false;
}

void CustomMapSelectionUI::Render()
{
}

bool CustomMapSelectionUI::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool CustomMapSelectionUI::IsActiveOverlay()
{
	return true;
}
