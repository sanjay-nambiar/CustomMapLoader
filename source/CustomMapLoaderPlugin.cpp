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

	static const char* locCmlMapToReplace = "Labs_Underpass_P.upk";
	static const char* locCmlRocketLeaguePath = "D:/Games/Epic Games/Games/rocketleague";
	static const char* locCmlCustomMapPath = "D:/Games/Personal Game Content/Rocket Leauge/Custom Maps/Custom/Maps";
	static const char* locCmlActiveCustomMap = "None";
}

CustomMapLoaderPlugin::CustomMapLoaderPlugin()
	: myIsRendererInitialized(false)
{
}

void CustomMapLoaderPlugin::onLoad()
{
	myMapLoader = std::make_shared<CustomMapLoader>();
	myMapSelectionUI = std::make_shared<CustomMapSelectionUI>();

	myMapLoader->InitializeDependencies(cvarManager, myMapSelectionUI, FULL_PLUGIN_NAME);
	myMapSelectionUI->InitializeDependencies(myMapLoader);

	std::filesystem::path pluginDataDirectory = gameWrapper->GetDataFolder() / L"CustomMapLoader";
	myMapLoader->LoadPlaceholderImage(pluginDataDirectory.string());

	cvarManager->registerCvar("cml_custom_map_path", CustomMapLoaderPlugin_private::locCmlCustomMapPath, "Custom maps directory", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myCustomMapDirectory);

	cvarManager->registerCvar("cml_error_message", "", "Error messages", false);
}

void CustomMapLoaderPlugin::onUnload()
{
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
	if (myMapSelectionUI->Render())
	{
		if (!myIsRendererInitialized)
			cvarManager->getCvar("cml_error_message").setValue("");

		myIsRendererInitialized = true;
	}
	else
	{
		if (myIsRendererInitialized)
			cvarManager->getCvar("cml_error_message").setValue("ERROR: ImGui isn't initialized properly!");

		myIsRendererInitialized = false;
	}

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
