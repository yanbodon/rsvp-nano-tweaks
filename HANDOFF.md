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

## OTA v0.1.1-tweaks.2 + Companion Pages candidate

- Empty persisted OTA source resolves to `yanbodon/rsvp-nano-tweaks`; an empty tag builds the exact Rev2 latest-release asset URL.
- Legacy bootstrap source/pin forms for `v0.1.1-tweaks.1` migrate idempotently to default/latest; custom repositories and pins are retained.
- Native and browser Companion provide three OTA presets: `My tweaks`, `Official RSVP Nano`, and validated `Custom GitHub repository`; the tag is optional/advanced.
- Pages is allowlisted to `ionutdecebal/rsvpnano` and `yanbodon/rsvp-nano-tweaks`; PRs build/smoke only, while deployment is restricted to non-PR successful paths. The fork smoke path is `/rsvp-nano-tweaks/`.
- No push, tag, release, deployment, Pages settings change, flash, or device operation was performed.

## Validation handoff and prerequisites

The isolated `.tooling` venv now contains the approved `freetype-py==2.5.1`; `localization/generate_localization.py --check` succeeds. `localization/generate_locale_packs.py --check` reaches Arabic shaping but stops because `uharfbuzz` and `fonttools` are absent. These additional dependencies were not part of the recorded package approval, so they were not installed. `pio`, Java/JDK, and Gradle are not preinstalled.

## Validation update (commits `15a1fc4`, `730eab7`; resumed validation)

- Local tooling was provisioned with `uharfbuzz==0.56.1`, `fonttools==4.65.0`, and `platformio==6.1.19`; local Temurin `17.0.13+11` was downloaded under ignored `.tooling/jdk17`.
- Regenerated all 12 locale-pack ZIP files and reran both generator checks. `localization/test_locale_packs.py` passed 9 tests and `fonts` discovery passed 15 tests.
- The added EPUB filename sanitizer test exposed a native-test source-filter omission. `platformio.ini` includes `src/storage/fs/StoragePaths.cpp` for `native_test` (in `15a1fc4`).
- A serialized SCons native test compilation (`--jobs 1`) completed successfully in the prior validation run, creating `.pio/build/native_test/program`. In the resumed run, the preserved executable was invoked and returned exit code `0` (2 reaction-screensaver tests, 0 failures). This avoids repeating the previously confirmed compilation in the 2 GiB cgroup.
- Waveshare ESP32-S3 Touch LCD 3.49 Rev2 firmware build completed successfully in the prior validation run (exit code `0`). The preserved `.pio/build/waveshare_esp32s3_touch_lcd_349_rev2/firmware.bin` is 2,945,152 bytes, modified `2026-09-11 07:59:14 UTC`, SHA-256 `9ff6b9085ee1b713fdec8de5bc5dab07e7113ab41795ea69c53f7902f2b95310`.
- Companion web task resolution confirms the correct task is `:webApp:wasmJsBrowserProductionWebpack` (not the obsolete `:companion:apps:web:...` path). A first resumed run inherited `org.gradle.jvmargs=-Xmx4g` and was OOM-killed. A second serialized attempt used Temurin 17, `--no-daemon`, `--max-workers=1`, `-Pkotlin.compiler.execution.strategy=in-process`, and an explicit `-Dorg.gradle.jvmargs=-Xmx1200m -Dfile.encoding=UTF-8`; it reached `:webApp:compileProductionExecutableKotlinWasmJs` but was kernel OOM-killed (Gradle exit code `1`; cgroup `oom_kill` increased from 54 to 55). Therefore the Companion production bundle is not validated.

Consequently locale-pack checks, preserved native test execution, and the prior Rev2 firmware build are validated. The Companion production build still cannot be truthfully claimed in this 2 GiB runner; do not release the branch as a fully validated candidate until it succeeds in CI/a runner with sufficient memory. Suggested remaining command:

```sh
JAVA_HOME="$PWD/.tooling/jdk17" ./gradlew :webApp:wasmJsBrowserProductionWebpack --no-daemon --max-workers=1 -Pkotlin.compiler.execution.strategy=in-process
```

## Packaging milestone

- `INTEGRATION_REPORT.md` maps every requested tweak to its v0.1.1 implementation and recorded test evidence; `RELEASE_NOTES.md` documents the local candidate as `v0.1.1-tweaks.1` without creating a tag or release.
- Packaging produces an offline Git bundle containing the complete history through `integration/v0.1.1-tweaks` (and therefore the upstream `v0.1.1` base), plus a binary-safe full patch from `1086e33081aa9c658d3131942c6a0f152b982055` to the candidate tip.
- Fresh bundle clone/restore is verified by checking out the integration branch, confirming the base object and ancestry, running `git diff --check`, and checking that the patch applies with `git apply --check` on the detached base.
- Deliverables are stored outside the repository working tree under `/workspace/t_f3f43353/deliverables/`; `.tooling/` and temporary clone material remain ignored/untracked local tooling only.

## Companion Pages rework (base `87cb05cf8f4153fab6f72ff3efe6fdefced61f7f`)

- The Pages workflow is explicitly restricted to `ionutdecebal/rsvpnano` and `yanbodon/rsvp-nano-tweaks`; no other repository can build, upload, enable, or deploy from this workflow.
- Pull requests run the full `./gradlew checkWeb --no-daemon` and smoke test, but cannot configure, upload, or deploy Pages. Pushes to `main`, successful `Release firmware` workflow completions, and manual dispatches may deploy only after the build and smoke test pass.
- `actions/configure-pages@v6` uses `enablement: true`. This is deliberate: `enablement: false` does not make a first Pages deployment usable. The workflow has the required `pages: write` and `id-token: write` permissions. If an organization policy prevents this action from enabling Pages, a maintainer must explicitly select **GitHub Actions** as the Pages source in repository Settings → Pages, then rerun the workflow; that remote settings change was not made here.
- The fork is validated at the exact project path `/rsvp-nano-tweaks/`; upstream uses `/rsvpnano/`. `tools/check_pages_site.py` verifies served index/PWA assets, `release.json`, and the complete `rsvp-nano-esp32-s3-touch-lcd-3.49-rev2.bin` image—not merely an OTA image.
- Rollback: `git revert <pages-rework-commit>` (or reset to `87cb05cf8f4153fab6f72ff3efe6fdefced61f7f` in an unpublished local branch). No push, Pages setting, deployment, tag, release, or device operation was performed.
