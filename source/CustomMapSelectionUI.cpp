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
#include <windows.h>

namespace CustomMapSelectionUI_private
{
	static std::string locMenuName = "custom_map_loader";
	static float locWindowWidth = 1024.f;
	static float locWindowHeight = 768.f;

	static ImColor locColorGreen = IM_COL32(0, 255, 0, 255);
	static ImColor locColorRed = IM_COL32(255, 0, 0, 255);
	static ImColor locColorYellow = IM_COL32(160, 160, 0, 255);

	static const char* locPlaceholderImageName = "placeholder.jpg";
	std::shared_ptr<ImageWrapper> locPlaceholderImage;
	std::filesystem::path locPlaceholderImagePath;

	void locDrawMapGrid(const std::vector<CustomMapLoader::MapInfo>& someMapDetails)
	{
		static std::uint32_t width = 300;
		static std::uint32_t height = 225;
		static std::uint32_t padding = 10;
		static std::uint32_t paddedWidth = width + (2 * padding);
		static std::uint32_t paddedHeight = height + (2 * padding);
		static std::uint32_t wordWrapMaxLength =  32;

		ImVec2 os = ImGui::GetCursorScreenPos();
		ImVec2 origin = ImGui::GetCursorPos();

		float x = 0.f;
		float y = 0.f;
		for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(someMapDetails.size()); i++)
		{
			const CustomMapLoader::MapInfo& mapDetails = someMapDetails[i];
			ImGui::PushID(i);

			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= os.x;
			mousePos.y -= os.y;
			bool isNotHovering = (mousePos.x > x && mousePos.x < x + paddedWidth && mousePos.y > y && mousePos.y < y + paddedHeight);

			// Draw selection border
			if (isNotHovering)
			{
				ImU32 col = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
				x += os.x;
				y += os.y;
				ImGui::RenderFrame(ImVec2(x, y), ImVec2(x + paddedWidth, y + paddedHeight), col, true, 3.f);
				x -= os.x;
				y -= os.y;
			}

			// Draw image
			auto previewImage = locPlaceholderImage;
			if (mapDetails.myPreviewImage && mapDetails.myPreviewImage->IsLoadedForImGui())
				previewImage = mapDetails.myPreviewImage;

			if (previewImage && previewImage->IsLoadedForImGui())
			{
				ImGui::SetCursorPosX(x + origin.x + padding);
				ImGui::SetCursorPosY(y + origin.y + padding);
				ImGui::Image(previewImage->GetImGuiTex(), ImVec2(static_cast<float>(width), static_cast<float>(height)));
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
	}

	void locRenderCustomMapSelectionTab(CustomMapLoader& aCustomMapLoader)
	{
		if (ImGui::BeginTabItem("Custom Maps"))
		{
			locDrawMapGrid(aCustomMapLoader.GetMaps());
			ImGui::EndTabItem();
		}
	}

	std::vector<std::string> locRefreshErrorMessages;
	bool locIsRestoreMapSuccessful = false;

	void locRenderSettingsTab(CustomMapLoader& aCustomMapLoader)
	{
		if (ImGui::BeginTabItem("Settings"))
		{
			ImGui::NewLine();
			ImGui::TextColored(locColorYellow, " Map to replace: %s", aCustomMapLoader.GetMapToReplace().c_str());

			char buffer[1024];

			ImGui::NewLine();
			strcpy(buffer, aCustomMapLoader.GetGameDirectory().string().c_str());
			ImGui::InputText("Rocket League Path", buffer, 128, ImGuiInputTextFlags_CharsNoBlank);
			aCustomMapLoader.SetGameDirectory(buffer);

			ImGui::NewLine();
			strcpy(buffer, aCustomMapLoader.GetCustomMapDirectory().string().c_str());
			ImGui::InputText("Custom Maps Directory", buffer, 128, ImGuiInputTextFlags_CharsNoBlank);
			aCustomMapLoader.SetCustomMapDirectory(buffer);

			ImGui::NewLine();
			ImGui::NewLine();
			if (ImGui::Button("Refresh Maps", ImVec2(100, 30)))
			{
				locRefreshErrorMessages.erase(locRefreshErrorMessages.begin(), locRefreshErrorMessages.end());

				if (aCustomMapLoader.ValidateDirectories(locRefreshErrorMessages))
					aCustomMapLoader.RefreshMaps();

				ImGui::OpenPopup("#cml_map_refresh_details");
			}

			ImGui::SameLine();
			if(ImGui::Button("Restore Map", ImVec2(100, 30)))
			{
				locIsRestoreMapSuccessful = false; // aCustomMapLoader.RestorePristineState();
				ImGui::OpenPopup("#cml_restore_map_details");
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 50));

			// Display error / success messages
			if (ImGui::BeginPopup("#cml_map_refresh_details"))
			{
				if (locRefreshErrorMessages.size() > 0)
				{
					for (const std::string& message : locRefreshErrorMessages)
					{
						ImGui::TextColored(locColorRed, message.c_str());
						ImGui::NewLine();
					}
				}
				else if (aCustomMapLoader.GetMaps().size() > 0)
				{
					ImGui::TextColored(locColorGreen, "Found %d maps.", aCustomMapLoader.GetMaps().size());
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("#cml_restore_map_details"))
			{
				if (locIsRestoreMapSuccessful)
					ImGui::TextColored(locColorGreen, "Restored original map from backup!");
				else
					ImGui::TextColored(locColorRed, "Failed restoring original map from backup!");

				ImGui::EndPopup();
			}

			ImGui::EndTabItem();
		}
	}
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

	if (ImGui::Begin(myTitle.c_str(), &myIsWindowOpen))
	{
		if (ImGui::BeginTabBar("#TabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip))
		{
			CustomMapSelectionUI_private::locRenderSettingsTab(myMapLoader);
			CustomMapSelectionUI_private::locRenderCustomMapSelectionTab(myMapLoader);
			ImGui::EndTabBar();
		}
	}
	ImGui::End();

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

void CustomMapSelectionUI::LoadPlaceholderImage(const std::filesystem::path& aPluginDataDirectory)
{
	std::filesystem::path imagePath = aPluginDataDirectory;
	imagePath.append(CustomMapSelectionUI_private::locPlaceholderImageName).make_preferred();
	CustomMapSelectionUI_private::locPlaceholderImage = std::make_shared<ImageWrapper>(imagePath, false, true);

	CustomMapSelectionUI_private::locPlaceholderImagePath = imagePath;
}
