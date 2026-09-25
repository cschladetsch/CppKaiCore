#pragma once

#include <KAI/Core/Config/Base.h>

#include <memory>
#include <sstream>
#include <string>

KAI_BEGIN

class ConsoleColor {
    struct Impl;
    std::unique_ptr<Impl> impl_;

   public:
    enum EType {
        Normal,
        Error,
        Warning,
        Trace,
        StackNumber,
        Prompt,
        LanguageName,
        Pathname,
        Input,

        Last,
    };

    enum EConsoleColor {
        Red,
        Green,
        Blue,
    };

    [[nodiscard]] std::string GetConsoleColor(EType type) const;
};

std::ostream& operator<<(std::ostream& s, ConsoleColor::EType c);

KAI_END
