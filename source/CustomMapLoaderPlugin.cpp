#include "CustomMapLoaderPlugin.h"

#include "CustomMapLoader.h"
#include "CustomMapSelectionUI.h"

#include "Version.h"

BAKKESMOD_PLUGIN(CustomMapLoaderPlugin, PLUGIN_NAME_STR, FULL_VERSION_STRING, PERMISSION_ALL)

namespace CustomMapLoaderPlugin_private
{
	std::string locGetConfigFileName(const std::filesystem::path& aConfigFolder)
	{
		return std::filesystem::path(aConfigFolder).append("/custom_map_loader.cfg").string();
	}
}

void CustomMapLoaderPlugin::onLoad()
{
	myMapLoader.reset(new CustomMapLoader());
	myMapSelectionUI.reset(new CustomMapSelectionUI(*myMapLoader));

	myMapSelectionUI->SetTitle(FULL_PLUGIN_NAME);

	myBakkesModConfigFolder = gameWrapper->GetBakkesModPath() / L"cfg";
	myRocketPluginDataFolder = gameWrapper->GetDataFolder() / L"RocketPlugin";

	cvarManager->registerCvar("map_to_replace", "Labs_Underpass_P", "The map file to replace with workshop map", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myMapToReplace);

	cvarManager->registerCvar("rocket_league_path", "C:/Program Files/Epic Games/rocketleague/", "Epic Games Rocket League path", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myGameDirectory);

	cvarManager->registerCvar("custom_map_path", "C:/Program Files/Epic Games/rocketleague/CustomMaps", "Custom maps directory", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myCustomMapDirectory);

	cvarManager->registerCvar("active_custom_map", "None", "error message", false);

	cvarManager->registerNotifier("cml_openMenu", [this](std::vector<std::string>)
		{
			cvarManager->executeCommand("togglemenu " + myMapSelectionUI->GetMenuName());
		}, "Opens Custom Map Loader menu", 0);

	std::string configFile = CustomMapLoaderPlugin_private::locGetConfigFileName(myBakkesModConfigFolder);
	if (std::filesystem::exists(configFile))
		cvarManager->loadCfg(CustomMapLoaderPlugin_private::locGetConfigFileName(myBakkesModConfigFolder));
}

void CustomMapLoaderPlugin::onUnload()
{
	cvarManager->backupCfg(CustomMapLoaderPlugin_private::locGetConfigFileName(myBakkesModConfigFolder));
}

void CustomMapLoaderPlugin::OnOpen()
{
	myMapSelectionUI->OnOpen();
}

void CustomMapLoaderPlugin::OnClose()
{
	myMapSelectionUI->OnClose();
}

void CustomMapLoaderPlugin::Render()
{
	myMapSelectionUI->Render();
}

std::string CustomMapLoaderPlugin::GetMenuName()
{
	return myMapSelectionUI->GetMenuName();
}

std::string CustomMapLoaderPlugin::GetMenuTitle()
{
	return myMapSelectionUI->GetMenuTitle();
}

void CustomMapLoaderPlugin::SetImGuiContext(uintptr_t aContext)
{
	myMapSelectionUI->SetImGuiContext(aContext);
}

bool CustomMapLoaderPlugin::ShouldBlockInput()
{
	return myMapSelectionUI->ShouldBlockInput();
}

bool CustomMapLoaderPlugin::IsActiveOverlay()
{
	return myMapSelectionUI->IsActiveOverlay();
}
