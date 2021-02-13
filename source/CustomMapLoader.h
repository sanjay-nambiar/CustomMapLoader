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

	struct MapInfo
	{
		std::string myTitle;
		std::string myLastUpdated;
		std::filesystem::path myMapFile;
		std::shared_ptr<ImageWrapper> myPreviewImage;
	};

	CustomMapLoader();
	~CustomMapLoader() = default;

	void InitializeDependencies(const std::shared_ptr<CVarManagerWrapper>& aCvarManager, std::shared_ptr<CustomMapSelectionUI> aCustomMapSelectionUI,
		const std::string& aPluginFullName);

	void SetCustomMapDirectory(const std::string& aCustomMapDirectory);
	std::filesystem::path GetCustomMapDirectory() const;

	bool ValidateDirectories(std::vector<std::string>& errorMessages);

	void LoadPlaceholderImage(const std::filesystem::path& aPluginDataDirectory);
	std::shared_ptr<ImageWrapper> GetPlaceholderImage() const;

	bool RefreshMaps();
	bool LoadMap(std::int32_t anIndex);

	const std::vector<MapInfo>& GetMaps() const;
private:
	std::shared_ptr<CVarManagerWrapper> myCVarManager;
	std::shared_ptr<CustomMapSelectionUI> myCustomMapSelectionUI;

	std::shared_ptr<std::string> myCustomMapDirectory;

	std::vector<MapInfo> myMaps;
};
