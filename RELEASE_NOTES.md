# RSVP Nano v0.1.1-tweaks.1

Local integration candidate based on upstream RSVP Nano `v0.1.1` (`1086e33081aa9c658d3131942c6a0f152b982055`). This is a local, untagged release-note document: no GitHub release or remote tag has been created.

## Included changes

- Preserve custom OTA repository overrides safely, including `yanbodon/rsvp-nano-tweaks` parsing without input-string corruption.
- Support Polish/Unicode EPUB filenames through safe filesystem sanitization, with native and Companion parity coverage.
- Handle supported EPUB embedded-font obfuscation consistently in firmware and Companion conversion paths.
- Add a persisted 180-degree screen rotation setting for current regular and watch firmware interfaces and Companion settings.
- Refresh rotation labels without the unsupported degree glyph (`Rotate 180` and `Rotate screen 180 degrees`).
- Regenerate firmware localization output and all 12 locale packs from canonical localization sources.
- Ensure the native-test source filter includes filename-sanitizer coverage.

## Validation status

Passed locally: diff whitespace validation; localization and locale-pack generation checks; 9 locale-pack tests; 15 font tests; execution of the preserved native test binary (2 tests, 0 failures); and integrity verification of the preserved Waveshare ESP32-S3 Touch LCD 3.49 Rev2 firmware artifact.

Known limitation: the Companion production WebAssembly bundle was not validated because Gradle was kernel OOM-killed during Kotlin/Wasm compilation on this 2 GiB runner, even serialized with a 1200 MiB heap. Run the production bundle in CI or on a higher-memory runner before calling this a fully validated release.

## Upgrade and rollback

This candidate is based on upstream `v0.1.1`. To discard the local integration branch and return it to the base:

```sh
git switch integration/v0.1.1-tweaks
git reset --hard 1086e33081aa9c658d3131942c6a0f152b982055
```

The pre-integration fork reference is retained at `fork/main` (`e7d60b863f861124c8a212e618405f1e99e8f7a6`).
