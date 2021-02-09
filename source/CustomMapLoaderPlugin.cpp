#include "CustomMapLoaderPlugin.h"

#define STRINGIZE(s)		STRINGIZE2(s)
#define STRINGIZE2(s)		#s

#define PLUGIN_NAME			Custom Map Loader
#define PLUGIN_NAME_STR		STRINGIZE(PLUGIN_NAME)

#define MAJOR_VERSION		0
#define MINOR_VERSION		1
#define HOTFIX_VERSION		1

#define FULL_VERSION_STRING	"v" STRINGIZE(MAJOR_VERSION) "." STRINGIZE(MINOR_VERSION) "." STRINGIZE(HOTFIX_VERSION)
#define FULL_PLUGIN_NAME	STRINGIZE(PLUGIN_NAME) " v" STRINGIZE(MAJOR_VERSION) "." STRINGIZE(MINOR_VERSION) "." STRINGIZE(HOTFIX_VERSION)

BAKKESMOD_PLUGIN(CustomMapLoaderPlugin, PLUGIN_NAME_STR, FULL_VERSION_STRING, PLUGINTYPE_FREEPLAY)

void CustomMapLoaderPlugin::onLoad()
{

	myMenuTitle = FULL_PLUGIN_NAME;
	myMapLoader.OnLoad();
}

void CustomMapLoaderPlugin::onUnload()
{
	myMapLoader.OnUnload();
}

void CustomMapLoaderPlugin::OnOpen()
{
	myMapSelectionUI.OnOpen();
}

void CustomMapLoaderPlugin::OnClose()
{
	myMapSelectionUI.OnClose();
}

void CustomMapLoaderPlugin::Render()
{
	myMapSelectionUI.Render();
}

std::string CustomMapLoaderPlugin::GetMenuName()
{
	return "Custom Map Loader";
}

std::string CustomMapLoaderPlugin::GetMenuTitle()
{
	return "Select Custom Map";
}

void CustomMapLoaderPlugin::SetImGuiContext(uintptr_t aContext)
{
	myMapSelectionUI.SetImGuiContext(aContext);
}

bool CustomMapLoaderPlugin::ShouldBlockInput()
{
	return true;
}

bool CustomMapLoaderPlugin::IsActiveOverlay()
{
	return true;
}
