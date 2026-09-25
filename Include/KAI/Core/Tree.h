#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Object.h>
#include <KAI/Core/Pathname.h>

KAI_BEGIN

class Tree {
   public:
       using SearchPathA = std::list<Object>;

   private:
    SearchPath path_;
    Object root_, scope_;
    Pathname current_;

   public:
       void SetRoot(const Object& q)
       {
           root_ = q;
       }
    void AddSearchPath(const Pathname &);
    void AddSearchPath(const Object &);

    [[nodiscard]] Object Resolve(const Pathname&) const;
    [[nodiscard]] Object Resolve(const Label&) const;

    [[nodiscard]] Object GetRoot() const
    {
        return root_;
    }
    [[nodiscard]] Object GetScope() const
    {
        return scope_;
    }
    [[nodiscard]] const SearchPath& GetSearchPath() const
    {
        return path_;
    }

    void SetScope(const Object &);
    void SetScope(const Pathname &);

    // void SetSearchPath(const SearchPath &);
    // void GetChildren() const;
};

KAI_END
