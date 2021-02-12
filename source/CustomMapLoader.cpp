#include "CustomMapLoader.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace CustomMapLoader_private
{
	static const char* locMapsDirectory = "TAGame/CookedPCConsole";

	static std::vector<std::string> locImageTypes = { ".jpg", ".png" };
	static std::vector<std::string> locMapTypes = { ".udk", ".upk" };

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

	bool locCopyFile(const std::filesystem::path& aSourcePath, const std::filesystem::path& aDestPath, bool aShouldOverwrite = true)
	{
		return std::filesystem::copy_file(aSourcePath, aDestPath, aShouldOverwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none);
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

	std::filesystem::path locGetBackupFilePath(const CustomMapLoader& aMapLoader)
	{
		std::filesystem::path backupFile = aMapLoader.GetGameDirectory();
		backupFile.append(aMapLoader.GetMapToReplace()).append(".bak").make_preferred();
		return backupFile;
	}

	bool locIsBackupAvailable(const CustomMapLoader& aMapLoader)
	{
		return  std::filesystem::exists(locGetBackupFilePath(aMapLoader));
	}
}

CustomMapLoader::CustomMapLoader()
	: myActiveCustomMapIndex(-1)
{
	myGameDirectory = std::make_shared<std::string>();
	myCustomMapDirectory = std::make_shared<std::string>();
	myMapToReplace = std::make_shared<std::string>();
	myActiveCustomMap = std::make_shared<std::string>();
}

bool CustomMapLoader::ValidateDirectories(std::vector<std::string>& errorMessages)
{
	bool isValid = true;

	if (!std::filesystem::exists(GetGameDirectory()))
	{
		isValid = false;
		errorMessages.push_back("Game directory does not exist!");
	}
	else
	{
		std::filesystem::path mapToReplacePath = *myGameDirectory;
		mapToReplacePath.append(CustomMapLoader_private::locMapsDirectory).append(*myMapToReplace).make_preferred();

		if (!std::filesystem::exists(mapToReplacePath))
		{
			errorMessages.push_back("Original game map to replace not found!");
			isValid = false;
		}
	}

	if (!std::filesystem::exists(GetCustomMapDirectory()))
	{
		isValid = false;
		errorMessages.push_back("Backup directory does not exist!");
	}

	return isValid;
}

void CustomMapLoader::SetGameDirectory(const std::string& aGameDirectory)
{
	*myGameDirectory = aGameDirectory;
}

void CustomMapLoader::SetCustomMapDirectory(const std::string& aCustomMapDirectory)
{
	*myCustomMapDirectory = aCustomMapDirectory;
}

void CustomMapLoader::SetMapToReplace(const std::string& aMapToReplace)
{
	*myMapToReplace = aMapToReplace;
}

std::filesystem::path CustomMapLoader::GetGameDirectory() const
{
	return std::filesystem::path(*myGameDirectory).make_preferred();
}

std::filesystem::path CustomMapLoader::GetCustomMapDirectory() const
{
	return std::filesystem::path(*myCustomMapDirectory).make_preferred();
}

std::string CustomMapLoader::GetMapToReplace() const
{
	return *myMapToReplace;
}

bool CustomMapLoader::BackupPristineState()
{
	if (CustomMapLoader_private::locIsBackupAvailable(*this))
		return false;

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(*myMapToReplace).make_preferred();

	return  CustomMapLoader_private::locCopyFile(gameFile, CustomMapLoader_private::locGetBackupFilePath(*this));
}

bool CustomMapLoader::RestorePristineState()
{
	if (!CustomMapLoader_private::locIsBackupAvailable(*this))
		return false;

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(*myMapToReplace).make_preferred();

	return CustomMapLoader_private::locCopyFile(CustomMapLoader_private::locGetBackupFilePath(*this), gameFile);
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

	BackupPristineState();

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(CustomMapLoader_private::locMapsDirectory).append(*myMapToReplace).make_preferred();

	return CustomMapLoader_private::locCopyFile(myMaps[anIndex].myMapFile, gameFile);
}

const std::vector<CustomMapLoader::MapInfo>& CustomMapLoader::GetMaps() const
{
	return myMaps;
}

std::int32_t CustomMapLoader::GetCurrentMap() const
{
	return myActiveCustomMapIndex;
}
