#include "CustomMapLoader.h"

#include <filesystem>
#include <fstream>

namespace CustomMapLoader_private
{
	static const char* locMapsDirectory = "TAGame/CookedPCConsole";

	CustomMapLoader::MapInfo locGetMapDetails(const std::filesystem::directory_entry& aMapDirectory)
	{
		CustomMapLoader::MapInfo info;

		for (const auto& entry : std::filesystem::directory_iterator(aMapDirectory))
		{
			if (!entry.is_regular_file())
				continue;

			const std::filesystem::path& filepath = entry.path().filename();
			if (filepath.compare("info.txt") == 0)
			{
				std::ifstream infoFile(filepath);
				std::string line;
				for (std::uint32_t i = 0; i < 2 && std::getline(infoFile, line); ++i)
				{
					if (i == 0)
						info.myTitle = line;
					else
						info.myLastUpdated = line;
				}
			}
			else if (filepath.compare("preview.jpg") == 0)
			{
				info.myPreviewImage = filepath;
			}
			else if (filepath.compare("map.udk") == 0)
			{
				info.myMapFile = filepath;
			}
		}

		return info;
	}

	bool locCopyFile(const std::filesystem::path& aSourcePath, const std::filesystem::path& aDestPath, bool aShouldOverwrite = true)
	{
		return std::filesystem::copy_file(aSourcePath, aDestPath, aShouldOverwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none);
	}
}

CustomMapLoader::CustomMapLoader()
	: myIsBackupAlive(false)
	, myIsMapsRefreshed(false)
	, myActiveCustomMap(-1)
{
	myGameDirectory = std::make_shared<std::string>();
	myCustomMapDirectory = std::make_shared<std::string>();
	myMapToReplace = std::make_shared<std::string>();
}

bool CustomMapLoader::ValdiateDirectories(std::vector<std::string>& errorMessages)
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
	if (myIsBackupAlive)
		return true;

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(*myMapToReplace).make_preferred();

	std::filesystem::path backupFile = *myGameDirectory;
	backupFile.append(*myMapToReplace).append(".bak").make_preferred();

	myIsBackupAlive = CustomMapLoader_private::locCopyFile(gameFile, backupFile);
	return myIsBackupAlive;
}

bool CustomMapLoader::RestorePristineState()
{
	if (!myIsBackupAlive)
		return false;

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(*myMapToReplace).make_preferred();

	std::filesystem::path backupFile = *myGameDirectory;
	backupFile.append(*myMapToReplace).append(".bak").make_preferred();

	myIsBackupAlive = !CustomMapLoader_private::locCopyFile(backupFile, gameFile);
	return myIsBackupAlive;
}

bool CustomMapLoader::RefreshMaps()
{
	if (!std::filesystem::exists(GetCustomMapDirectory()))
		return false;

	myMaps.erase(myMaps.begin(), myMaps.end());

	for (const auto& entry : std::filesystem::directory_iterator(GetCustomMapDirectory()))
	{
		if (!entry.is_directory())
			continue;

		myMaps.push_back(CustomMapLoader_private::locGetMapDetails(entry));
	}

	return true;
}

bool CustomMapLoader::LoadMap(std::int32_t anIndex)
{
	if (!myIsBackupAlive || (anIndex > 0 && anIndex >= myMaps.size()))
		return false;

	std::filesystem::path gameFile = *myGameDirectory;
	gameFile.append(CustomMapLoader_private::locMapsDirectory).append(*myMapToReplace).make_preferred();

	return CustomMapLoader_private::locCopyFile(myMaps[anIndex].myMapFile, gameFile);
}