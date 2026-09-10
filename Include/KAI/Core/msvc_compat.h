#pragma once
// MSVC 2026 compatibility - explicitly include headers that older MSVC provided transitively
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <algorithm>
#include <stdexcept>
#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  ifdef GetObject
#    undef GetObject
#  endif
#  ifdef GetMessage
#    undef GetMessage
#  endif
#  ifdef SendMessage
#    undef SendMessage
#  endif
#endif
