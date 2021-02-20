#include "CustomMapSelectionUI.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "bakkesmod/wrappers/ImageWrapper.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace CustomMapSelectionUI_private
{
	static std::string locMenuName = "custom_map_loader";
	static float locWindowWidth = 1024.f;
	static float locWindowHeight = 768.f;

	static ImColor locColorGreen = IM_COL32(0, 255, 0, 255);
	static ImColor locColorRed = IM_COL32(255, 0, 0, 255);
	static ImColor locColorYellow = IM_COL32(255, 255, 0, 255);
	static ImColor locColorWhite = IM_COL32(255, 255, 255, 255);

	void locRenderSettingsTab(CustomMapLoader& aCustomMapLoader)
	{
		if (ImGui::BeginTabItem("Settings"))
		{
			const CustomMapLoader::UIModel& uiModel = aCustomMapLoader.GetUIModel();

			char buffer[1024];
			strcpy(buffer, aCustomMapLoader.GetCustomMapDirectory().string().c_str());

			ImGui::NewLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::TextColored(locColorYellow, "Map folder: ");
			ImGui::SameLine();
			if (ImGui::InputText("##Custom Maps Directory", buffer, 1023, ImGuiInputTextFlags_None))
			{
				aCustomMapLoader.SetCustomMapDirectory(std::string(buffer));
			}
			ImGui::NewLine();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::TextColored(locColorYellow, "Selected Map: ");
			ImGui::SameLine();
			ImGui::TextColored(locColorWhite, uiModel.mySelectedMap->c_str());

			ImGui::NewLine();
			ImGui::NewLine();
			if (ImGui::Button("Refresh Maps", ImVec2(100, 30)))
			{
				aCustomMapLoader.RefreshMaps();
			}

			// Display error / success messages
			ImGui::NewLine();
			if (uiModel.myErrorMessages.size() > 0)
			{
				for (const std::string& message : uiModel.myErrorMessages)
				{
					ImGui::TextColored(locColorRed, message.c_str());
					ImGui::NewLine();
				}
			}
			else if (uiModel.myMaps.size() > 0)
			{
				ImGui::TextColored(locColorGreen, "Found %d maps.", uiModel.myMaps.size());
			}

			ImGui::EndTabItem();
		}
	}

	std::int32_t locDrawMapGrid(const std::vector<CustomMapLoader::MapInfo>& someMapDetails, std::shared_ptr<ImageWrapper> aPlaceholderImage, const std::string& aSelectedMap)
	{
		static std::uint32_t width = 300;
		static std::uint32_t height = 225;
		static std::uint32_t padding = 10;
		static std::uint32_t paddedWidth = width + (2 * padding);
		static std::uint32_t paddedHeight = height + (2 * padding);
		static std::uint32_t wordWrapMaxLength =  32;

		std::int32_t selectedIndex = -1;

		ImVec2 os = ImGui::GetCursorScreenPos();
		ImVec2 origin = ImGui::GetCursorPos();

		float x = 0.f;
		float y = 0.f;
		for (std::int32_t index = 0; index < static_cast<std::int32_t>(someMapDetails.size()); index++)
		{
			const CustomMapLoader::MapInfo& mapDetails = someMapDetails[index];
			ImGui::PushID(index);

			// Highlight currently selected map
			if (aSelectedMap == mapDetails.myMapFile)
			{
				x += os.x + static_cast<float>(padding / 2);
				y += os.y;
				ImGui::RenderFrame(ImVec2(x, y), ImVec2(x + paddedWidth, y + paddedHeight), locColorGreen, true, 3.f);
				x -= os.x + static_cast<float>(padding / 2);
				y -= os.y;
			}

			// Draw image
			std::shared_ptr<ImageWrapper> previewImage = aPlaceholderImage;
			if (mapDetails.myPreviewImage && mapDetails.myPreviewImage->IsLoadedForImGui())
				previewImage = mapDetails.myPreviewImage;

			if (previewImage && previewImage->IsLoadedForImGui())
			{
				ImGui::SetCursorPosX(x + origin.x + padding);
				ImGui::SetCursorPosY(y + origin.y + padding);
				if (ImGui::ImageButton(previewImage->GetImGuiTex(), ImVec2(static_cast<float>(width), static_cast<float>(height))))
					selectedIndex = index;
			}

			// Draw map name
			ImGui::SetCursorPosX(x + origin.x + padding);
			ImGui::SetCursorPosY(y + origin.y + paddedHeight);

			std::string description = mapDetails.myTitle;
			if (mapDetails.myTitle.length() > wordWrapMaxLength)
				description = mapDetails.myTitle.substr(0, wordWrapMaxLength) + "...";
			ImGui::Text(description.c_str());

			// Draw map last updated
			ImGui::SetCursorPosX(x + origin.x + padding);
			ImGui::SetCursorPosY(y + origin.y + paddedHeight + (2 * padding));
			ImGui::Text(mapDetails.myLastUpdated.c_str());

			// Update next image position
			x += paddedWidth;
			if (x + paddedWidth > ImGui::GetWindowWidth())
			{
				x = 0;
				y += paddedHeight + (5 * padding);
			}

			ImGui::PopID();
		}

		return selectedIndex;
	}

	std::int32_t locRenderCustomMapSelectionTab(std::shared_ptr<CustomMapLoader> aCustomMapLoader)
	{
		std::int32_t selectedIndex = -1;

		if (ImGui::BeginTabItem("Custom Maps"))
		{
			ImGui::NewLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

			const CustomMapLoader::UIModel& uiModel = aCustomMapLoader->GetUIModel();
			bool isLaunchDisabled = (*uiModel.mySelectedMap == "");

			float originalAlpha = ImGui::GetStyle().Alpha;
			if (isLaunchDisabled)
				ImGui::GetStyle().Alpha = 0.25f;

			if (ImGui::Button("Launch Map", ImVec2(100.f, 30.f)) && !isLaunchDisabled)
			{
				aCustomMapLoader->Execute([aCustomMapLoader](GameWrapper*)
				{
					aCustomMapLoader->LoadSelectedMap();
				});
			}

			ImGui::GetStyle().Alpha = originalAlpha;

			ImGui::NewLine();

			selectedIndex = locDrawMapGrid(uiModel.myMaps, uiModel.myPlaceholderImage, *uiModel.mySelectedMap);

			ImGui::EndTabItem();
		}

		return selectedIndex;
	}
}

CustomMapSelectionUI::CustomMapSelectionUI()
: myIsWindowOpen(false)
{}

void CustomMapSelectionUI::Initialize(std::shared_ptr<CustomMapLoader> aCustomMapLoader)
{
	myCustomMapLoader = aCustomMapLoader;
}

void CustomMapSelectionUI::SetImGuiContext(std::uintptr_t aContext)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(aContext));
}

bool CustomMapSelectionUI::OnOpen()
{
	myIsWindowOpen = false;

	if (IMGUI_CHECKVERSION())
	{
		myIsWindowOpen = true;
		myCustomMapLoader->RefreshMaps();
	}

	return myIsWindowOpen;
}

void CustomMapSelectionUI::OnClose()
{
	myIsWindowOpen = false;
}

bool CustomMapSelectionUI::Render()
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(CustomMapSelectionUI_private::locWindowWidth, CustomMapSelectionUI_private::locWindowHeight), ImVec2(FLT_MAX, FLT_MAX));

	std::int32_t selectedIndex = -1;

	const CustomMapLoader::UIModel& uiModel = myCustomMapLoader->GetUIModel();
	if (ImGui::Begin(uiModel.myWindowTitle.c_str(), &myIsWindowOpen))
	{
		if (ImGui::BeginTabBar("#TabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip))
		{
			CustomMapSelectionUI_private::locRenderSettingsTab(*myCustomMapLoader);
			selectedIndex = CustomMapSelectionUI_private::locRenderCustomMapSelectionTab(myCustomMapLoader);
			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	if (selectedIndex != -1)
		myCustomMapLoader->SelectCustomMap(selectedIndex);

	return true;
}

std::string CustomMapSelectionUI::GetMenuName() const
{
	return CustomMapSelectionUI_private::locMenuName;
}

std::string CustomMapSelectionUI::GetMenuTitle() const
{
	return myCustomMapLoader->GetUIModel().myWindowTitle;
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
