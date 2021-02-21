
function Setup-Dependencies
{
	param
	(
		[Parameter(Mandatory=$true)] [string]$ThirdpartyDir
	)

	if (Test-Path -Path "$ThirdpartyDir/imgui" -PathType Container) { Remove-Item -LiteralPath "$ThirdpartyDir/imgui" -Force -Recurse }
	Expand-Archive -Path "$ThirdpartyDir/BakkesModSDK/imgui_bm.zip" -DestinationPath "$ThirdpartyDir/"
}

function Generate-Artifact
{
	param
	(
		[Parameter(Mandatory=$true)] [string]$PluginName,
		[Parameter(Mandatory=$true)] [string]$ArtifactDir,
		[Parameter(Mandatory=$true)] [string]$AssemblyDir,
		[Parameter(Mandatory=$true)] [string]$ConfigDir
	)

	$WorkDir = "$ArtifactDir/tmp"

	if (Test-Path -Path $WorkDir -PathType Container) { Remove-Item -LiteralPath $WorkDir -Force -Recurse }
	New-Item -ItemType Directory -Force -Path $WorkDir

	Remove-Item -LiteralPath "$ArtifactDir/$PluginName.zip" -Force

	New-Item -ItemType Directory -Force -Path "$WorkDir/plugins/settings"
	Copy-Item "$AssemblyDir/$PluginName.dll" -Destination "$WorkDir/plugins"
	Copy-Item -Path "$ConfigDir/$PluginName.set" -Destination "$WorkDir/plugins/settings"

	New-Item -ItemType Directory -Force -Path "$WorkDir/data/$PluginName"
	Copy-Item -Path "$ConfigDir/placeholder.jpg" -Destination "$WorkDir/data/$PluginName"

	Compress-Archive -Path "$WorkDir/*" -DestinationPath "$ArtifactDir/$PluginName.zip"

	Remove-Item -LiteralPath $WorkDir -Force -Recurse
}
