#include "CustomMapLoaderPlugin.h"

#include "CustomMapLoader.h"
#include "CustomMapSelectionUI.h"

#include "Version.h"

BAKKESMOD_PLUGIN(CustomMapLoaderPlugin, PLUGIN_NAME_STR, FULL_VERSION_STRING, PLUGINTYPE_FREEPLAY)

void CustomMapLoaderPlugin::onLoad()
{
	//TODO:: find game dir and backup dir. also update backup dir if settings change
	myMapLoader.reset(new CustomMapLoader());
	myMapSelectionUI.reset(new CustomMapSelectionUI(*myMapLoader));
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
	myMapSelectionUI->Render();
}

std::string CustomMapLoaderPlugin::GetMenuName()
{
	return "custommaploader";
}

std::string CustomMapLoaderPlugin::GetMenuTitle()
{
	return FULL_PLUGIN_NAME;
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
