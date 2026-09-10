#!/usr/bin/env python3
"""
apply_win_patches.py - Apply all Windows port patches to CppKaiCore submodule.
Run from the CppKAI repo root.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXT  = ROOT / "Ext" / "CppKaiCore"

def patch(rel, old, new, label=""):
    path = EXT / rel
    content = path.read_text(encoding="utf-8", errors="replace")
    if old in content:
        path.write_text(content.replace(old, new, 1), encoding="utf-8")
        print(f"  OK  {label or rel}")
    else:
        print(f"  --  {label or rel} (already applied or not found)")

print("Applying KAI Windows port patches...\n")

# 1. FwdDeclarations.h - add <cstddef> for std::size_t
patch(
    "Include/KAI/Core/FwdDeclarations.h",
    "#pragma once\n\n#include <KAI/Core/Base.h>",
    "#pragma once\n\n#include <cstddef>\n#include <KAI/Core/Base.h>",
    "FwdDeclarations.h: add <cstddef>"
)

# 2. String.cpp - add <ostream>
patch(
    "Source/Library/Core/Source/BuiltinTypes/String.cpp",
    "#include <algorithm>",
    "#include <ostream>\n#include <algorithm>",
    "String.cpp: add <ostream>"
)

# 3. rang.hpp - move <windows.h> before <VersionHelpers.h>
patch(
    "Include/KAI/Console/rang.hpp",
    "#include <VersionHelpers.h>\n#include <io.h>\n#include <windows.h>",
    "#include <windows.h>\n#include <VersionHelpers.h>\n#include <io.h>",
    "rang.hpp: fix include order"
)

# 4. Platform.h - include use_windows.h
patch(
    "Include/KAI/Core/Config/Platform.h",
    "// EOF",
    "#include <KAI/Platform/Platform.h>\n\n// EOF",
    "Platform.h: include KAI/Platform/Platform.h"
)

# 5. KaiProcess.h - rename (copy content, original deleted by git mv)
kai_process = EXT / "Include/KAI/Language/Common/KaiProcess.h"
old_process  = EXT / "Include/KAI/Language/Common/Process.h"
if not kai_process.exists() and old_process.exists():
    kai_process.write_bytes(old_process.read_bytes())
    old_process.unlink()
    print("  OK  KaiProcess.h: renamed from Process.h")
elif kai_process.exists():
    print("  --  KaiProcess.h: already exists")
else:
    print("  !!  KaiProcess.h: Process.h not found either - manual fix needed")

# 6. Update all includes of process.h -> KaiProcess.h
import re
files_to_update = [
    "Include/KAI/Language/Common/LangCommon.h",
    "Include/KAI/Language/Common/LexerCommon.h",
    "Include/KAI/Language/Common/ParserBase.h",
    "Include/KAI/Language/Common/ParserCommon.h",
    "Include/KAI/Language/Common/ProcessCommon.h",
    "Source/Library/Executor/Source/Translator/Process.cpp",
    "Source/Library/Executor/Source/Executor.cpp",
]
for rel in files_to_update:
    p = EXT / rel
    if p.exists():
        t = p.read_text(encoding="utf-8", errors="replace")
        if "Common/Process.h" in t or '"process.h"' in t:
            t = t.replace("Common/Process.h", "Common/KaiProcess.h")
            t = t.replace('"process.h"', '"KaiProcess.h"')
            p.write_text(t, encoding="utf-8")
            print(f"  OK  {rel}: updated process.h -> KaiProcess.h")
        else:
            print(f"  --  {rel}: already updated")
    else:
        print(f"  !!  {rel}: not found")

# 7. use_windows.h
use_windows = EXT / "Include/KAI/Core/Config/use_windows.h"
use_windows.write_text("""\
#pragma once
// Included from Config/Platform.h - redirects to KAI/Platform/Platform.h
// Kept for backward compatibility
#include <KAI/Platform/Platform.h>
""", encoding="utf-8")
print("  OK  use_windows.h: created/updated")

# 8. msvc_compat.h (force-include for CppKaiCore targets)
msvc_compat = EXT / "Include/KAI/Core/msvc_compat.h"
msvc_compat.write_text("""\
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
""", encoding="utf-8")
print("  OK  msvc_compat.h: created/updated")

# 9. CppKaiCore CMakeLists.txt - add MSVC force-include
patch(
    "CMakeLists.txt",
    "# --- Core -------------------------------------------------------------------",
    """\
# MSVC 2026 compatibility - force-include missing transitive headers
if(MSVC)
    set(KAI_COMPAT "${CPPKAICORE_INCLUDE}/KAI/Core/msvc_compat.h")
    add_compile_options(/FI${KAI_COMPAT})
endif()

# --- Core -------------------------------------------------------------------""",
    "CppKaiCore CMakeLists.txt: add MSVC force-include"
)

# 10. Console.cpp - wrap popen/pclose and add cyan lambda prompt
console = EXT / "Source/Library/Executor/Source/Console.cpp"
content = console.read_text(encoding="utf-8", errors="replace")

# Add #ifdef _WIN32 process.h guard
if "#ifdef _WIN32" not in content[:500]:
    content = content.replace(
        '#include "KAI/Console/Console.h"',
        '#include "KAI/Console/Console.h"\n#ifdef _WIN32\n#  include <process.h>\n#endif'
    )
    print("  OK  Console.cpp: added <process.h> guard")

# Add cyan lambda prompt
old_prompt = "out << rang::style::bold << symbol << rang::fg::reset;"
new_prompt = (
    'if (std::string_view(symbol) == "\\xce\\xbb ")\n'
    '        out << rang::fg::cyan << rang::style::bold << symbol << rang::fg::reset;\n'
    '    else\n'
    '        out << rang::style::bold << symbol << rang::fg::reset;'
)
if old_prompt in content:
    content = content.replace(old_prompt, new_prompt)
    print("  OK  Console.cpp: cyan lambda prompt")

# Wrap popen blocks with #ifndef _WIN32
popen_count = content.count("FILE *pipe = popen(")
already_guarded = content.count("#ifndef _WIN32") 
if popen_count > 0 and already_guarded < popen_count:
    # Do line-based wrapping
    lines = content.splitlines(keepends=True)
    output = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if "FILE *pipe = popen(" in line and "#" not in line:
            output.append("#ifndef _WIN32\n")
            depth = 0
            found_open = False
            while i < len(lines):
                output.append(lines[i])
                depth += lines[i].count("{") - lines[i].count("}")
                if "{" in lines[i]:
                    found_open = True
                if found_open and depth <= 0:
                    i += 1
                    break
                i += 1
            output.append("#endif // _WIN32\n")
            continue
        output.append(line)
        i += 1
    content = "".join(output)
    print(f"  OK  Console.cpp: wrapped {popen_count} popen block(s)")

console.write_text(content, encoding="utf-8")

print("\nDone. Now run:\n  cd build && cmake .. && cmake --build . --config Release --target Console --parallel")
