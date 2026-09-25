#include <KAI/Core/BuiltinTypes.h>

KAI_BEGIN

namespace exception {
std::string Base::ToString() const {
    StringStream S;
    std::string loc = location.ToString().StdString();
    loc = loc.substr(loc.find_last_of('/') + 1);
    S << loc << text << ": ";
    WriteExtendedInformation(S);
    return S.ToString().StdString();
}

void TypeMismatch::WriteExtendedInformation(StringStream &S) const {
    S << "expected=" << Type::Number(first).ToString()
      << ", got=" << Type::Number(second).ToString();
}

void UnknownTypeNumber::WriteExtendedInformation(StringStream &S) const {
    S << "type=" << typeNumber;
}

void UnknownObject::WriteExtendedInformation(StringStream &S) const {
    S << "handle=" << (int)handle.GetValue();
}

void ObjectNotFound::WriteExtendedInformation(StringStream &S) const {
    S << "label=" << label;
}

void NoOperation::WriteExtendedInformation(StringStream &S) const {
    S << "type=" << typeNumber << ", typeProperty=" << typeProperty;
}

void PacketExtraction::WriteExtendedInformation(StringStream &S) const {
    S << "type=" << typeNumber;
}

void OutOfBounds::WriteExtendedInformation(StringStream &S) const {
    S << "type=" << typeNumber << ", index=" << index;
}

void PacketInsertion::WriteExtendedInformation(StringStream &S) const {
    S << "type=" << typeNumber;
}

void CannotResolve::WriteExtendedInformation(StringStream &S) const {
    if (!path.Empty())
        S << "path=" << path;
    else if (!label.Empty())
        S << "label=" << label;
    else
        S << "object=" << object;
}

void InvalidPathname::WriteExtendedInformation(StringStream &S) const {
    S << "text=" << text;
}

void ObjectNotInTree::WriteExtendedInformation(StringStream &S) const {
    S << "object=" << object;
}

void UnknownMethod::WriteExtendedInformation(StringStream &S) const {
    S << "name=" << name << ", class=" << className;
}

void UnknownHandle::WriteExtendedInformation(StringStream &S) const {
    S << "handle=" << handle.GetValue();
}

void InvalidStringLiteral::WriteExtendedInformation(StringStream &S) const {
    S << "text='" << text << "'";
}

void CannotNew::WriteExtendedInformation(StringStream &S) const {
    S << "object=" << arg;
}

void UnknownKey::WriteExtendedInformation(StringStream &S) const {
    S << "key=" << key;
}

void NotImplemented::WriteExtendedInformation(StringStream &S) const {
    S << "what=" << text;
}

void BadIndex::WriteExtendedInformation(StringStream &S) const {
    S << "index=" << index;
}

void InvalidIdentifier::WriteExtendedInformation(StringStream &S) const {
    S << "what='" << what << "'";
}

void FileNotFound::WriteExtendedInformation(StringStream &S) const {
    S << "filename='" << filename << "'";
}
void UnknownProperty::WriteExtendedInformation(StringStream &S) const {
    S << "class=" << klass << ", property=" << prop;
}
}  // namespace exception

StringStream &operator<<(StringStream &S, exception::Base const &E) {
    return S << E.ToString();
}

KAI_END

// EOF
