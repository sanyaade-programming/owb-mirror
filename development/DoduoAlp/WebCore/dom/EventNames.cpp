/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 2005 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define DOM_EVENT_NAMES_HIDE_GLOBALS 1
#endif

#include "EventNames.h"
#include "StaticConstructors.h"

namespace WebCore { namespace EventNames {

#define DEFINE_EVENT_GLOBAL(name) \
    DEFINE_GLOBAL(AtomicString, name##Event, #name)
DOM_EVENT_NAMES_FOR_EACH(DEFINE_EVENT_GLOBAL)

void init()
{
    static bool initialized;
    if (!initialized) {
        // Use placement new to initialize the globals.
        
        AtomicString::init();
        #define INITIALIZE_GLOBAL(name) new ((void*)&name##Event) AtomicString(#name);
        DOM_EVENT_NAMES_FOR_EACH(INITIALIZE_GLOBAL)
        initialized = true;
    }
}

} }
