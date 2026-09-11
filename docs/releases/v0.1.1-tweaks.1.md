# RSVP Nano v0.1.1-tweaks.1

Integration release based on upstream RSVP Nano `v0.1.1` (`1086e33081aa9c658d3131942c6a0f152b982055`).

## Included changes

- Preserve custom OTA repository overrides safely, including `yanbodon/rsvp-nano-tweaks` parsing without input-string corruption.
- Support Polish/Unicode EPUB filenames through safe filesystem sanitization, with native and Companion parity coverage.
- Handle supported EPUB embedded-font obfuscation consistently in firmware and Companion conversion paths.
- Add a persisted 180-degree screen rotation setting for current regular and watch firmware interfaces and Companion settings.
- Refresh rotation labels without the unsupported degree glyph (`Rotate 180` and `Rotate screen 180 degrees`).
- Regenerate firmware localization output and all 12 locale packs from canonical localization sources.
- Ensure the native-test source filter includes filename-sanitizer coverage.

## Validation

GitHub Actions passed on the integration branch for:

- web Companion checks and production bundle,
- generated localization and locale packs,
- native firmware tests,
- Android checks and release APK build,
- iOS checks and application build.

Local validation also passed for the Waveshare ESP32-S3 Touch LCD 3.49 Rev2 firmware build, locale-pack tests, font tests, whitespace checks, bundle restoration, patch applicability, and Git history integrity.

## Upgrade and rollback

This release is based on upstream `v0.1.1`. The pre-integration fork state remains available at commit `e7d60b863f861124c8a212e618405f1e99e8f7a6` and through the repository history.
