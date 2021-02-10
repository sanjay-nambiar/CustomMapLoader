#include "CustomMapSelectionUI.h"

#include "imgui.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <windows.h>

namespace CustomMapSelectionUI_private
{
	static std::string locMenuName = "custom_map_loader";
	static float locWindowWidth = 800.f;
	static float locWindowHeight = 600.f;
}

CustomMapSelectionUI::CustomMapSelectionUI(CustomMapLoader& aMapLoader)
	: myIsWindowOpen(false)
	, myMapLoader(aMapLoader)
{
}

void CustomMapSelectionUI::SetImGuiContext(std::uintptr_t aContext)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(aContext));
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

bool CustomMapSelectionUI::Render()
{
	if (!IMGUI_CHECKVERSION())
		return false;

	ImGui::SetNextWindowSizeConstraints(ImVec2(CustomMapSelectionUI_private::locWindowWidth, CustomMapSelectionUI_private::locWindowHeight), ImVec2(FLT_MAX, FLT_MAX));

	return true;
}

std::string CustomMapSelectionUI::GetMenuName() const
{
	return CustomMapSelectionUI_private::locMenuName;
}

std::string CustomMapSelectionUI::GetMenuTitle() const
{
	return myTitle;
}

bool CustomMapSelectionUI::IsWindowOpen() const
{
	return myIsWindowOpen;
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
