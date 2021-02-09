#pragma once

#include <cstdint>

class CustomMapSelectionUI
{
public:
	void SetImGuiContext(std::uintptr_t aContext);
	void OnOpen();
	void OnClose();
	void Render();
};
