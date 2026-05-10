#pragma once

#include <string>
#include <cstdlib>

namespace ymir::debug::util {

/// @brief Sets an environment variable safely across platforms.
/// @param name The name of the environment variable.
/// @param value The value to set.
inline void EnvSet(const std::string& name, const std::string& value) {
#ifdef _WIN32
    // Windows requires _putenv_s for safe environment manipulation
    _putenv_s(name.c_str(), value.c_str());
#else
    // POSIX standard, 1 means overwrite
    setenv(name.c_str(), value.c_str(), 1);
#endif
}

/// @brief Unsets an environment variable safely across platforms.
/// @param name The name of the environment variable to unset.
inline void EnvUnset(const std::string& name) {
#ifdef _WIN32
    // Windows unsets by assigning an empty string via _putenv_s
    _putenv_s(name.c_str(), "");
#else
    // POSIX standard
    unsetenv(name.c_str());
#endif
}

} // namespace ymir::debug::util
