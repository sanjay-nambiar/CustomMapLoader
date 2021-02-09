#include "CustomMapSelectionUI.h"

#include "imgui.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

CustomMapSelectionUI::CustomMapSelectionUI(CustomMapLoader& aMapLoader)
	: myIsWindowOpen(false)
	, myMapLoader(aMapLoader)
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
	RenderCustomMapSelectionWidget();
}

bool CustomMapSelectionUI::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool CustomMapSelectionUI::IsActiveOverlay()
{
	return true;
}

void CustomMapSelectionUI::RenderCustomMapSelectionWidget()
{
}
