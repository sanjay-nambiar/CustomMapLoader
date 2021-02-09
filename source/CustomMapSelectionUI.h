#pragma once

#include "CustomMapLoader.h"

#include <cstdint>

class CustomMapSelectionUI
{
public:
	CustomMapSelectionUI(CustomMapLoader& aMapLoader);
	~CustomMapSelectionUI() = default;

	void SetImGuiContext(std::uintptr_t aContext);

	void OnOpen();
	void OnClose();

	void Render();

	bool ShouldBlockInput();
	bool IsActiveOverlay();

private:
	void RenderCustomMapSelectionWidget();

	bool myIsWindowOpen;
	CustomMapLoader& myMapLoader;
};
