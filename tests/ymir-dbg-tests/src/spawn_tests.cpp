#include <catch2/catch_test_macros.hpp>

#include <cxxopts.hpp>

#include <string>
#include <vector>

TEST_CASE("spawn: --headless-path flag is parsed", "[spawn]") {
    cxxopts::Options options{"ymir-dbg", "test"};
    // clang-format off
    options.add_options()
        ("headless-path", "path", cxxopts::value<std::string>())
        ("h,help", "help");
    // clang-format on
    options.allow_unrecognised_options();

    std::vector<const char *> argv = {"ymir-dbg", "--headless-path", "/tmp/fake-headless"};
    auto result = options.parse(static_cast<int>(argv.size()), const_cast<char **>(argv.data()));

    CHECK(result.count("headless-path") > 0);
    CHECK(result["headless-path"].as<std::string>() == "/tmp/fake-headless");
}

TEST_CASE("spawn: help flag recognized", "[spawn]") {
    cxxopts::Options options{"ymir-dbg", "test"};
    options.add_options()("h,help", "help");
    options.allow_unrecognised_options();

    std::vector<const char *> argv = {"ymir-dbg", "--help"};
    auto result = options.parse(static_cast<int>(argv.size()), const_cast<char **>(argv.data()));

    CHECK(result.count("help") > 0);
}

TEST_CASE("spawn: unrecognized args captured", "[spawn]") {
    cxxopts::Options options{"ymir-dbg", "test"};
    options.add_options()("h,help", "help");
    options.allow_unrecognised_options();

    std::vector<const char *> argv = {"ymir-dbg", "--extra-flag", "--another"};
    auto result = options.parse(static_cast<int>(argv.size()), const_cast<char **>(argv.data()));

    CHECK(result.unmatched().size() == 2);
    CHECK(result.unmatched()[0] == "--extra-flag");
    CHECK(result.unmatched()[1] == "--another");
}
