# HANDOFF — RSVP Nano v0.1.1-tweaks integration

Status: implementation complete and handed to the validation lane; no remote change, push, PR, release, flash, or device operation was performed.

## Base and rollback

- Integration branch: `integration/v0.1.1-tweaks`
- Base: upstream v0.1.1 `1086e33081aa9c658d3131942c6a0f152b982055`
- Local commits: `f2085ac` (`fix(ota): preserve custom owner/repository parsing`) and `f79df21` (`feat: port EPUB and rotation tweaks to v0.1.1`).
- Rollback: `git switch integration/v0.1.1-tweaks && git reset --hard 1086e33081aa9c658d3131942c6a0f152b982055`
- Original fork remains at `fork/main` = `e7d60b863f861124c8a212e618405f1e99e8f7a6`.

## Implemented changes awaiting complete candidate validation

- OTA owner/repository splitting was moved to `releaseparser::splitOwnerRepo`, using independent temporary strings so that aliased `std::string_view` input/output cannot corrupt parsing. Test explicitly uses `yanbodon/rsvp-nano-tweaks`.
- Polish/Unicode EPUB filename sanitation and supported embedded-font obfuscation behavior were applied with native and Kotlin parity tests.
- `rotate180` settings/model/Companion API changes were applied. The obsolete screen implementation conflict was resolved by adding the setting to the current upstream `regular/` and `watch/` screen variants, while startup of the interface screen restores the selected orientation.
- The Rotate 180 UI strings use `Rotate 180` / `Rotate screen 180 degrees` and avoid the unsupported degree glyph; `Localization.generated.cpp` was regenerated.

## Validation handoff and prerequisites

The isolated `.tooling` venv now contains the approved `freetype-py==2.5.1`; `localization/generate_localization.py --check` succeeds. `localization/generate_locale_packs.py --check` reaches Arabic shaping but stops because `uharfbuzz` and `fonttools` are absent. These additional dependencies were not part of the recorded package approval, so they were not installed. `pio`, Java/JDK, and Gradle are not preinstalled.

## Validation update (commit `15a1fc4`)

- Local tooling was provisioned with `uharfbuzz==0.56.1`, `fonttools==4.65.0`, and `platformio==6.1.19`; local Temurin `17.0.13+11` was also downloaded under ignored `.tooling/jdk17`.
- Regenerated all 12 locale-pack ZIP files and reran both generator checks. `localization/test_locale_packs.py` passed 9 tests and `fonts` discovery passed 15 tests.
- The added EPUB filename sanitizer test exposed a native-test source-filter omission. `platformio.ini` now includes `src/storage/fs/StoragePaths.cpp` for `native_test` (in `15a1fc4`).
- Full native tests remain unvalidated in this constrained runner: PlatformIO launches 12 concurrent C++ compilers for `pio test` and the kernel kills `cc1plus` processes. PlatformIO 6.1.19 provides no `pio test --jobs` option. Attempts to use a temporary SCons one-job extra script disrupted PlatformIO's Glaze dependency resolution and were discarded.
- Full Waveshare Rev2 and `checkWeb` attempts were also not completed: this runner's background subprocess wrapper terminated each after 10 seconds. Do not claim firmware or Companion build success without rerunning in CI/a runner that permits bounded long jobs.

Consequently the locale ZIP packs are regenerated and their determinism checks pass. Native/OTA, firmware, and Gradle web/Companion builds still cannot be truthfully claimed. Do not release the branch as a candidate until they have been rerun successfully in a suitable runner. The validation lane must run:

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
