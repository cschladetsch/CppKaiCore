#pragma once

#include <KAI/Core/Base.h>
#include <KAI/Core/Exception/ExceptionMacros.h>
#include <KAI/Core/Meta/Base.h>

#include <boost/type_index.hpp>

#include "KAI/Core/Object/Reflected.h"
#include "KAI/Core/Type/Properties.h"

KAI_TYPE_BEGIN

template <typename T>
struct NoTraitsDefined;

template <typename T>
struct Traits {
    using Type = typename NoTraitsDefined<T>::ForType;
};

using TypeNumber = int;

template <class T>
struct StorageType {
    using Type = T;
};

template <class T, int E, int Q, class St = StorageType<T>::Type, class Ref = T&, class ConstRef = T const&>
struct TraitsBase {
    enum { Number = E };
    enum { Props = Q };

    using Type = T;
    using Store = St;
    using Pointer = Store*;
    using ConstPointer = Store const*;
    using Reference = Ref;
    using ConstReference = ConstRef;

    static std::string Name() {
        return boost::typeindex::type_id<T>().pretty_name();
    }

    // used as a dummy template parameter to avoid nested explicit template
    // instantiations.
    class D {};

    template <int N2>
    struct HasProperty {
        enum { Value = (N2 & Q) != 0 };
    };

    template <int N3>
    struct HasProperties {
        enum { Value = (N3 & Q) == N3 };
    };

    template <class, bool>
    struct UpCast {
        // static Storage<Parent *> *Cast(Registry &R, Reference Q)
        //{
        //     return 0;
        // }
    };

    template <class X>
    struct UpCast<X, true> {
        /*
        static Storage<meta::Null> *Cast(Registry &, Reference)
        {
            Storage<Parent *> *parent = NewStorage<Parent *>(R);
            **parent = &Q;
            return parent;
        }
        */
    };

    // Assignment
    template <class, bool>
    struct AssignOp {
        static void Perform(Reference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Assign);
        }
    };
    template <class Dummy>
    struct AssignOp<Dummy, true> {
        static void Perform(Reference a, ConstReference b)
        {
            a = b;
        }
    };

    // absolute
    template <class Dummy, bool>
    struct AbsoluteOp {
        static void Perform(Reference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Absolute);
        }
    };

    template <class Dummy>
    struct AbsoluteOp<Dummy, true> {
        static void Perform(Reference value) {
            // TODO SetAbsoluteValue(value);
        }
    };

    template <class Dummy, bool>
    struct LessOp {
        static bool Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Less);
        }
    };

    template <class Dummy>
    struct LessOp<Dummy, true> {
        static bool Perform(ConstReference a, ConstReference b)
        {
            return a < b;
        }
    };

    template <class Dummy, bool>
    struct EquivOp {
        static bool Perform(ConstReference a, ConstReference b)
        {
            return a == b;
        }
    };

    template <class Dummy>
    struct EquivOp<Dummy, false> {
        static bool Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Equiv);
        }
    };
    /// Greater
    template <class Dummy, bool>
    struct GreaterOp {
        static bool Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Greater);
        }
    };

    template <class Dummy>
    struct GreaterOp<Dummy, true> {
        static bool Perform(ConstReference a, ConstReference b)
        {
            return a > b;
        }
    };

    // TODO: implement arithmetic using += etc
    template <class Dummy, bool>
    struct PlusOp {
        static Store Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Plus);
        }
    };

    template <class Dummy>
    struct PlusOp<Dummy, true> {
        static Store Perform(ConstReference a, ConstReference b)
        {
            return a + b;
        }
    };

    template <class Dummy, bool>
    struct BoolOp {
        static bool Perform(ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Boolean);
        }
    };

    template <class Dummy>
    struct BoolOp<Dummy, true> {
        static bool Perform(ConstReference a)
        {
            return static_cast<bool>(a);
        }
    };

    template <class, bool>
    struct MinusOp {
        static Store Perform(ConstReference a, ConstReference b)
        {
            return a - b;
        }
    };

    template <class D>
    struct MinusOp<D, false> {
        static Store Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Minus);
        }
    };

    template <class D, bool>
    struct MultiplyOp {
        static Store Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Multiply);
        }
    };

    template <class D>
    struct MultiplyOp<D, true> {
        static Store Perform(ConstReference a, ConstReference b)
        {
            return a * b;
        }
    };

    template <class, bool>
    struct DivideOp {
        static Store Perform(ConstReference /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::Divide);
        }
    };

    template <class D>
    struct DivideOp<D, true> {
        static Store Perform(ConstReference a, ConstReference b)
        {
            return a / b;
        }
    };

    template <class, bool>
    struct HashOp {
        static HashValue Calc(ConstReference a)
        {
#pragma warning(push)
            // warning: unreachable code. NFI why...
#pragma warning(disable : 4702)
            // TODO GetHash(A);
            return 42;
#pragma warning(push)
        }
    };

    template <class D>
    struct HashOp<D, false> {
        static HashValue Calc(ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::CalcHashValue);
        }
    };

    struct UnReflectedLifetimeManagement {
        static void Create(Storage<T>* /*unused*/) {}
        static bool Destroy(Storage<T>* /*unused*/)
        {
            return true;
        }
        static void Delete(Storage<T> *ptr) {
            if (ptr) {
                ptr->GetRegistry()->GetMemorySystem().DeAllocate(ptr);
            }
        }
    };

    struct ReflectedLifetimeManagement {
        static void Create(Storage<T> *storage) {
            UnReflectedLifetimeManagement::Create(storage);
            auto* reflected = (Reflected*) &storage->GetCleanReference();
            reflected->self = (StorageBase*) storage;
            reflected->Create();
        }

        static bool Destroy(Storage<T> *storage) {
            Reflected *reflected = &storage->GetReference();
            bool destroyed = reflected->Destroy();
            if (destroyed) {
                UnReflectedLifetimeManagement::Destroy(storage);
            }

            return destroyed;
        }

        static void Delete(Storage<T> *storage) {
            Reflected *reflected = &storage->GetReference();
            reflected->Delete();
            UnReflectedLifetimeManagement::Delete(storage);
        }
    };

    template <class, bool>
    struct Contained {
        template <class A, class B, class C> static void SetSwitch(A /*unused*/, B /*unused*/, C /*unused*/) {}

        template <class A, class B> static void SetMarked(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void Erase(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void ForEachContained(A /*unused*/, B /*unused*/)
        {
            // KAI_NOT_IMPLEMENTED();
        }
    };

    template <class D>
    struct Contained<D, true> {
        template <class A, class B, class C> static void SetSwitch(A /*unused*/, B /*unused*/, C /*unused*/) {}

        template <class A, class B> static void SetMarked(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void Erase(A /*unused*/, B /*unused*/) {}
        template <class A, class B> static void ForEachContained(A /*unused*/, B /*unused*/)
        {
            // KAI_NOT_IMPLEMENTED();
        }
    };

    using ContainerOps = Contained<D, HasProperty<::kai::Type::Properties::Container>::Value>;
    using LifetimeManager = typename meta::If<HasProperty<::kai::Type::Properties::Reflected>::Value != 0,
                                              ReflectedLifetimeManagement, UnReflectedLifetimeManagement>::Type;
    using Assign = AssignOp<D, HasProperty<::kai::Type::Properties::Assign>::Value != 0>;
    using Absolute = AbsoluteOp<D, HasProperty<::kai::Type::Properties::Absolute>::Value != 0>;
    using Less = LessOp<D, HasProperty<::kai::Type::Properties::Less>::Value != 0>;
    using Equiv = EquivOp<D, HasProperty<::kai::Type::Properties::Equiv>::Value != 0>;
    using Greater = GreaterOp<D, HasProperty<::kai::Type::Properties::Greater>::Value != 0>;
    using Plus = PlusOp<D, HasProperty<::kai::Type::Properties::Plus>::Value != 0>;
    using Minus = MinusOp<D, HasProperty<::kai::Type::Properties::Minus>::Value != 0>;
    using Divide = DivideOp<D, HasProperty<::kai::Type::Properties::Divide>::Value != 0>;
    using Multiply = MultiplyOp<D, HasProperty<::kai::Type::Properties::Multiply>::Value != 0>;
    using Boolean = BoolOp<D, HasProperty<::kai::Type::Properties::Boolean>::Value != 0>;
    template <bool, class Stream>
    struct StreamInsertOp {
        static void Insert(Stream& s, ConstReference x)
        {
            s << x;
        }
    };

    template <class Stream>
    struct StreamInsertOp<false, Stream> {
        static void Insert(Stream& /*unused*/, ConstReference /*unused*/)
        {
            // nKAI_THROW_1("Can't insert into stream");
        }
    };

    template <bool, class Stream>
    struct StreamExtractOp {
        static void Extract(Stream& s, Reference x)
        {
            s >> x;
        }
    };
    template <class Stream>
    struct StreamExtractOp<false, Stream> {
        static void Extract(Stream& /*unused*/, ConstReference /*unused*/)
        {
            KAI_THROW_2(NoOperation, Number, ::kai::Type::Properties::StreamExtract);
        }
    };

    using StringStreamInsert = StreamInsertOp<HasProperty<::kai::Type::Properties::StringStreamInsert>::Value != 0,
                                              KAI_NAMESPACE(StringStream)>;
    using StringStreamExtract = StreamExtractOp<HasProperty<::kai::Type::Properties::StringStreamExtract>::Value != 0,
                                                KAI_NAMESPACE(StringStream)>;
    using BinaryStreamInsert = StreamInsertOp<HasProperty<::kai::Type::Properties::BinaryStreamInsert>::Value != 0,
                                              KAI_NAMESPACE(BinaryStream)>;
    using BinaryPacketExtract = StreamExtractOp<HasProperty<::kai::Type::Properties::BinaryStreamExtract>::Value != 0,
                                                KAI_NAMESPACE(BinaryStream)>;

    // typedef typename
    // StreamInsert<HasProperty<::kai::Type::Properties::XmlStreamInsert>::Value, XmlStream>
    // XmlStreamInsert;

    using HashFunction = HashOp<D, HasProperty<::kai::Type::Properties::NoHashValue>::Value != 0>;
};

KAI_TYPE_END
