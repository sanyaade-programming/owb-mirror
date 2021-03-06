/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef HTML5Token_h
#define HTML5Token_h

#include "NamedMappedAttrMap.h"
#include <wtf/Noncopyable.h>
#include <wtf/Vector.h>

namespace WebCore {

class HTML5Token : public Noncopyable {
public:
    enum Type {
        Uninitialized,
        DOCTYPE,
        StartTag,
        EndTag,
        Comment,
        Character,
        EndOfFile,
    };

    HTML5Token() { clear(); }

    void clear() { m_type = Uninitialized; }

    void beginStartTag(UChar character)
    {
        ASSERT(m_type == Uninitialized);
        m_type = StartTag;
        m_data.clear();
        m_selfClosing = false;
        m_attrs = 0;

        m_data.append(character);
    }

    void beginEndTag(UChar character)
    {
        ASSERT(m_type == Uninitialized);
        m_type = EndTag;
        m_data.clear();
        m_selfClosing = false;
        m_attrs = 0;

        m_data.append(character);
    }

    void beginCharacter(UChar character)
    {
        ASSERT(m_type == Uninitialized);
        m_type = Character;
        m_data.clear();
        m_data.append(character);
    }

    void appendToName(UChar character)
    {
        ASSERT(m_type == StartTag || m_type == EndTag || m_type == DOCTYPE);
        m_data.append(character);
    }

    void appendToCharacter(UChar character)
    {
        ASSERT(m_type == Character);
        m_data.append(character);
    }

    Type type() const { return m_type; }

    AtomicString name()
    {
        ASSERT(m_type == StartTag || m_type == EndTag || m_type == DOCTYPE);
        return AtomicString(StringImpl::adopt(m_data));
    }

    bool selfClosing() const
    {
        ASSERT(m_type == StartTag || m_type == EndTag);
        return m_selfClosing;
    }

    NamedMappedAttrMap* attrs() const
    {
        ASSERT(m_type == StartTag || m_type == EndTag);
        return m_attrs.get();
    }

    String characters()
    {
        ASSERT(m_type == Character);
        return String(StringImpl::adopt(m_data));
    }

private:
    Type m_type;

    // "name" for DOCTYPE, StartTag, and EndTag
    // "characters" for Character
    // "data" for Comment
    WTF::Vector<UChar, 1024> m_data;

    // For DOCTYPE
    String m_publicIdentifier;
    String m_systemIdentifier;
    bool m_forceQuirks;

    // For StartTag and EndTag
    bool m_selfClosing;
    RefPtr<NamedMappedAttrMap> m_attrs;
};

}

#endif
