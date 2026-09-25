#pragma once

#include <KAI/Core/SetGCFlagFwd.h>

#include <utility>

#include "KAI/Core/Object/Accessor.h"
#include "KAI/Core/Object/Class.h"
#include "KAI/Core/Object/Method.h"
#include "KAI/Core/Object/PropertyBase.h"
#include "KAI/Core/Pathname.h"

#undef RegisterClass

KAI_BEGIN

void RegisterClass(Registry &, ClassBase const &, Object const &,
                   Pathname const &);

template <class T>
class ClassBuilder {
   public:
       using Traits = Type::Traits<T>;
       Registry* registry;
       Class<T>* klass;
       Pathname path;
       Object root;

       struct MethodsCollector {
           struct PropertiesCollector {
               ClassBuilder<T>* builder;

               PropertiesCollector& operator=(PropertiesCollector&);

               template <class Property>
               PropertiesCollector& operator()(const char* n, Property p, String const& d = "",
                                               member_create_params::Enum createParams = member_create_params::Default)
               {
                   auto label = Label(n);
                   auto q = MakeProperty<T>(p, label, createParams);
                   if (!d.Empty()) {
                       q->description = d;
                   }
                   builder->klass->AddProperty(label, q.release());
                   return *this;
               }
           };

           ClassBuilder<T>* builder;
           PropertiesCollector properties;

           MethodsCollector& operator=(MethodsCollector&);

           template <class Method> MethodsCollector& operator()(const char* name, Method method, String const& d = "")
           {
               auto label = Label(name);
               auto m = MakeMethod(method, label);
               if (!d.Empty()) {
                   m->description = d;
               }
               builder->klass->AddMethod(label, m.release());
               return *this;
           }
    };

    MethodsCollector methods;

    ClassBuilder(Registry& r, const char* n) : registry(&r), klass(new Class<T>(Label(n)))
    {
        methods.builder = this;
        methods.properties.builder = this;
    }

    ClassBuilder(Registry& r, Label const& n) : registry(&r), klass(new Class<T>(n))
    {
        methods.builder = this;
        methods.properties.builder = this;
    }

    ClassBuilder(Registry& r, const Label& n, Object const& q, Pathname p)
        : registry(&r), root(q), path(std::move(p)), klass(new Class<T>(n))
    {
        methods.builder = this;
        methods.properties.builder = this;
    }

    ~ClassBuilder() { RegisterClass(*registry, *klass, root, path); }
};

KAI_END
