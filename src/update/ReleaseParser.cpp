#include "update/ReleaseParser.h"

#include <algorithm>
#include <cctype>
#include <utility>

#include "text/AsciiText.h"

namespace releaseparser {
    namespace {
        bool isDefaultSource(std::string_view owner) {
            owner = AsciiText::trim(owner);
            return owner.empty() || owner == settings::kDefaultRepositoryOwner
                || owner == std::string{settings::kDefaultRepositoryOwner} + "/" +
                                std::string{settings::kDefaultRepositoryName};
        }

        bool isLegacyTweaksPin(std::string_view tag) {
            tag = AsciiText::trim(tag);
            return tag == "v0.1.1-tweaks.1"
                || tag == std::string{settings::kDefaultRepositoryOwner} + "/" +
                              std::string{settings::kDefaultRepositoryName} + "@v0.1.1-tweaks.1"
                || tag == std::string{settings::kDefaultRepositoryName} + "@v0.1.1-tweaks.1";
        }
    } // namespace

    ReleaseSource sourceForSettings(const settings::UpdateSettings& settings) {
        ReleaseSource source{settings.repositoryOwner.empty() ? std::string{settings::kDefaultRepositoryOwner}
                                                              : std::string{AsciiText::trim(settings.repositoryOwner)},
                             std::string{settings::kDefaultRepositoryName},
                             std::string{AsciiText::trim(settings.releaseTag)}};
        splitOwnerRepo(source.owner, source.owner, source.repo);
        splitOwnerRepo(source.repo, source.owner, source.repo);
        const size_t at = source.tag.find('@');
        if (at > 0 && at + 1 < source.tag.length()) {
            std::string repoPart{AsciiText::trim(std::string_view{source.tag}.substr(0, at))};
            source.tag = std::string{AsciiText::trim(std::string_view{source.tag}.substr(at + 1))};
            if (!splitOwnerRepo(repoPart, source.owner, source.repo) && !repoPart.empty())
                source.repo = repoPart;
        }
        return source;
    }

    std::string assetUrlForSource(const ReleaseSource& source, std::string_view assetName) {
        std::string url = "https://github.com/" + source.owner + "/" + source.repo + "/releases/";
        if (source.tag.empty()) return url + "latest/download/" + std::string{assetName};
        return url + "download/" + source.tag + "/" + std::string{assetName};
    }

    bool migrateTweaksReleaseSettings(settings::UpdateSettings& settings) {
        if (!isDefaultSource(settings.repositoryOwner)) return false;
        if (!settings.releaseTag.empty() && !isLegacyTweaksPin(settings.releaseTag)) return false;
        settings.repositoryOwner.clear();
        settings.releaseTag.clear();
        return true;
    }

    bool splitOwnerRepo(std::string_view value, std::string& owner, std::string& repo) {
        const std::string_view trimmed = AsciiText::trim(value);
        const size_t slash = trimmed.find('/');
        if (slash == 0 || slash == std::string_view::npos || slash + 1 >= trimmed.length()) {
            return false;
        }

        // Keep parsed values independent of `value`: an output string may
        // also be the storage backing the input view.
        std::string parsedOwner{AsciiText::trim(trimmed.substr(0, slash))};
        std::string parsedRepo{AsciiText::trim(trimmed.substr(slash + 1))};
        if (parsedOwner.empty() || parsedRepo.empty()) {
            return false;
        }

        owner = std::move(parsedOwner);
        repo = std::move(parsedRepo);
        return true;
    }

    std::expected<std::string, std::error_code> tagFromAssetLocation(std::string_view location,
                                                                    std::string_view assetName) {
        constexpr std::string_view marker = "/releases/download/";
        const size_t start = location.find(marker);
        if (start == std::string_view::npos || assetName.empty() || !location.ends_with(assetName)) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        const size_t tagStart = start + marker.size();
        const size_t assetStart = location.size() - assetName.size();
        if (assetStart <= tagStart || location[assetStart - 1] != '/')
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));

        const std::string_view tag = location.substr(tagStart, assetStart - tagStart - 1);
        if (tag.empty() || tag.contains('/'))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        return std::string{tag};
    }

    std::expected<std::string, std::error_code> versionForCommit(std::string_view tagName, std::string_view commitSha) {
        commitSha = AsciiText::trim(commitSha);
        if (tagName.empty() || commitSha.length() != 40) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        if (!std::ranges::all_of(commitSha, [](char c) {
                return std::isxdigit(static_cast<unsigned char>(c));
            })) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        std::string version;
        version.reserve(tagName.size() + 13);
        version.append(tagName).append("+").append(commitSha.substr(0, 12));
        return version;
    }

} // namespace releaseparser
