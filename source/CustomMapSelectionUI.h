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

	void Render();

	std::string GetMenuName();
	std::string GetMenuTitle();

	bool ShouldBlockInput();
	bool IsActiveOverlay();

private:
	void RenderCustomMapSelectionWidget();

	bool myIsWindowOpen;
	CustomMapLoader& myMapLoader;

	std::string myTitle;
};
