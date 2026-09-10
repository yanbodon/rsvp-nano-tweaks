# HANDOFF — RSVP Nano v0.1.1-tweaks integration

Status: blocked before candidate validation; no remote change, push, PR, release, flash, or device operation was performed.

## Base and rollback

- Integration branch: `integration/v0.1.1-tweaks`
- Base: upstream v0.1.1 `1086e33081aa9c658d3131942c6a0f152b982055`
- First completed local commit: `f2085ac` (`fix(ota): preserve custom owner/repository parsing`)
- Rollback: `git switch integration/v0.1.1-tweaks && git reset --hard 1086e33081aa9c658d3131942c6a0f152b982055`
- Original fork remains at `fork/main` = `e7d60b863f861124c8a212e618405f1e99e8f7a6`.

## Ported working tree changes awaiting locale-pack generation and validation

- OTA owner/repository splitting was moved to `releaseparser::splitOwnerRepo`, using independent temporary strings so that aliased `std::string_view` input/output cannot corrupt parsing. Test explicitly uses `yanbodon/rsvp-nano-tweaks`.
- Polish/Unicode EPUB filename sanitation and supported embedded-font obfuscation behavior were applied with native and Kotlin parity tests.
- `rotate180` settings/model/Companion API changes were applied. The obsolete screen implementation conflict was resolved by adding the setting to the current upstream `regular/` and `watch/` screen variants, while startup of the interface screen restores the selected orientation.
- The Rotate 180 UI strings were changed to avoid the unsupported degree glyph; `Localization.generated.cpp` was regenerated.

## Blocking validation prerequisite

`localization/generate_locale_packs.py` requires `freetype-py`. A pinned local tooling setup was attempted with `uv pip install freetype-py==2.5.1 platformio==6.1.19`, but the environment's required threat-intelligence scan could not complete (OSV/deps.dev/ecosyste.ms deadlines) and single-query policy denied the install. `pio`, Java/JDK, and Gradle are not preinstalled.

Consequently locale ZIP packs cannot yet be regenerated, and native/OTA, firmware, and Gradle web/Companion builds cannot be truthfully claimed. Do not release the currently dirty branch as a candidate. After an approved/preprovisioned tooling environment is available, run:

```sh
.tooling/bin/python localization/generate_localization.py
.tooling/bin/python localization/generate_locale_packs.py
.tooling/bin/python localization/generate_localization.py --check
.tooling/bin/python localization/generate_locale_packs.py --check
.tooling/bin/pio test -e native_test
.tooling/bin/pio test -e native_watch_test
.tooling/bin/pio run -e waveshare_esp32s3_touch_lcd_349_rev2
./gradlew :companion:apps:web:jsBrowserProductionWebpack
```
