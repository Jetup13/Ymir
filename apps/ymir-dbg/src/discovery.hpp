#pragma once

#include <fmt/format.h>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <ymir/debug/util/path.hpp>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <sys/param.h>
#elif defined(__linux__)
#include <unistd.h>
#include <linux/limits.h>
#endif

namespace ymir::debug {

// Locate the ymir-headless binary using a 4-level priority cascade.
// The first hit wins; the winning path is logged to stderr.
// Returns nullopt if all levels fail — caller should exit with a clear error.
//
// explicit_path is non-empty when the caller passed --headless-path; it short-
// circuits all other levels. If the path does not point to a regular file the
// function returns nullopt immediately rather than falling through, since an
// explicit flag that resolves to nothing is almost certainly a misconfiguration.
inline std::optional<std::filesystem::path> find_headless_binary(std::string_view explicit_path = {}) {
    namespace fs = std::filesystem;
    constexpr const char *kBinaryName = "ymir-headless";

    // Level 1: explicit path (--headless-path flag)
    if (!explicit_path.empty()) {
        fs::path p(explicit_path);
        if (fs::exists(p) && fs::is_regular_file(p)) {
            fmt::print(stderr, "[ymir-dbg] found headless via explicit path: {}\n", p.string());
            return p;
        }
        return std::nullopt;
    }

    // Level 2: YMIR_HEADLESS environment variable
    if (const char *env = std::getenv("YMIR_HEADLESS")) {
        fs::path p(env);
        if (fs::exists(p) && fs::is_regular_file(p)) {
            fmt::print(stderr, "[ymir-dbg] found headless via YMIR_HEADLESS: {}\n", p.string());
            return p;
        }
    }

    // Level 3: PATH lookup
    // Dev-only convenience — a symlink from the build output to ~/bin is the
    // intended workflow. Could collide with a same-named binary on a compromised
    // PATH, but that is not a concern on typical dev machines.
    if (const char *pathEnv = std::getenv("PATH")) {
        // Use our cross-platform utility to correctly handle colons (POSIX) or semicolons (Windows)
        auto segments = ymir::debug::util::SplitSearchPath(pathEnv);
        for (const auto& dir : segments) {
            fs::path candidate = fs::path(dir) / kBinaryName;
            if (fs::exists(candidate) && fs::is_regular_file(candidate)) {
                fmt::print(stderr, "[ymir-dbg] found headless via PATH: {}\n", candidate.string());
                return candidate;
            }
        }
    }

    // Level 4: adjacent binary (same directory as ymir-dbg)
    // Covers installed and packaged layouts where both binaries land in the
    // same prefix/bin directory without any PATH or env configuration.
#if defined(__APPLE__)
    {
        char buf[PATH_MAX] = {};
        uint32_t bufSize = PATH_MAX;
        if (_NSGetExecutablePath(buf, &bufSize) == 0) {
            fs::path exePath(buf);
            std::error_code ec;
            fs::path can = fs::canonical(exePath, ec);
            if (!ec) {
                fs::path adjacent = can.parent_path() / kBinaryName;
                if (fs::exists(adjacent) && fs::is_regular_file(adjacent)) {
                    fmt::print(stderr, "[ymir-dbg] found headless via adjacent binary: {}\n", adjacent.string());
                    return adjacent;
                }
            }
        }
    }
#elif defined(__linux__)
    {
        char buf[PATH_MAX] = {};
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';
            std::error_code ec;
            fs::path can = fs::canonical(fs::path(buf), ec);
            if (!ec) {
                fs::path adjacent = can.parent_path() / kBinaryName;
                if (fs::exists(adjacent) && fs::is_regular_file(adjacent)) {
                    fmt::print(stderr, "[ymir-dbg] found headless via adjacent binary: {}\n", adjacent.string());
                    return adjacent;
                }
            }
        }
    }
#endif

    return std::nullopt;
}

} // namespace ymir::debug
