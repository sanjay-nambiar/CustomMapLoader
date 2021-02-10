#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "CustomMapLoaderPlugin.h"

class CustomMapLoader
{
public:
	friend class CustomMapLoaderPlugin;

	struct MapInfo
	{
		std::string myTitle;
		std::string myLastUpdated;
		std::filesystem::path myMapFile;
		std::filesystem::path myPreviewImage;
	};

	CustomMapLoader();
	~CustomMapLoader() = default;

	std::filesystem::path GetGameDirectory() const;
	std::filesystem::path GetCustomMapDirectory() const;
	std::string GetMapToReplace() const;

	bool ValdiateDirectories(std::vector<std::string>& errorMessages);

	bool BackupPristineState();
	bool RestorePristineState();

	bool RefreshMaps();

	const std::vector<MapInfo>& GetMaps() { return myMaps; }

	bool LoadMap(std::int32_t anIndex);
	std::int32_t GetCurrentMap() const { return myActiveCustomMap; }
private:
	std::shared_ptr<std::string> myGameDirectory;
	std::shared_ptr<std::string> myCustomMapDirectory;
	std::shared_ptr<std::string> myMapToReplace;

	bool myIsBackupAlive;

	bool myIsMapsRefreshed;
	std::vector<MapInfo> myMaps;
	std::int32_t myActiveCustomMap;
};
