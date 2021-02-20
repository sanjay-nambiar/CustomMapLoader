#include "CustomMapLoaderPlugin.h"

#include "CustomMapLoader.h"
#include "CustomMapSelectionUI.h"
#include "Version.h"

#include <fstream>
#include <sstream>

BAKKESMOD_PLUGIN(CustomMapLoaderPlugin, PLUGIN_NAME_STR, FULL_VERSION_STRING, 0x00)

CustomMapLoaderPlugin::CustomMapLoaderPlugin()
	: myIsWindowInitialized(false)
{
}

void CustomMapLoaderPlugin::onLoad()
{
	std::filesystem::path pluginDataDirectory = gameWrapper->GetDataFolder() / L"CustomMapLoader";

	// Create objects and initialize sub systems
	myMapLoader = std::make_shared<CustomMapLoader>();
	myMapSelectionUI = std::make_shared<CustomMapSelectionUI>();

	myMapLoader->Initialize(gameWrapper, cvarManager, myMapSelectionUI, FULL_PLUGIN_NAME, pluginDataDirectory.string());
	myMapSelectionUI->Initialize(myMapLoader);

	myLaunchWindowKeybind = std::make_shared<std::string>();
	myLoadMapKeybind = std::make_shared<std::string>();

	// Register console vars and commands
	cvarManager->registerCvar("cml_custom_map_path", "", "Custom maps directory", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myCustomMapDirectory);

	cvarManager->registerCvar("cml_selected_map", "", "Selected custom map", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myModel.mySelectedMap);

	cvarManager->registerNotifier("cml_load_custom_map", [this](const std::vector<std::string>&)
	{
		myMapLoader->LoadSelectedMap();
    }, "Loads currently selected custom map", PERMISSION_ALL);
}

void CustomMapLoaderPlugin::onUnload()
{ }

void CustomMapLoaderPlugin::OnOpen()
{
	myIsWindowInitialized = myMapSelectionUI->OnOpen();
}

void CustomMapLoaderPlugin::OnClose()
{
	myMapSelectionUI->OnClose();
}

void CustomMapLoaderPlugin::Render()
{
	if (myIsWindowInitialized)
		myMapSelectionUI->Render();

	if (!myMapSelectionUI->IsWindowOpen())
		cvarManager->executeCommand("togglemenu " + myMapSelectionUI->GetMenuName());
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
