#include "CustomMapLoader.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace CustomMapLoader_private
{
	static const char* locLoadMapCommand = "load_workshop ";

	static std::vector<std::string> locImageTypes = { ".jpg", ".png" };
	static std::vector<std::string> locMapTypes = { ".udk", ".upk" };

	static const char* locPlaceholderImageName = "placeholder.jpg";
	std::shared_ptr<ImageWrapper> locPlaceholderImage;
	std::filesystem::path locPlaceholderImagePath;

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
				info.myMapFile = entry.path();
				info.myLastUpdated = locTimeString(std::filesystem::last_write_time(info.myMapFile));
			}
		}

		return info;
	}
}

CustomMapLoader::CustomMapLoader()
: myCustomMapDirectory(std::make_shared<std::string>())
{}

void CustomMapLoader::InitializeDependencies(const std::shared_ptr<CVarManagerWrapper>& aCvarManager, std::shared_ptr<CustomMapSelectionUI> aCustomMapSelectionUI,
	const std::string& aPluginFullName)
{
	myCVarManager = aCvarManager;
	myCustomMapSelectionUI = aCustomMapSelectionUI;

	myCustomMapSelectionUI->SetTitle(aPluginFullName);
}

void CustomMapLoader::SetCustomMapDirectory(const std::string& aCustomMapDirectory)
{
	*myCustomMapDirectory = aCustomMapDirectory;
}

std::filesystem::path CustomMapLoader::GetCustomMapDirectory() const
{
	return std::filesystem::path(*myCustomMapDirectory).make_preferred();
}

bool CustomMapLoader::ValidateDirectories(std::vector<std::string>& errorMessages)
{
	if (std::filesystem::exists(GetCustomMapDirectory()))
		return true;

	errorMessages.push_back("Backup directory does not exist!");
	return false;
}

void CustomMapLoader::LoadPlaceholderImage(const std::filesystem::path& aPluginDataDirectory)
{
	std::filesystem::path imagePath = aPluginDataDirectory;
	imagePath.append(CustomMapLoader_private::locPlaceholderImageName).make_preferred();
	CustomMapLoader_private::locPlaceholderImage = std::make_shared<ImageWrapper>(imagePath, false, true);

	CustomMapLoader_private::locPlaceholderImagePath = imagePath;
}

std::shared_ptr<ImageWrapper> CustomMapLoader::GetPlaceholderImage() const
{
	return CustomMapLoader_private::locPlaceholderImage;
}

bool CustomMapLoader::RefreshMaps()
{
	myMaps.erase(myMaps.begin(), myMaps.end());

	for (const auto& entry : std::filesystem::directory_iterator(GetCustomMapDirectory()))
	{
		if (!entry.is_directory())
			continue;

		MapInfo info = CustomMapLoader_private::locGetMapDetails(entry, *this);
		if (!info.myMapFile.empty())
			myMaps.emplace_back(info);
	}

	return true;
}

bool CustomMapLoader::LoadMap(std::int32_t anIndex)
{
	if (anIndex < 0 && anIndex >= myMaps.size())
		return false;

	std::stringstream commandBuilder;
	commandBuilder << CustomMapLoader_private::locLoadMapCommand << '"' << myMaps[anIndex].myMapFile << '"';

	std::string command;
	commandBuilder.str(command);
	myCVarManager->executeCommand(command);

	return false;
}

const std::vector<CustomMapLoader::MapInfo>& CustomMapLoader::GetMaps() const
{
	return myMaps;
}
