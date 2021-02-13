#pragma once

#include "CustomMapLoader.h"

#include <cstdint>
#include <string>

class CustomMapSelectionUI
{
public:
	CustomMapSelectionUI();
	~CustomMapSelectionUI() = default;

	void InitializeDependencies(std::shared_ptr<CustomMapLoader> aCustomMapLoader);

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

private:
	bool myIsWindowOpen;
	std::string myTitle;

	std::shared_ptr<CustomMapLoader> myCustomMapLoader;
};
