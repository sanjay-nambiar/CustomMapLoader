#include "CustomMapLoaderPlugin.h"

#include "CustomMapLoader.h"
#include "CustomMapSelectionUI.h"
#include "Version.h"

#include <fstream>
#include <sstream>

BAKKESMOD_PLUGIN(CustomMapLoaderPlugin, PLUGIN_NAME_STR, FULL_VERSION_STRING, 0x00)

namespace CustomMapLoaderPlugin_private
{
	constexpr const char* locDefaultLaunchMenuKeyBind = "F1";
	constexpr const char* locDefaultLoadMapKeyBind = "Zero";

	constexpr const char* locCustomMapPath = "D:/Games/Personal Game Content/Rocket League/Custom Maps/Custom/Maps";
	static const char* locLoadMapCommand = "load_workshop";

	std::filesystem::path locBakkesModConfigFolder;

	std::string locGetConfigFileName()
	{
		return locBakkesModConfigFolder.append("config.cfg").string();
	}

	std::string locGetBindsFileName()
	{
		return locBakkesModConfigFolder.append("binds.cfg").string();
	}

	bool locIsCommandBound(const std::string& aCommand)
	{
		std::ifstream file(locGetBindsFileName());
		if (file.is_open())
		{
			std::string line;
			while (getline(file, line))
			{
				if (line.find(aCommand) != std::string::npos)
				{
					file.close();
					return true;
				}
			}
			file.close();
		}

		return false;
	}

	bool locIsWindowBound(const std::string& aWindowName)
	{
		const std::string bind = "togglemenu " + aWindowName;
		return locIsCommandBound(bind);
	}
}

CustomMapLoaderPlugin::CustomMapLoaderPlugin()
	: myIsRendererInitialized(false)
{
}

void CustomMapLoaderPlugin::onLoad()
{
	CustomMapLoaderPlugin_private::locBakkesModConfigFolder = gameWrapper->GetBakkesModPath() / L"cfg";

	std::filesystem::path pluginDataDirectory = gameWrapper->GetDataFolder() / L"CustomMapLoader";

	// Create objects and initialize sub systems
	myMapLoader = std::make_shared<CustomMapLoader>();
	myMapSelectionUI = std::make_shared<CustomMapSelectionUI>();

	myMapLoader->Initialize(cvarManager, myMapSelectionUI, FULL_PLUGIN_NAME, pluginDataDirectory.string());
	myMapSelectionUI->Initialize(myMapLoader);

	myLaunchWindowKeybind = std::make_shared<std::string>();
	myLoadMapKeybind = std::make_shared<std::string>();

	// Register console vars and commands
	cvarManager->registerCvar("cml_custom_map_path", CustomMapLoaderPlugin_private::locCustomMapPath, "Custom maps directory", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myCustomMapDirectory);

	cvarManager->registerCvar("cml_selected_map", "", "Selected custom map", true, false, 0.0f, false, 0.0f, true)
		.bindTo(myMapLoader->myModel.mySelectedMap);

	cvarManager->registerCvar("cml_error_message", "", "Error messages", false);

	cvarManager->registerNotifier("cml_load_custom_map", [this](const std::vector<std::string>&)
	{
		std::stringstream commandBuilder;
		commandBuilder << CustomMapLoaderPlugin_private::locLoadMapCommand << " \"" << *myMapLoader->GetUIModel().mySelectedMap
			<< "\"";
		cvarManager->executeCommand(commandBuilder.str());
    }, "Loads currently selected custom map", PERMISSION_ALL);

	cvarManager->registerCvar("cml_menu_keybind", CustomMapLoaderPlugin_private::locDefaultLaunchMenuKeyBind, "Keybind for Custom Map Loader menu")
		.addOnValueChanged([this](std::string aNewValue, CVarWrapper)
		{
			*myLaunchWindowKeybind = aNewValue;
		});

	cvarManager->registerCvar("cml_load_map_keybind", CustomMapLoaderPlugin_private::locDefaultLoadMapKeyBind, "Keybind for launching custom map")
		.addOnValueChanged([this](std::string aNewValue, CVarWrapper)
		{
			*myLoadMapKeybind = aNewValue;
		});

	// Set binds if not already bound
	std::string toggleWindowCommand = "togglemenu " + GetMenuName();
	if (!CustomMapLoaderPlugin_private::locIsCommandBound(toggleWindowCommand))
		cvarManager->setBind(*myLaunchWindowKeybind, toggleWindowCommand);

	if (!CustomMapLoaderPlugin_private::locIsCommandBound("cml_load_custom_map"))
		cvarManager->setBind(*myLoadMapKeybind, "cml_load_custom_map");

	cvarManager->loadCfg(CustomMapLoaderPlugin_private::locGetConfigFileName());
}

void CustomMapLoaderPlugin::onUnload()
{
	cvarManager->backupCfg(CustomMapLoaderPlugin_private::locGetConfigFileName());
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
