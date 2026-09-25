#pragma once

#include <KAI/Core/BuiltinTypes/Stack.h>
#include <KAI/Core/Object/Reflected.h>
#include <KAI/Core/Pathname.h>
#include <KAI/Core/Value.h>
#include <KAI/Executor/Continuation.h>
#include <KAI/Executor/Operation.h>

KAI_BEGIN

class Tree;
struct Executor;

KAI_TYPE_TRAITS(Executor, Number::Executor, Properties::Reflected);

struct Executor : Reflected {
    Executor();
    ~Executor() override;

    friend bool operator<(const Executor& a, const Executor& b);
    friend bool operator==(const Executor& a, const Executor& b);

    void Create() override;
    bool Destroy() override;

    void SetScope(Object);
    void PopScope();
    [[nodiscard]] Object GetScope() const;

    void SetContinuation(Value<Continuation>);
    [[nodiscard]] Value<Continuation> GetContinuation() const { return continuation_; }
    void Continue();
    void Continue(Value<Continuation>);
    void ContinueOnly(Value<Continuation> c);
    void ContinueOneInstruction();

    void SetSingleStep(bool enable) { singleStep_ = enable; }
    [[nodiscard]] bool GetSingleStep() const
    {
        return singleStep_;
    }

    bool Step();

    [[nodiscard]] Object GetCompiler() const
    {
        return compiler_;
    }
    void SetCompiler(const Object& c)
    {
        compiler_ = c;
    }

    void Eval(Object const& q);
    void Dump(Object const& q);

    [[nodiscard]] std::string PrintStack() const;
    void PrintStack(std::ostream &out) const;
    void Run();

    template <class T>
    Value<T> New() {
        return Reg().New<T>();
    }

    template <class T> Value<T> New(T const& x)
    {
        return Reg().New(x);
    }

    void SetTree(Tree* t)
    {
        tree_ = t;
    }
    [[nodiscard]] Tree* GetTree() const
    {
        return tree_;
    }

    void SetTraceLevel(int);
    [[nodiscard]] int GetTraceLevel() const;

    template <class T>
    void Push(const Value<T> &val) {
        Push(val.GetObject());
    }

    template <class Ident> void EvalIdent(Object const& q)
    {
        try {
            // Validate the input object
            if (!q.Valid()) {
                KAI_TRACE_ERROR() << "EvalIdent: Invalid object";
                return;  // Return early instead of throwing
            }

            // Extract the identifier from the object
            Ident const& ident = ConstDeref<Ident>(q);
            std::cerr << "[EI1] EvalIdent name=" << ident.ToString() << " quoted=" << ident.Quoted() << std::endl;

            // For quoted identifiers, just push the original object
            if (ident.Quoted()) {
                if (traceLevel_ > 3) {
                    KAI_TRACE() << "EvalIdent: Pushing quoted identifier: "
                                << ident.ToString();
                }
                Push(q);
                return;
            }

            // Handle empty labels as a special case
            if constexpr (std::is_same_v<Ident, Label> ||
                          std::is_same_v<Ident, Pathname>) {
                if (ident.ToString().Empty()) {
                    KAI_TRACE() << "EvalIdent: Empty identifier name, creating "
                                   "placeholder";
                    // Push an empty object rather than throwing an exception
                    Push(Object());
                    return;
                }
            }

            // Try to resolve the identifier
            auto found = TryResolve(ident);

            // If found, push it onto the stack
            if (found.Valid() && found.Exists()) {
                if (traceLevel_ > 3) {
                    KAI_TRACE()
                        << "EvalIdent: Resolved identifier " << ident.ToString()
                        << " to " << found.ToString();
                    if (found.GetClass()) {
                        KAI_TRACE()
                            << "  (Type: " << found.GetClass()->GetName()
                            << ")";
                    }
                }

                // For all types including continuations, push the resolved
                // object This allows operations like & to control when
                // continuations execute
                Push(found);
            } else {
                // If not found, try to create a placeholder instead of throwing
                // an exception
                if constexpr (std::is_same_v<Ident, Label>) {
                    KAI_TRACE()
                        << "EvalIdent: Object not found: " << ident.ToString()
                        << ", creating placeholder";
                    // Create a placeholder object - use TryResolveOrCreate
                    auto placeholder = TryResolveOrCreate(ident);
                    Push(placeholder);
                } else {
                    // For non-Label types, we still need to handle the error
                    KAI_TRACE_ERROR()
                        << "EvalIdent: Object not found: " << ident.ToString();
                    // Instead of throwing, push an empty object
                    Push(Object());
                }
            }
        } catch (const exception::Base& e) {
            KAI_TRACE_ERROR() << "EvalIdent: KAI exception: " << e.ToString();
            // Instead of rethrowing, push an empty object to allow execution to
            // continue
            Push(Object());
        } catch (const std::exception& e) {
            KAI_TRACE_ERROR() << "EvalIdent: std::exception: " << e.what();
            // Instead of rethrowing, push an empty object to allow execution to
            // continue
            Push(Object());
        } catch (...) {
            KAI_TRACE_ERROR() << "EvalIdent: Unknown exception";
            // Instead of rethrowing, push an empty object to allow execution to
            // continue
            Push(Object());
        }
    }

    void Push(Object const &);
    void Push(const std::pair<Object, Object> &);
    Object Pop();
    [[nodiscard]] Object Top() const;

    Value<Stack> GetDataStack();
    [[nodiscard]] Value<const Stack> GetDataStack() const
    {
        if (!data_.Valid() || !data_.Exists()) {
            KAI_TRACE_ERROR() << "GetDataStack: Invalid data stack";
            return {};
        }
        return Value<const Stack>(data_.GetConstObject());
    }

    void SetDataStack(Value<Stack> stack) { data_ = stack; }

    [[nodiscard]] Value<Stack> GetContextStack() const;

    void ClearStacks() {
        data_->Clear();
        context_->Clear();
    }

    static void Register(Registry &, const char * = "Executor");
    void ClearContext();
    void DropN();
    void ContinuePi();
    void EvalContinuation(Object const& q);
    bool IsBinaryOp(Operation::Type op);
    Object PerformBinaryOp(Object const& a, Object const& b, Operation::Type op);
    [[nodiscard]] Object Resolve(Object, bool ignoreQuote = false) const;
    [[nodiscard]] Object Resolve(const Label&) const;
    [[nodiscard]] Object Resolve(const Pathname&) const;
    Object TryResolveOrCreate(Label const &label, Type::Number type = Type::Number::None);
    Object ExtractValueFromContinuation(Object const &value);
    Object UnwrapValue(const Object &value);

public:
    void Perform(Operation::Type op);

protected:
    bool PopBool();

    void ToArray();
    void ProcessToArray(int len);

    void GetChildren();
    void Expand();
    void MarkAndSweep();
    void MarkAndSweep(Object &root);

    void Push(Stack& l, Object const& q);
    Object Pop(Stack &stack);
    void NextContinuation();

    void DumpStack(Stack const &);
    static void DumpContinuation(Continuation const &, int);

private:
    template <class C>
    Value<Array> ForEach(C const &, Object const &);
    template <class Cont>
    void PushAll(const Cont &cont);

    void TraceAll();
    void Trace(const Object &);
    void Trace(const Label &, const StorageBase &, StringStream &);
    void Trace(const Object &, StringStream &);
    void ConditionalContextSwitch(Operation::Type);
    Pointer<Continuation> NewContinuation(Value<Continuation> p);
    void ExecuteContinuationInline(Pointer<Continuation> cont);
    void ExecuteContinuationInlineAndDrain(Pointer<Continuation> cont);

    [[nodiscard]] Object TryResolve(Object const&) const;
    [[nodiscard]] Object TryResolve(Label const& label) const;
    [[nodiscard]] Object TryResolve(Pathname const& label) const;

    Value<Continuation> continuation_;
    Value<Stack> context_;
    Value<Stack> data_;
    Object compiler_;
    bool break_;      // Set by Break operation to exit loops
    bool continue_;   // Set by Continue operation to skip to next loop iteration
    bool loopBreak_ = false;  // Set ONLY by Operation::Break; survives drain-loop resets so WhileLoop can observe a genuine loop break even after an intervening function call (Suspend/Return share break_, which is expected to be absorbed mid-drain - loopBreak_ is not).
    bool returning_ = false;  // set by Return; survives drain resets
    bool replace_;    // Set by Replace operation to replace current continuation
    Tree *tree_;
    int traceLevel_;
    int stepNumber_;
    bool singleStep_;
};

StringStream &operator<<(StringStream &, Executor const &);
BinaryStream &operator<<(BinaryStream &, Executor const &);
BinaryPacket &operator>>(BinaryPacket &, Executor &);

KAI_END

