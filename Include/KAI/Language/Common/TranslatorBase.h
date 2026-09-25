#pragma once

#include <KAI/Language/Common/TranslatorCommon.h>

KAI_BEGIN

template <class EParser>
struct TranslatorBase : TranslatorCommon {
    using Parser = EParser;
    using TokenNode = typename Parser::TokenNode;
    using TokenEnum = typename TokenNode::Enum;
    using Lexer = typename Parser::Lexer;
    using AstNode = typename Parser::AstNode;
    using AstEnum = typename AstNode::Enum;
    using AstNodePtr = typename Parser::AstNodePtr;

    TranslatorBase(const TranslatorBase &) = delete;
    TranslatorBase(Registry &reg) : TranslatorCommon(reg) {}

    Pointer<Continuation> Translate(const char* text, Structure st) override
    {
        if (text == nullptr || text[0] == 0) {
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

        if (lex->failed) {
            KAI_TRACE_WARN_1(lex->Error);
            Fail(lex->error);
            return Object();
        }

        if (trace > 0) {
            KAI_TRACE_1(lex->Print());
        }

        auto parse = std::make_shared<Parser>(*reg_);
        parse->Process(lex, st);
        if (parse->failed) {
            if (trace > 1) {
                KAI_TRACE_1(parse->PrintTree());
            }

            Fail(parse->error);
            return Object();
        }

        if (trace > 1) {
            KAI_TRACE_1(parse->PrintTree());
        }

        PushNew();

        TranslateNode(parse->GetRoot());

        if (stack_.empty()) {
            KAI_THROW_0(EmptyStack);
        }

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
            if (!failed) {
                Fail("Failed");
            }
        }
    }
};

KAI_END
