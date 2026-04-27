extern "C" {
    #include "dlls/chromium/util.h"
}
#include <gtest/gtest.h>


TEST(ExtractWsUrl, ValidJson) {
    const char *json = R"({"webSocketDebuggerUrl":"ws://localhost:9222/devtools/page/abc"})";
    char buf[256];
    EXPECT_TRUE(extract_ws_url_internal(buf, sizeof(buf), json));
    EXPECT_STREQ(buf, "ws://localhost:9222/devtools/page/abc");
}

TEST(ExtractWsUrl, MissingWebSocketField) {
    const char *json = R"({"other":"ws://localhost:9222"})";
    char buf[256];
    EXPECT_FALSE(extract_ws_url_internal(buf, sizeof(buf), json));
}

TEST(ExtractWsUrl, MissingWsPrefix) {
    const char *json = R"({"webSocketDebuggerUrl":"http://localhost:9222"})";
    char buf[256];
    EXPECT_FALSE(extract_ws_url_internal(buf, sizeof(buf), json));
}

TEST(ExtractWsUrl, BufferTooSmall) {
    const char *json = R"({"webSocketDebuggerUrl":"ws://localhost:9222"})";
    char buf[5];
    EXPECT_FALSE(extract_ws_url_internal(buf, sizeof(buf), json));
}

TEST(ExtractWsUrl, EmptyInput) {
    char buf[256];
    EXPECT_FALSE(extract_ws_url_internal(buf, sizeof(buf), ""));
}
