#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "CustomMapLoaderPlugin.h"
#include "CustomMapSelectionUI.h"

class CustomMapLoader
{
public:
	friend class CustomMapLoaderPlugin;
	friend class CustomMapSelectionUI;

	struct MapInfo
	{
		std::string myTitle;
		std::string myLastUpdated;
		std::filesystem::path myMapFile;
		std::shared_ptr<ImageWrapper> myPreviewImage;
	};

	CustomMapLoader();
	~CustomMapLoader() = default;

	void SetGameDirectory(const std::string& aGameDirectory);
	void SetCustomMapDirectory(const std::string& aCustomMapDirectory);
	void SetMapToReplace(const std::string& aMapToReplace);

	std::filesystem::path GetGameDirectory() const;
	std::filesystem::path GetCustomMapDirectory() const;
	std::string GetMapToReplace() const;

	bool ValidateDirectories(std::vector<std::string>& errorMessages);

	bool BackupPristineState();
	bool RestorePristineState();

	bool RefreshMaps();
	bool LoadMap(std::int32_t anIndex);

	const std::vector<MapInfo>& GetMaps() const;
	std::int32_t GetCurrentMap() const;

	const std::vector<std::string>& GetValues() const { return values; }
private:
	std::shared_ptr<std::string> myGameDirectory;
	std::shared_ptr<std::string> myCustomMapDirectory;
	std::shared_ptr<std::string> myMapToReplace;
	std::shared_ptr<std::string> myActiveCustomMap;

	std::vector<MapInfo> myMaps;
	std::int32_t myActiveCustomMapIndex;

	std::vector<std::string> values;
};
