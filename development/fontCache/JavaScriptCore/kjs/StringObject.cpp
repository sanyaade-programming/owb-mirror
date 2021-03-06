/*
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"
#include "StringObject.h"

#include "PropertyNameArray.h"

namespace KJS {

const ClassInfo StringObject::info = { "String", 0, 0, 0 };

StringObject::StringObject(ExecState* exec, JSObject* prototype)
    : JSWrapperObject(prototype)
{
    setInternalValue(jsString(exec, ""));
}

StringObject::StringObject(JSObject* prototype, JSString* string)
    : JSWrapperObject(prototype)
{
    setInternalValue(string);
}

StringObject::StringObject(ExecState* exec, JSObject* prototype, const UString& string)
    : JSWrapperObject(prototype)
{
    setInternalValue(jsString(exec, string));
}

bool StringObject::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    if (internalValue()->getStringPropertySlot(exec, propertyName, slot))
        return true;
    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}
    
bool StringObject::getOwnPropertySlot(ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    if (internalValue()->getStringPropertySlot(propertyName, slot))
        return true;    
    return JSObject::getOwnPropertySlot(exec, Identifier::from(exec, propertyName), slot);
}

void StringObject::put(ExecState* exec, const Identifier& propertyName, JSValue* value)
{
    if (propertyName == exec->propertyNames().length)
        return;
    JSObject::put(exec, propertyName, value);
}

bool StringObject::deleteProperty(ExecState* exec, const Identifier& propertyName)
{
    if (propertyName == exec->propertyNames().length)
        return false;
    return JSObject::deleteProperty(exec, propertyName);
}

void StringObject::getPropertyNames(ExecState* exec, PropertyNameArray& propertyNames)
{
    int size = internalValue()->value().size();
    for (int i = 0; i < size; ++i)
        propertyNames.add(Identifier(exec, UString::from(i)));
    return JSObject::getPropertyNames(exec, propertyNames);
}

UString StringObject::toString(ExecState*) const
{
    return internalValue()->value();
}

UString StringObject::toThisString(ExecState*) const
{
    return internalValue()->value();
}

JSString* StringObject::toThisJSString(ExecState*)
{
    return internalValue();
}

} // namespace KJS
