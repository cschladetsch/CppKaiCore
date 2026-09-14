#pragma once

#include <KAI/Language/Common/TranslatorCommon.h>

KAI_BEGIN

template <class EParser>
struct TranslatorBase : TranslatorCommon {
    typedef EParser Parser;
    typedef typename Parser::TokenNode TokenNode;
    typedef typename TokenNode::Enum TokenEnum;
    typedef typename Parser::Lexer Lexer;
    typedef typename Parser::AstNode AstNode;
    typedef typename AstNode::Enum AstEnum;
    typedef typename Parser::AstNodePtr AstNodePtr;

    TranslatorBase(const TranslatorBase &) = delete;
    TranslatorBase(Registry &reg) : TranslatorCommon(reg) {}

    virtual Pointer<Continuation> Translate(const char *text,
                                            Structure st) override {
        // Failed/Error (inherited from Process) previously weren't reset
        // here, so once a lex/parse failure set them, they stayed set
        // forever - every later Translate() call on this same
        // Pi/RhoTranslator instance would still read as Failed even after a
        // subsequent call actually succeeded, since nothing ever cleared
        // them back. Reset at the top of every call so Failed/Error always
        // describe only the most recent Translate().
        Failed = false;
        Error.clear();

        if (text == 0 || text[0] == 0) {
            KAI_TRACE_WARN_1("No input");
            return Object();
        }

        trace = 0;

        auto lex = std::make_shared<Lexer>(text, *reg_);
        lex->Process();
        if (lex->GetTokens().empty()) {
            KAI_TRACE_WARN_1("No tokens");
            return Object();
        }

        if (lex->Failed) {
            KAI_TRACE_WARN_1(lex->Error);
            Fail(lex->Error);
            return Object();
        }

        if (trace > 0) KAI_TRACE_1(lex->Print());

        auto parse = std::make_shared<Parser>(*reg_);
        parse->Process(lex, st);
        if (parse->Failed) {
            if (trace > 1) KAI_TRACE_1(parse->PrintTree());

            Fail(parse->Error);
            return Object();
        }

        if (trace > 1) KAI_TRACE_1(parse->PrintTree());

        PushNew();

        TranslateNode(parse->GetRoot());

        if (stack.empty()) KAI_THROW_0(EmptyStack);

        auto cont = Pop();

        // Always return the continuation - do not optimize single values
        // The Console expects a continuation and will handle execution
        return cont;
    }

    // Helper method for loop-related continuation creation
    [[nodiscard]] Pointer<Continuation> CreateContinuationAndTranslate(
        AstNodePtr node) {
        // Create a new continuation for the code block
        PushNew();

        // Translate the node into the continuation
        TranslateNode(node);

        // Get the resulting continuation
        return Pop();
    }

   protected:
    virtual void TranslateNode(AstNodePtr node) = 0;

    void Run(std::shared_ptr<Parser> p) {
        PushNew();

        try {
            TranslateNode(p);
        } catch (Exception &) {
            if (!Failed) Fail("Failed");
        }
    }
};

KAI_END
