#include "CustomMapLoader.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace CustomMapLoader_private
{
	static const char* locLoadMapCommand = "load_workshop";

	static std::vector<std::string> locImageTypes = { ".jpg", ".png" };
	static std::vector<std::string> locMapTypes = { ".udk", ".upk" };

	static const char* locPlaceholderImageName = "placeholder.jpg";

	std::shared_ptr<ImageWrapper> locLoadPlaceholderImage(const std::filesystem::path& aPluginDataDirectory)
	{
		std::filesystem::path imagePath = aPluginDataDirectory;
		imagePath.append(locPlaceholderImageName).make_preferred();

		return std::make_shared<ImageWrapper>(imagePath, false, true);
	}

	void locCloseOpenMenus(std::shared_ptr<CVarManagerWrapper> aCVarManagerconst, CustomMapSelectionUI& aCustomMapSelectionUI)
	{
		aCVarManagerconst->executeCommand("closemenu settings");
		aCVarManagerconst->executeCommand("closemenu console2");
		aCVarManagerconst->executeCommand("closemenu pluginmanager");
		aCVarManagerconst->executeCommand("closemenu " + aCustomMapSelectionUI.GetMenuName());
	}

	template <typename TimePoint>
	std::string locTimeString(TimePoint tp)
	{
		using namespace std::chrono;

		auto sctp = time_point_cast<system_clock::duration>(tp - TimePoint::clock::now() + system_clock::now());
		std::time_t tt = system_clock::to_time_t(sctp);
		std::tm* local = std::localtime(&tt);
		std::stringstream buffer;
		buffer << std::put_time(local, "%A, %d %B %Y %H:%M");
		return buffer.str();
	}

	CustomMapLoader::MapInfo locGetMapDetails(const std::filesystem::directory_entry& aMapDirectory, CustomMapLoader& aLoader)
	{
		CustomMapLoader::MapInfo info;
		info.myTitle = aMapDirectory.path().stem().string();

		for (const auto& entry : std::filesystem::directory_iterator(aMapDirectory))
		{
			if (!entry.is_regular_file())
				continue;

			if (std::find(locImageTypes.begin(), locImageTypes.end(), entry.path().extension().string()) != locImageTypes.end())
			{
				info.myPreviewImage = std::make_shared<ImageWrapper>(entry.path(), false, true);
			}

			if (std::find(locMapTypes.begin(), locMapTypes.end(), entry.path().extension().string()) != locMapTypes.end())
			{
				info.myMapFile = entry.path().generic_string();
				info.myLastUpdated = locTimeString(std::filesystem::last_write_time(info.myMapFile));
			}
		}

		return info;
	}
}

CustomMapLoader::CustomMapLoader()
: myCustomMapDirectory(std::make_shared<std::string>())
{
	myModel.mySelectedMap = std::make_shared<std::string>();
}

void CustomMapLoader::Initialize(const std::shared_ptr<GameWrapper> aGameWrapper, const std::shared_ptr<CVarManagerWrapper>& aCvarManager,
	std::shared_ptr<CustomMapSelectionUI> aCustomMapSelectionUI, const std::string& aPluginFullName, const std::filesystem::path& aPluginDataDirectory)
{
	myGameWrapper = aGameWrapper;
	myCVarManager = aCvarManager;
	myCustomMapSelectionUI = aCustomMapSelectionUI;

	myModel.myWindowTitle = aPluginFullName;
	myModel.myPlaceholderImage = CustomMapLoader_private::locLoadPlaceholderImage(aPluginDataDirectory);
}

void CustomMapLoader::SetCustomMapDirectory(const std::string& aCustomMapDirectory)
{
	myCVarManager->getCvar("cml_custom_map_path").setValue(std::filesystem::path(aCustomMapDirectory).generic_string());
}

std::filesystem::path CustomMapLoader::GetCustomMapDirectory() const
{
	return std::filesystem::path(*myCustomMapDirectory).make_preferred();
}

bool CustomMapLoader::RefreshMaps()
{
	myModel.myErrorMessages.erase(myModel.myErrorMessages.begin(), myModel.myErrorMessages.end());

	if (!std::filesystem::exists(GetCustomMapDirectory()))
	{
		myModel.myErrorMessages.push_back("Custom map directory not found.");
		return false;
	}

	myModel.myMaps.erase(myModel.myMaps.begin(), myModel.myMaps.end());

	for (const auto& entry : std::filesystem::directory_iterator(GetCustomMapDirectory()))
	{
		if (!entry.is_directory())
			continue;

		MapInfo info = CustomMapLoader_private::locGetMapDetails(entry, *this);
		if (!info.myMapFile.empty())
			myModel.myMaps.emplace_back(info);
	}

	return true;
}

bool CustomMapLoader::SelectCustomMap(std::int32_t anIndex)
{
	if (anIndex < 0 && anIndex >= myModel.myMaps.size())
		return false;

	myCVarManager->getCvar("cml_selected_map").setValue(myModel.myMaps[anIndex].myMapFile);

	return true;
}

void CustomMapLoader::LoadSelectedMap()
{
	if (*myModel.mySelectedMap == "")
		return;

	CustomMapLoader_private::locCloseOpenMenus(myCVarManager, *myCustomMapSelectionUI);

	std::stringstream commandBuilder;
	commandBuilder << CustomMapLoader_private::locLoadMapCommand << " \"" << *myModel.mySelectedMap << "\"";

	myCVarManager->executeCommand(commandBuilder.str());
}

void CustomMapLoader::Execute(const std::function<void(GameWrapper*)>& aFunction)
{
	myGameWrapper->Execute([this, aFunction](GameWrapper* aGameWrapper)
	{
		try
		{
			aFunction(aGameWrapper);
		}
		catch (const std::exception& e)
		{
			myCVarManager->log(std::string("[CRITICAL] [CML] Execute threw exception: ") + e.what());
		}
		catch (...)
		{
			myCVarManager->log("[CRITICAL] [CML] Execute threw an exception");
		}
	});
}

const CustomMapLoader::UIModel& CustomMapLoader::GetUIModel() const
{
	return myModel;
}
