#pragma once

#include <KAI/Core/ObjectColor.h>

#include <list>

#include "KAI/Core/BuiltinTypes/Dictionary.h"
#include "KAI/Core/Object/Object.h"

KAI_BEGIN

// Base for all object instances_. The value stored with an object is placed
// contiguously in memory with the object.
struct StorageBase : public Object {
   public:
       using Switches = int;
       using Containers = std::list<Handle>;

   private:
       Containers containers_;
       Dictionary dictionary_;
       Handle parent_;
       Switches switches{DefaultSwitches};
       Label label_;
       ObjectColor::Color color_;

   public:
       StorageBase(const ObjectConstructParams& p) : Object(p) {}

       virtual ~StorageBase() = default;

       void Delete();

       bool SetColor(ObjectColor::Color c);
       [[nodiscard]] ObjectColor::Color GetColor() const
       {
           return color_;
       }
    void SetWhite() { SetColor(ObjectColor::White); }
    void SetGrey() { SetColor(ObjectColor::Grey); }
    void SetBlack() { SetColor(ObjectColor::Black); }
    [[nodiscard]] bool IsWhite() const
    {
        return color_ == ObjectColor::White;
    }
    [[nodiscard]] bool IsGrey() const
    {
        return color_ == ObjectColor::Grey;
    }
    [[nodiscard]] bool IsBlack() const
    {
        return color_ == ObjectColor::Black;
    }

    [[nodiscard]] const Label& GetLabel() const
    {
        return label_;
    }
    void SetLabel(const Label& l)
    {
        label_ = l;
    }

    [[nodiscard]] const Dictionary& GetDictionary() const
    {
        return dictionary_;
    }
    Dictionary &GetDictionary() {
        return dictionary_;
    }
    [[nodiscard]] Object Get(const Label&) const;
    void Set(const Label &, Object const &);
    void Remove(const Label &);
    void Detach(const Label& l)
    {
        Remove(l);
    }
    void Detach(Object const &);
    [[nodiscard]] bool Has(const Label&) const;

    void SetParentHandle(Handle h);
    [[nodiscard]] Handle GetParentHandle() const
    {
        return parent_;
    }

    void SetSwitch(int, bool);
    void SetSwitches(int s)
    {
        switches = s;
    }
    void SetMarked(bool b)
    {
        SetSwitch(Marked, b);
    }
    void SetManaged(bool b);
    void SetConstant(bool b)
    {
        SetSwitch(Const, b);
    }
    void SetClean(bool b = true);
    void SetDirty(bool b = true)
    {
        SetClean(!b);
    }
    [[nodiscard]] int GetSwitches() const
    {
        return switches;
    }

    [[nodiscard]] bool IsSwitchOn(Switch s) const
    {
        return (switches & s) != 0;
    }
    [[nodiscard]] bool IsMarked() const
    {
        return IsSwitchOn(Marked);
    }
    [[nodiscard]] bool IsManaged() const
    {
        return IsSwitchOn(Managed);
    }
    [[nodiscard]] bool IsConst() const
    {
        return IsSwitchOn(Const);
    }
    [[nodiscard]] bool IsClean() const
    {
        return IsSwitchOn(Clean);
    }

    Object &operator[](Label const &);
    Object const &operator[](Label const &) const;

    // private:
    void MakeReachableGrey();
    bool CanBlacken();
    void SetColorRecursive(ObjectColor::Color color);

    [[nodiscard]] Containers const& GetContainers() const
    {
        return containers_;
    }
    void RemovedFromContainer(Object const &container);
    void DetermineNewColor();
    void AddedToContainer(Object const &container);
    void DetachFromContainers();
    // protected:

    StorageBase *GetParentPtr();

    void SetColorRecursive(ObjectColor::Color color, HandleSet &handles);
};

KAI_END

// EOF
