#pragma once

#include <KAI/Core/Registry.h>
#include <KAI/Executor/Executor.h>
#include <KAI/Language/Common/Base.h>
#include <KAI/Language/Common/ParserCommon.h>
#include <KAI/Language/Common/Process.h>
#include <KAI/Language/Common/Structure.h>

KAI_BEGIN

// Common for all all languages, given the Translator Tr
// which transforms raw text into a Continuation for an Executor
template <class Tr>
class Lang : public Process {
   public:
       using Translator = Tr;
       using Lexer = typename Tr::Lexer;
       using Parser = typename Tr::Parser;

   protected:
       Registry& reg_;
       std::shared_ptr<Lexer> lex_;
       std::shared_ptr<Parser> parse_;
       std::shared_ptr<Translator> trans_;
       Pointer<Executor> exec_;

   public:
    Lang(const Lang &) = delete;
    Lang(Registry& r) : reg_(r) {}

    virtual Pointer<Executor> Exec(const char *text,
                                   Structure st = Structure::Expression) = 0;
    virtual Pointer<Continuation> Translate(
        const char *text, Structure st = Structure::Expression) = 0;
    virtual Pointer<Continuation> TranslateFile(
        const char *name, Structure st = Structure::Program) = 0;
};

KAI_END
