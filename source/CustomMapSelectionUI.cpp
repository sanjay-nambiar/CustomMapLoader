#include "CustomMapSelectionUI.h"

#include "imgui.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <windows.h>

namespace CustomMapSelectionUI_private
{
	static std::string locMenuName = "custom_map_loader";
}

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

void CustomMapSelectionUI::SetTitle(const std::string& aTitle)
{
	myTitle = aTitle;
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

std::string CustomMapSelectionUI::GetMenuName()
{
	return CustomMapSelectionUI_private::locMenuName;
}

std::string CustomMapSelectionUI::GetMenuTitle()
{
	return myTitle;
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
