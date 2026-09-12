#pragma once

#include <string>

#include "KAI/Core/Config/Base.h"

KAI_BEGIN

// Backtick shell execution (the ENABLE_SHELL_SYNTAX feature) is written
// assuming a POSIX shell: echo/grep/wc/sed/cut/tr/uname/date/expr/printf/pwd,
// pipes, simple commands, etc. That is true unconditionally on Linux/macOS,
// and equally true when this binary itself is *built and run inside* WSL2
// (WSL2 is just Linux at that point, popen() already calls a real /bin/sh,
// nothing to do). It is not true for a binary built for native Windows:
// MSVC's CRT does provide `popen`/`pclose` (as deprecated aliases for
// `_popen`/`_pclose`), so code calling them compiles, but the process they
// spawn is `cmd.exe`, not a POSIX shell - which is why commands like
// `printf`/`pwd` fail there with "is not recognized as an internal or
// external command" even though the exact same command text works fine on
// Linux/macOS/WSL2. Rather than reimplementing every POSIX command's
// semantics in terms of cmd.exe/PowerShell, route the command through
// WSL2's own bash on native Windows, so the exact same command text runs
// identically everywhere. This requires a WSL2 distro with bash and
// coreutils installed and `wsl` reachable on PATH; that is what
// ENABLE_SHELL_SYNTAX=ON now depends on for a native Windows build.
//
// Every call site that executes a backtick/shell command - Console.cpp's
// REPL-level handling *and* ExecutorPerform.cpp's Operation::ShellCommand
// VM opcode, which is the actual runtime path for backtick expressions
// compiled out of a Pi/Rho script - must go through KAI_POPEN/KAI_PCLOSE
// with a command run through ToPlatformShellCommand() first, or it will
// silently fall back to cmd.exe on Windows and fail exactly like this.
#ifdef _WIN32
#define KAI_POPEN _popen
#define KAI_PCLOSE _pclose
#else
#define KAI_POPEN popen
#define KAI_PCLOSE pclose
#endif

#ifdef _WIN32
namespace PlatformShellDetail {
// Minimal, dependency-free base64 encoder - see the comment in
// ToPlatformShellCommand() below for why this exists.
inline std::string Base64Encode(const std::string &data) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((data.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 3 <= data.size()) {
        unsigned char b0 = static_cast<unsigned char>(data[i]);
        unsigned char b1 = static_cast<unsigned char>(data[i + 1]);
        unsigned char b2 = static_cast<unsigned char>(data[i + 2]);
        out += table[b0 >> 2];
        out += table[((b0 & 0x3) << 4) | (b1 >> 4)];
        out += table[((b1 & 0xF) << 2) | (b2 >> 6)];
        out += table[b2 & 0x3F];
        i += 3;
    }

    size_t rem = data.size() - i;
    if (rem == 1) {
        unsigned char b0 = static_cast<unsigned char>(data[i]);
        out += table[b0 >> 2];
        out += table[(b0 & 0x3) << 4];
        out += "==";
    } else if (rem == 2) {
        unsigned char b0 = static_cast<unsigned char>(data[i]);
        unsigned char b1 = static_cast<unsigned char>(data[i + 1]);
        out += table[b0 >> 2];
        out += table[((b0 & 0x3) << 4) | (b1 >> 4)];
        out += table[(b1 & 0xF) << 2];
        out += "=";
    }

    return out;
}
}  // namespace PlatformShellDetail
#endif

inline std::string ToPlatformShellCommand(const std::string &command) {
#ifdef _WIN32
    // `_popen` on Windows runs the given command line via `cmd.exe /c`, so
    // this string has to survive TWO independent, incompatible escaping
    // passes before bash ever sees it: cmd.exe's own command-line
    // tokenizing, and then bash -lc's quoting of what cmd.exe hands to
    // wsl.exe. A first version of this function tried to handle that with
    // plain backslash-doubling (escaping embedded `"` and `\`) - which is
    // exactly the wrong fix for a command whose OWN backslashes are
    // meaningful to the command being run, not to the transport: e.g.
    // `printf '5\n2\n8\n1\n9\n'`, where `\n` must reach printf as its own
    // newline-escape, or `grep -o '[0-9]\+'`, where `\+` is a regex
    // quantifier. Doubling those backslashes for cmd.exe's sake changed
    // what printf/grep actually saw, so the command still ran (no error)
    // but produced the wrong output - e.g. printf emitting literal `\n`
    // characters instead of newlines, so `sort`/`tail`/`head` never split
    // into separate lines. That surfaced downstream as a Rho "Type
    // Mismatch" (expecting an int, getting a string full of stray
    // backslashes), not as a shell error - much harder to diagnose than an
    // outright failure.
    //
    // Base64-encoding the command and having bash decode+eval it sidesteps
    // both escaping passes entirely: the only characters cmd.exe or
    // wsl.exe ever see are [A-Za-z0-9+/=], none of which are special to
    // either one, so the command text reaches bash byte-for-byte with no
    // reinterpretation of its own backslashes/quotes along the way.
    std::string encoded = PlatformShellDetail::Base64Encode(command);
    return "wsl.exe -e bash -lc \"echo " + encoded + " | base64 -d | bash\"";
#else
    return command;
#endif
}

KAI_END
