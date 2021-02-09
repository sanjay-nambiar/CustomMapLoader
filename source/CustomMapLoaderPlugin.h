#pragma once

#pragma comment(lib, "BakkesMod.lib")

#include "bakkesmod\plugin\bakkesmodplugin.h"
#include "bakkesmod\plugin\pluginwindow.h"

#include "CustomMapLoader.h"
#include "CustomMapSelectionUI.h"

#include <string>

class CustomMapLoaderPlugin : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginWindow
{
public:
	// BakkesModPlugin API
	void onLoad() override;
	void onUnload() override;


	// PluginWindow API
	void OnOpen() override;
	void OnClose() override;
	void Render() override;

	std::string GetMenuName() override;
	std::string GetMenuTitle() override;
	void SetImGuiContext(uintptr_t aContext) override;
	bool ShouldBlockInput() override;
	bool IsActiveOverlay() override;

private:
	CustomMapLoader myMapLoader;
	CustomMapSelectionUI myMapSelectionUI;
};
