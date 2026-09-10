#include "update/ReleaseParser.h"

#include <algorithm>
#include <cctype>
#include <utility>

#include "text/AsciiText.h"

namespace releaseparser {
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
