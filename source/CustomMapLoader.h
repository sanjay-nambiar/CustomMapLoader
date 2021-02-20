#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "CustomMapLoaderPlugin.h"
#include "CustomMapSelectionUI.h"

class ImageWrapper;

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

	struct UIModel
	{
		std::string myTitle;
		std::vector<MapInfo> myMaps;
		std::shared_ptr<std::string> mySelectedMap;
		std::shared_ptr<ImageWrapper> myPlaceholderImage;
		std::vector<std::string> myErrorMessages;
	};

	CustomMapLoader();
	~CustomMapLoader() = default;

	void Initialize(const std::shared_ptr<GameWrapper> aGameWrapper, const std::shared_ptr<CVarManagerWrapper>& aCvarManager, std::shared_ptr<CustomMapSelectionUI> aCustomMapSelectionUI,
		const std::string& aPluginFullName, const std::filesystem::path& aPluginDataDirectory);

	void SetCustomMapDirectory(const std::string& aCustomMapDirectory);
	std::filesystem::path GetCustomMapDirectory() const;

	bool RefreshMaps();
	bool SelectCustomMap(std::int32_t anIndex);
	void LoadSelectedMap();

	void Execute(const std::function<void(GameWrapper*)>& aFunction);

	const CustomMapLoader::UIModel& GetUIModel() const;
private:
	std::shared_ptr<GameWrapper> myGameWrapper;
	std::shared_ptr<CVarManagerWrapper> myCVarManager;
	std::shared_ptr<CustomMapSelectionUI> myCustomMapSelectionUI;

	std::shared_ptr<std::string> myCustomMapDirectory;

	CustomMapLoader::UIModel myModel;
};
