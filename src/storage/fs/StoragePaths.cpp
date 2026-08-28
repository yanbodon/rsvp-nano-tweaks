#include "storage/fs/StoragePaths.h"

#include <algorithm>
#include <iterator>

#include "text/AsciiText.h"
#include "text/UnicodeText.h"
#include "text/Utf8Text.h"

namespace StoragePaths {
    namespace {

        bool hasExtension(std::string_view path, std::string_view extension) {
            return path.size() >= extension.size()
                && std::ranges::equal(path.substr(path.size() - extension.size()), extension, {}, AsciiText::toLower,
                                      AsciiText::toLower);
        }

    } // namespace

    bool hasTextExtension(std::string_view path) {
        return hasExtension(path, kTextExtension);
    }

    bool hasRsvpExtension(std::string_view path) {
        return hasExtension(path, kRsvpExtension);
    }

    bool hasEpubExtension(std::string_view path) {
        return hasExtension(path, kEpubExtension);
    }

    bool hasPdfExtension(std::string_view path) {
        return hasExtension(path, kPdfExtension);
    }

    bool hasConvertibleDocumentExtension(std::string_view path) {
        return hasEpubExtension(path) || hasPdfExtension(path);
    }

    std::string sanitizeFilename(std::string_view name) {
        std::string sanitized;
        sanitized.reserve(name.size());
        while (!name.empty()) {
            const std::string_view remaining = name;
            uint32_t codepoint = 0;
            if (!Utf8Text::decode(name, codepoint)) {
                sanitized += '-';
                continue;
            }

            const size_t codepointBytes = remaining.size() - name.size();
            const auto category = UnicodeText::generalCategory(codepoint);
            const bool combiningMark = category == HB_UNICODE_GENERAL_CATEGORY_SPACING_MARK
                                    || category == HB_UNICODE_GENERAL_CATEGORY_ENCLOSING_MARK
                                    || category == HB_UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK;
            if (UnicodeText::isLetter(codepoint) || UnicodeText::isDigit(codepoint) || combiningMark) {
                sanitized.append(remaining.substr(0, codepointBytes));
            } else if (codepoint == '-' || codepoint == '_' || codepoint == '.' || codepoint == ' ') {
                sanitized += static_cast<char>(codepoint);
            } else if (UnicodeText::isWhitespace(codepoint)) {
                sanitized += ' ';
            } else {
                sanitized += '-';
            }
        }
        sanitized = AsciiText::trim(sanitized);
        const size_t firstVisible = sanitized.find_first_not_of('.');
        return firstVisible == std::string::npos ? std::string{} : sanitized.substr(firstVisible);
    }

    std::string parentDirectoryForPath(std::string_view path) {
        const size_t separator = path.find_last_of('/');
        if (separator == std::string_view::npos || separator == 0) {
            return "/";
        }
        return std::string{path.substr(0, separator)};
    }

    std::string siblingPathWithExtension(std::string_view path, std::string_view extension) {
        const size_t dot = path.find_last_of('.');
        if (dot != std::string_view::npos && dot > 0) {
            path = path.substr(0, dot);
        }
        std::string siblingPath{path};
        siblingPath += extension;
        return siblingPath;
    }

    std::string displayNameForPath(std::string_view path) {
        const size_t separator = path.find_last_of('/');
        return std::string{separator == std::string_view::npos ? path : path.substr(separator + 1)};
    }

    std::string displayNameWithoutExtension(std::string_view path) {
        std::string name = displayNameForPath(path);
        for (const std::string_view extension:
             {std::string_view{kTextExtension}, std::string_view{kRsvpExtension}, std::string_view{kEpubExtension},
              std::string_view{kPdfExtension}}) {
            if (hasExtension(name, extension)) {
                name.resize(name.size() - extension.size());
                break;
            }
        }
        return name;
    }

    std::string rsvpCachePathForDocument(std::string_view documentPath) {
        return siblingPathWithExtension(documentPath, kRsvpExtension);
    }

    std::string indexedIndexPathFor(std::string_view path) {
        return std::string{path} + kIndexExtension;
    }

    std::string indexedDataPathFor(std::string_view path) {
        return std::string{path} + kDataExtension;
    }

    std::string bookStatePathFor(std::string_view path) {
        return siblingPathWithExtension(path, kBookStateExtension);
    }

    std::string indexedTempPathFor(std::string_view path) {
        return std::string{path} + kTempExtension;
    }

    bool isHiddenOrSidecarPath(std::string_view path) {
        const std::string name = displayNameForPath(path);
        if (name.empty()) {
            return true;
        }

        if (name.starts_with('.')) {
            return true;
        }

        if (hasExtension(name, kIndexExtension) || hasExtension(name, kDataExtension)
            || hasExtension(name, kBookStateExtension) || hasExtension(name, kTempExtension)) {
            return true;
        }

        return std::ranges::equal(name, "thumbs.db", {}, AsciiText::toLower, AsciiText::toLower)
            || std::ranges::equal(name, "desktop.ini", {}, AsciiText::toLower, AsciiText::toLower);
    }

} // namespace StoragePaths
