#pragma once

#include "CustomMapLoader.h"

#include <cstdint>
#include <string>

class CustomMapSelectionUI
{
public:
	CustomMapSelectionUI(CustomMapLoader& aMapLoader);
	~CustomMapSelectionUI() = default;

	void SetImGuiContext(std::uintptr_t aContext);
	void SetTitle(const std::string& aTitle);

	void OnOpen();
	void OnClose();

	bool Render();

	std::string GetMenuName() const;
	std::string GetMenuTitle() const;
	bool IsWindowOpen() const;

	bool ShouldBlockInput();
	bool IsActiveOverlay();

	void LoadPlaceholderImage(const std::filesystem::path& aPluginDataDirectory);

private:
	bool myIsWindowOpen;
	CustomMapLoader& myMapLoader;

	std::string myTitle;
};
