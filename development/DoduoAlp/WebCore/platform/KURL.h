/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef KURL_h
#define KURL_h

#include "PlatformString.h"

#if PLATFORM(CF)
typedef const struct __CFURL* CFURLRef;
#endif

#if PLATFORM(MAC)
#ifdef __OBJC__
@class NSURL;
#else
class NSURL;
#endif
#endif

#if PLATFORM(QT)
QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE
#endif

namespace WebCore {

class TextEncoding;
struct KURLHash;

// FIXME: Our terminology here is a bit inconsistent. We refer to the part
// after the "#" as the "fragment" in some places and the "ref" in others.
// We should fix the terminology to match the URL and URI RFCs as closely
// as possible to resolve this.

class KURL {
public:
    // Generates a URL which contains a null string.
    KURL() { invalidate(); }

    // The argument is an absolute URL string. The string is assumed to be
    // already encoded.
    // FIXME: This constructor has a special case for strings that start with
    // "/", prepending "file://" to such strings; it would be good to get
    // rid of that special case.
    explicit KURL(const char*);

    // The argument is an absolute URL string. The string is assumed to be
    // already encoded.
    // FIXME: For characters with codes other than 0000-00FF will be chopped
    // off, so this call is currently not safe to use with arbitrary strings.
    // FIXME: This constructor has a special case for strings that start with
    // "/", prepending "file://" to such strings; it would be good to get
    // rid of that special case.
    explicit KURL(const String&);

    // Resolves the relative URL with the given base URL. If provided, the
    // TextEncoding is used to encode non-ASCII characers. The base URL can be
    // null or empty, in which case the relative URL will be interpreted as
    // absolute.
    // FIXME: If the base URL is invalid, this always creates an invalid
    // URL. Instead I think it would be better to treat all invalid base URLs
    // the same way we treate null and empty base URLs.
    KURL(const KURL& base, const String& relative);
    KURL(const KURL& base, const String& relative, const TextEncoding&);

    // FIXME: The above functions should be harmonized so that passing a
    // base of null or the empty string gives the same result as the
    // standard String constructor.

    bool isNull() const { return m_string.isNull(); }
    bool isEmpty() const { return m_string.isEmpty(); }

    bool isValid() const { return m_isValid; }

    // Returns true if this URL has a path. Note that "http://foo.com/" has a
    // path of "/", so this function will return true. Only invalid or
    // non-hierarchical (like "javascript:") URLs will have no path.
    bool hasPath() const;

    const String& string() const { return m_string; }

    String protocol() const;
    String host() const;
    unsigned short port() const;
    String user() const;
    String pass() const;
    String path() const;
    String lastPathComponent() const;
    String query() const; // Includes the "?".
    String ref() const; // Does *not* include the "#".
    bool hasRef() const;

    String prettyURL() const;
    String fileSystemPath() const;

    // Returns true if the current URL's protocol is the same as the null-
    // terminated ASCII argument. The argument must be lower-case.
    bool protocolIs(const char*) const;
    bool isLocalFile() const;

    void setProtocol(const String&);
    void setHost(const String&);

    // Setting the port to 0 will clear any port from the URL.
    void setPort(unsigned short);

    // Input is like "foo.com" or "foo.com:8000".
    void setHostAndPort(const String&);

    void setUser(const String&);
    void setPass(const String&);

    // If you pass an empty path for HTTP or HTTPS URLs, the resulting path
    // will be "/".
    void setPath(const String&);

    // The query may begin with a question mark, or, if not, one will be added
    // for you. Setting the query to the empty string will leave a "?" in the
    // URL (with nothing after it). To clear the query, pass a null string.
    void setQuery(const String&);

    void setRef(const String&);
    void removeRef();
    
    friend bool equalIgnoringRef(const KURL&, const KURL&);

    friend bool protocolHostAndPortAreEqual(const KURL&, const KURL&);
    
    operator const String&() const { return m_string; }
    operator JSC::UString() const { return m_string; }

    unsigned hostStart() const { return (m_passwordEnd == m_userStart) ? m_passwordEnd : m_passwordEnd + 1; }
    unsigned hostEnd() const { return m_hostEnd; }
    
    unsigned pathStart() const { return m_portEnd; }
    unsigned pathEnd() const { return m_pathEnd; }
    unsigned pathAfterLastSlash() const { return m_pathAfterLastSlash; }

#if PLATFORM(CF)
    KURL(CFURLRef);
    CFURLRef createCFURL() const;
#endif

#if PLATFORM(MAC)
    KURL(NSURL*);
    operator NSURL*() const;
#endif
#ifdef __OBJC__
    operator NSString*() const { return m_string; }
#endif

#if PLATFORM(QT)
    KURL(const QUrl&);
    operator QUrl() const;
#endif

#ifndef NDEBUG
    void print() const;
#endif

private:
    void invalidate();
    bool isHierarchical() const;
    void init(const KURL&, const String&, const TextEncoding&);
    static bool protocolIs(const String&, const char*);
    void copyToBuffer(Vector<char, 512>& buffer) const;
    
    // Parses the given URL. The originalString parameter allows for an
    // optimization: When the source is the same as the fixed-up string,
    // it will use the passed-in string instead of allocating a new one.
    void parse(const String&);
    void parse(const char* url, const String* originalString);

    String m_string;
    bool m_isValid;
    int m_schemeEnd;
    int m_userStart;
    int m_userEnd;
    int m_passwordEnd;
    int m_hostEnd;
    int m_portEnd;
    int m_pathAfterLastSlash;
    int m_pathEnd;
    int m_queryEnd;
    int m_fragmentEnd;
};

bool operator==(const KURL&, const KURL&);
bool operator==(const KURL&, const String&);
bool operator==(const String&, const KURL&);
bool operator!=(const KURL&, const KURL&);
bool operator!=(const KURL&, const String&);
bool operator!=(const String&, const KURL&);

bool equalIgnoringRef(const KURL&, const KURL&);
bool protocolHostAndPortAreEqual(const KURL&, const KURL&);
    
const KURL& blankURL();

// Functions to do URL operations on strings.
// These are operations that aren't faster on a parsed URL.

bool protocolIs(const String& url, const char* protocol);

String mimeTypeFromDataURL(const String& url);

// Unescapes the given string using URL escaping rules, given an optional
// encoding (defaulting to UTF-8 otherwise). DANGER: If the URL has "%00"
// in it, the resulting string will have embedded null characters!
String decodeURLEscapeSequences(const String&);
String decodeURLEscapeSequences(const String&, const TextEncoding&);

String encodeWithURLEscapeSequences(const String&);

// Inlines.

inline bool operator==(const KURL& a, const KURL& b)
{
    return a.string() == b.string();
}

inline bool operator==(const KURL& a, const String& b)
{
    return a.string() == b;
}

inline bool operator==(const String& a, const KURL& b)
{
    return a == b.string();
}

inline bool operator!=(const KURL& a, const KURL& b)
{
    return a.string() != b.string();
}

inline bool operator!=(const KURL& a, const String& b)
{
    return a.string() != b;
}

inline bool operator!=(const String& a, const KURL& b)
{
    return a != b.string();
}

} // namespace WebCore

namespace WTF {

    // KURLHash is the default hash for String
    template<typename T> struct DefaultHash;
    template<> struct DefaultHash<WebCore::KURL> {
        typedef WebCore::KURLHash Hash;
    };

} // namespace WTF

#endif // KURL_h
