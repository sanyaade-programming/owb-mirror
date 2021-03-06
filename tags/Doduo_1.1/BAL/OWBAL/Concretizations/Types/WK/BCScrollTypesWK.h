/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef ScrollTypes_h
#define ScrollTypes_h

namespace OWBAL {

    enum ScrollDirection {
        ScrollUp,
        ScrollDown,
        ScrollLeft,
        ScrollRight
    };

    enum ScrollGranularity {
        ScrollByLine,
        ScrollByPage,
        ScrollByDocument,
        ScrollByPixel
    };

    enum ScrollbarOrientation { HorizontalScrollbar, VerticalScrollbar };

    enum ScrollbarMode { ScrollbarAuto, ScrollbarAlwaysOff, ScrollbarAlwaysOn };

    enum ScrollbarControlSize { RegularScrollbar, SmallScrollbar, MiniScrollbar };

    typedef unsigned ScrollbarControlState;

    enum ScrollbarControlStateMask {
        ActiveScrollbarState = 1,
        EnabledScrollbarState = 1 << 1,
        PressedScrollbarState = 1 << 2,
    };

    enum ScrollbarPart {
        NoPart = 0,
        BackButtonPart = 1,
        BackTrackPart = 1 << 1,
        ThumbPart = 1 << 2,
        ForwardTrackPart = 1 << 3,
        ForwardButtonPart = 1 << 4,
        AllParts = 0xffffffff,
    };

    typedef unsigned ScrollbarControlPartMask;

}

#endif
