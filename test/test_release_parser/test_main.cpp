#include <unity.h>

#include "update/ReleaseParser.h"

void setUp() {}

void tearDown() {}

void test_split_owner_repo_supports_aliased_input() {
    std::string owner = "yanbodon/rsvp-nano-tweaks";
    std::string repo = "rsvpnano";

    TEST_ASSERT_TRUE(releaseparser::splitOwnerRepo(owner, owner, repo));
    TEST_ASSERT_EQUAL_STRING("yanbodon", owner.c_str());
    TEST_ASSERT_EQUAL_STRING("rsvp-nano-tweaks", repo.c_str());
}

void test_empty_source_follows_tweaks_latest_release() {
    const auto source = releaseparser::sourceForSettings({});
    TEST_ASSERT_EQUAL_STRING("yanbodon", source.owner.c_str());
    TEST_ASSERT_EQUAL_STRING("rsvp-nano-tweaks", source.repo.c_str());
    TEST_ASSERT_TRUE(source.tag.empty());
    const auto url = releaseparser::assetUrlForSource(source, "rsvp-nano-esp32-s3-touch-lcd-3.49-rev2-ota.bin");
    TEST_ASSERT_EQUAL_STRING("https://github.com/yanbodon/rsvp-nano-tweaks/releases/latest/download/"
                             "rsvp-nano-esp32-s3-touch-lcd-3.49-rev2-ota.bin", url.c_str());
}

void test_migration_clears_only_legacy_tweaks_bootstrap() {
    settings::UpdateSettings legacy{"", "yanbodon/rsvp-nano-tweaks@v0.1.1-tweaks.1"};
    TEST_ASSERT_TRUE(releaseparser::migrateTweaksReleaseSettings(legacy));
    TEST_ASSERT_TRUE(legacy.repositoryOwner.empty());
    TEST_ASSERT_TRUE(legacy.releaseTag.empty());
    TEST_ASSERT_TRUE(releaseparser::migrateTweaksReleaseSettings(legacy));

    settings::UpdateSettings custom{"example/custom", "v9"};
    TEST_ASSERT_FALSE(releaseparser::migrateTweaksReleaseSettings(custom));
    TEST_ASSERT_EQUAL_STRING("example/custom", custom.repositoryOwner.c_str());
    TEST_ASSERT_EQUAL_STRING("v9", custom.releaseTag.c_str());
}

void test_extracts_tag_from_asset_redirect() {
    constexpr std::string_view asset = "rsvp-nano-esp32-s3-touch-lcd-3.49-ota.bin";
    const auto tag = releaseparser::tagFromAssetLocation(
        "https://github.com/ionutdecebal/rsvpnano/releases/download/v0.0.9/"
        "rsvp-nano-esp32-s3-touch-lcd-3.49-ota.bin",
        asset);
    TEST_ASSERT_TRUE(tag.has_value());
    TEST_ASSERT_EQUAL_STRING("v0.0.9", tag->c_str());
}

void test_rejects_invalid_asset_redirects() {
    TEST_ASSERT_FALSE(releaseparser::tagFromAssetLocation("https://github.com/releases/latest", "firmware.bin")
                          .has_value());
    TEST_ASSERT_FALSE(releaseparser::tagFromAssetLocation("https://github.com/releases/download/v1/other.bin",
                                                          "firmware.bin")
                          .has_value());
}

void test_builds_version_from_release_tag_and_commit() {
    const auto version =
        releaseparser::versionForCommit("preview-v0.0.9", "0123456789abcdef0123456789abcdef01234567\n");
    TEST_ASSERT_TRUE(version.has_value());
    TEST_ASSERT_EQUAL_STRING("preview-v0.0.9+0123456789ab", version->c_str());
    TEST_ASSERT_FALSE(releaseparser::versionForCommit("v1", "not-a-commit").has_value());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_split_owner_repo_supports_aliased_input);
    RUN_TEST(test_empty_source_follows_tweaks_latest_release);
    RUN_TEST(test_migration_clears_only_legacy_tweaks_bootstrap);
    RUN_TEST(test_extracts_tag_from_asset_redirect);
    RUN_TEST(test_rejects_invalid_asset_redirects);
    RUN_TEST(test_builds_version_from_release_tag_and_commit);
    return UNITY_END();
}
