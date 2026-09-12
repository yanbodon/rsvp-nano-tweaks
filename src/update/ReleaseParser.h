#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <system_error>

#include "settings/SettingsModel.h"

// Pure parsing of GitHub release metadata. No networking or SD access.
namespace releaseparser {

    struct ReleaseSource {
        std::string owner;
        std::string repo;
        std::string tag;
    };

    ReleaseSource sourceForSettings(const settings::UpdateSettings& settings);
    std::string assetUrlForSource(const ReleaseSource& source, std::string_view assetName);
    bool migrateTweaksReleaseSettings(settings::UpdateSettings& settings);

    bool splitOwnerRepo(std::string_view value, std::string& owner, std::string& repo);

    std::expected<std::string, std::error_code> tagFromAssetLocation(std::string_view location,
                                                                    std::string_view assetName);

    // Published builds use the release tag plus a stable abbreviated commit.
    std::expected<std::string, std::error_code> versionForCommit(std::string_view tagName, std::string_view commitSha);

} // namespace releaseparser
