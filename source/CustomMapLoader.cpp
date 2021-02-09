#include "CustomMapLoader.h"

#include <filesystem>
#include <fstream>

namespace CustomMapLoader_private
{
	static std::filesystem::path locMapsDirectory = std::filesystem::path("TAGame/CookedPCConsole", std::filesystem::path::native_format);
	static std::string locPristineMapToReplace = "Labs_Underpass_P.upk";

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
}

bool CustomMapLoader::BackupPristineState()
{
	if (myIsBackupAlive)
		return true;

	std::filesystem::path gameFile = myGameDirectory;
	gameFile.append(CustomMapLoader_private::locPristineMapToReplace);

	std::filesystem::path backupFile = myBackupDirectory;
	backupFile.append(CustomMapLoader_private::locPristineMapToReplace);

	myIsBackupAlive = CustomMapLoader_private::locCopyFile(gameFile, backupFile);
	return myIsBackupAlive;
}

bool CustomMapLoader::RestorePristineState()
{
	if (!myIsBackupAlive)
		return false;

	std::filesystem::path gameFile = myGameDirectory;
	gameFile.append(CustomMapLoader_private::locPristineMapToReplace);

	std::filesystem::path backupFile = myBackupDirectory;
	backupFile.append(CustomMapLoader_private::locPristineMapToReplace);

	myIsBackupAlive = !CustomMapLoader_private::locCopyFile(backupFile, gameFile);
	return myIsBackupAlive;
}

bool CustomMapLoader::RefreshMaps(const std::string& aMapsDirectory)
{
	if (!std::filesystem::exists(aMapsDirectory))
		return false;

	myMaps.erase(myMaps.begin(), myMaps.end());

	for (const auto& entry : std::filesystem::directory_iterator(aMapsDirectory))
	{
		if (!entry.is_directory())
			continue;

		myMaps.push_back(CustomMapLoader_private::locGetMapDetails(entry));
	}

	return true;
}

bool CustomMapLoader::SetCurrentMap(std::int32_t anIndex)
{
	if (!myIsBackupAlive || (anIndex > 0 && anIndex >= myMaps.size()))
		return false;

	std::filesystem::path gameFile = myGameDirectory;
	gameFile.append(CustomMapLoader_private::locPristineMapToReplace);

	return CustomMapLoader_private::locCopyFile(myMaps[anIndex].myMapFile, gameFile);
}