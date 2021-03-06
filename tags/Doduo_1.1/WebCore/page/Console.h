/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Console_h
#define Console_h

#include "PlatformString.h"
#include <profiler/Profiler.h>
#include <wtf/RefCounted.h>
#include <wtf/PassRefPtr.h>

namespace KJS {
    class ExecState;
    class ArgList;
    class Profile;
    class JSValue;
}

namespace WebCore {

    class Frame;
    class String;

    enum MessageSource {
        HTMLMessageSource,
        XMLMessageSource,
        JSMessageSource,
        CSSMessageSource,
        OtherMessageSource
    };

    enum MessageLevel {
        TipMessageLevel,
        LogMessageLevel,
        WarningMessageLevel,
        ErrorMessageLevel
    };

    class Console : public RefCounted<Console>, public KJS::ProfilerClient {
    public:
        static PassRefPtr<Console> create(Frame* frame) { return adoptRef(new Console(frame)); }

        void disconnectFrame();

        void addMessage(MessageSource, MessageLevel, const String& message, unsigned lineNumber, const String& sourceURL);

        void debug(KJS::ExecState*, const KJS::ArgList& arguments);
        void error(KJS::ExecState*, const KJS::ArgList& arguments);
        void info(KJS::ExecState*, const KJS::ArgList& arguments);
        void log(KJS::ExecState*, const KJS::ArgList& arguments);
        void warn(KJS::ExecState*, const KJS::ArgList& arguments);
        void assertCondition(bool condition, KJS::ExecState*, const KJS::ArgList& arguments);
        void profile(KJS::ExecState*, const KJS::ArgList& arguments);
        void profileEnd(KJS::ExecState*, const KJS::ArgList& arguments);
        void time(const KJS::UString& title);
        void timeEnd(const KJS::UString& title);

        void finishedProfiling(PassRefPtr<KJS::Profile>);

        void reportException(KJS::ExecState*, KJS::JSValue*);
        void reportCurrentException(KJS::ExecState*);
    private:
        Console(Frame*);
        
        Frame* m_frame;
    };

} // namespace WebCore

#endif // Console_h
