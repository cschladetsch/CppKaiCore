#pragma once

#include <KAI/Core/Debug.h>
#include <KAI/Executor/Continuation.h>
#include <KAI/Executor/Operation.h>
#include <KAI/Language/Common/Language.h>

#include <functional>
#include <map>
#include <utility>

KAI_BEGIN

class Compiler : public Reflected {
    using OperationToString = std::map<Operation, String>;
    using StringToOperation = std::map<String, Pointer<Operation>>;
    using TranslateFunction = std::function<Pointer<Continuation>(const String&, Structure)>;

private:
    OperationToString opToString_;
    StringToOperation stringToOp_;
    Language language_ = Language::Pi;
    int traceLevel_ = 0;
    TranslateFunction translateFunction_;

   public:
       bool Destroy() override;

       void SetLanguage(int);
       [[nodiscard]] int GetLanguage() const;
       void SetTraceLevel(int n)
       {
           traceLevel_ = n;
       }
       [[nodiscard]] int GetTraceLevel() const
       {
           return traceLevel_;
       }
    void SetTranslateFunction(TranslateFunction func) {
        translateFunction_ = std::move(func);
    }

    [[nodiscard]] Pointer<Continuation> Translate(const String& text, Structure st = Structure::Expression) const;
    [[nodiscard]] Pointer<Continuation> CompileFile(const String& fileName, Structure st = Structure::Program) const;

    static void Register(Registry &, const char * = "Compiler");

    void AddOperation(int n, const String& s);
};

KAI_TYPE_TRAITS(Compiler, Number::Compiler, Properties::Reflected);

KAI_END
