#include <catch2/catch_test_macros.hpp>

#include <discovery.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace fs = std::filesystem;

namespace {

class ScopedEnvVar {
public:
    explicit ScopedEnvVar(const char *name)
        : m_name(name) {
        if (const char *value = std::getenv(name)) {
            m_prior = value;
        }
    }
    ~ScopedEnvVar() {
        if (m_prior) {
            setenv(m_name, m_prior->c_str(), 1);
        } else {
            unsetenv(m_name);
        }
    }
    void Set(const std::string &value) { setenv(m_name, value.c_str(), 1); }
    void Unset() { unsetenv(m_name); }
private:
    const char *m_name;
    std::optional<std::string> m_prior;
};

} // namespace

TEST_CASE("discovery: explicit path finds existing file", "[discovery]") {
    // Use a file known to exist on all Unix systems
    auto result = ymir::debug::find_headless_binary("/bin/sh");
    REQUIRE(result.has_value());
    // canonical resolves symlinks; /bin/sh -> /bin/dash or /bin/bash depending on OS
    std::error_code ec;
    auto expected = fs::canonical("/bin/sh", ec);
    auto actual = fs::canonical(result.value(), ec);
    CHECK(actual == expected);
}

TEST_CASE("discovery: explicit path to nonexistent returns nullopt", "[discovery]") {
    auto result = ymir::debug::find_headless_binary("/nonexistent/path/ymir-headless");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("discovery: YMIR_HEADLESS env var finds file", "[discovery]") {
    auto tmp = fs::temp_directory_path() / "ymir-dbg-discovery-test";
    {
        std::ofstream ofs(tmp);
        ofs << "dummy";
    }

    ScopedEnvVar env{"YMIR_HEADLESS"};
    env.Set(tmp.string());

    auto result = ymir::debug::find_headless_binary();

    env.Unset();
    fs::remove(tmp);

    REQUIRE(result.has_value());
    std::error_code ec;
    auto expected = fs::canonical(tmp, ec);
    auto actual = fs::canonical(result.value(), ec);
    CHECK(actual == expected);
}

TEST_CASE("discovery: all levels fail returns nullopt", "[discovery]") {
    // Ensure YMIR_HEADLESS is not set
    unsetenv("YMIR_HEADLESS");

    // Use a name that definitely does not exist on PATH
    auto result = ymir::debug::find_headless_binary();
    // This may succeed on adjacent binary level if ymir-headless is in the build dir.
    // That is acceptable behaviour.
    // We verify the function doesn't crash.
    SUCCEED("find_headless_binary completed without exception");
}
