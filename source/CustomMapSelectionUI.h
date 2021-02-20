#pragma once

#include "CustomMapLoader.h"

#include <cstdint>
#include <memory>
#include <string>

class CustomMapSelectionUI
{
public:
	CustomMapSelectionUI();
	~CustomMapSelectionUI() = default;

	void Initialize(std::shared_ptr<CustomMapLoader> aCustomMapLoader);

	void SetImGuiContext(std::uintptr_t aContext);

	bool OnOpen();
	void OnClose();

	bool Render();

	std::string GetMenuName() const;
	std::string GetMenuTitle() const;
	bool IsWindowOpen() const;

	bool ShouldBlockInput();
	bool IsActiveOverlay();

private:
	std::shared_ptr<CustomMapLoader> myCustomMapLoader;
	bool myIsWindowOpen;
};
