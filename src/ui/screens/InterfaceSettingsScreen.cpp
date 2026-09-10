#include "ui/screens/ScreenCommon.h"

#include "board/BoardDisplay.h"
#include "localization/LocaleCatalog.h"

namespace screens {
    bool InterfaceScreen::begin(ui::Context& ui, settings::InterfaceSettings& config, const locales::Catalog& languages,
                                void (*setBrightness)(uint8_t)) {
        languages_ = &languages;
        if (setBrightness != nullptr)
            setBrightness(config.brightnessPercent);
        ui.setOrientation(config.rotate180 ? Board::Display::rotatedUiOrientation()
                                           : Board::Display::defaultUiOrientation());

        themes.loadFromSd();
        const ui::themes::Theme& selected = themes.resolve(config.selectedThemeId);
        const bool corrected = config.selectedThemeId != selected.id;
        config.selectedThemeId = selected.id;
        ui.setTheme(selected);
        ui.setLocale(config.locale);
        return corrected;
    }

} // namespace screens
