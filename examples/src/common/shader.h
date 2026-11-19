#ifndef VK_TOOLKIT_SHADER_H
#define VK_TOOLKIT_SHADER_H

#include <filesystem>
#include <string>



static std::string vkToolkitGetShaderPath(
	const char* pInstalledDataDirectory,
	const char* pInstalledRelativePath,
	const std::filesystem::path& sourcePath
) {
	if (pInstalledDataDirectory != NULL && pInstalledDataDirectory[0] != '\0') {
		return (
			std::filesystem::path(pInstalledDataDirectory) /
			pInstalledRelativePath
		).lexically_normal().string();
	}

	return sourcePath.lexically_normal().string();
}

#endif//VK_TOOLKIT_SHADER_H
