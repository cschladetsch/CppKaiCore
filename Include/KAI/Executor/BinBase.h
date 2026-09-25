#pragma once

#include <KAI/Core/BuiltinTypes/Array.h>
#include <KAI/Core/BuiltinTypes/String.h>
#include <KAI/Core/BuiltinTypes/Vector4.h>
#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Object.h>

KAI_BEGIN

/// Defines what goes into the /bin folder at runtime setup
namespace Bin
{
void Help();
Object GetMethods(Object q);
Object GetProperties(Object q);
Vector3 ScaleVector3(Vector3 vec, float scalar);
Vector3 AddVector3(Vector3 vec, Vector3 addition);
void WriteToFile(String filename, Object q);
String ReadFile(String filename);
void Printf(String fmt, Array items);
void Print(Object q);
void Print(Object q);
void PrintXml(Object q);
Object UpCast(Object q);
void SetClean(Object q, bool d);
bool IsDirty(Object q);
bool IsClean(Object q);
bool IsConst(Object q);
void Assert(bool b);
Object Freeze(Object q);
Object Thaw(Object q);
String ToString(Object q);
String ToXmlString(Object q);
String Version();
void Quit();
void ExitToOS(int n);
Object RunOne(Object object);
Object RunAllTests(Object object);
Object TypeNumberToClass(Object tn);
Object Describe(Object q);
String Info(Object object);
void AddFunctions(Object q);
} // namespace Bin

KAI_END
