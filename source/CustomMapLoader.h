#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class CustomMapLoader
{
public:
	struct MapInfo
	{
		std::string myTitle;
		std::string myLastUpdated;
		std::filesystem::path myMapFile;
		std::filesystem::path myPreviewImage;
	};

	CustomMapLoader();
	~CustomMapLoader() = default;

	const std::filesystem::path& GetGameDirectory() const { return myGameDirectory; }
	const std::filesystem::path& GetBackupDirectory() const { return myBackupDirectory; }
	void SetGameDirectory(const std::filesystem::path& aDirectory) { myGameDirectory = aDirectory; }
	void SetBackupDirectory(const std::filesystem::path& aDirectory) { myBackupDirectory = aDirectory; }

	bool BackupPristineState();
	bool RestorePristineState();

	bool RefreshMaps(const std::string& aMapDirectory);

	const std::vector<MapInfo>& GetMaps() { return myMaps; }

	bool SetCurrentMap(std::int32_t anIndex);
	std::int32_t GetCurrentMap() const { return myActiveCustomMap; }
private:
	std::filesystem::path myGameDirectory;
	std::filesystem::path myBackupDirectory;

	bool myIsBackupAlive;

	bool myIsMapsRefreshed;
	std::vector<MapInfo> myMaps;
	std::int32_t myActiveCustomMap;
};
