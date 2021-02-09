#pragma once

#include <cstdint>

class CustomMapSelectionUI
{
public:
	CustomMapSelectionUI();
	~CustomMapSelectionUI() = default;

	void SetImGuiContext(std::uintptr_t aContext);

	void OnOpen();
	void OnClose();

	void Render();

	bool ShouldBlockInput();
	bool IsActiveOverlay();

private:
	bool myIsWindowOpen;
};
