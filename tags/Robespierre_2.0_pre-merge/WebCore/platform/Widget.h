/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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

#ifndef BTWidget_h
#define BTWidget_h

#include <wtf/Platform.h>
#ifdef __OWB__
#include "Font.h"
#include "PlatformMouseEvent.h"
#endif

#if PLATFORM(MAC)
#ifdef __OBJC__
@class NSView;
#else
class NSView;
#endif
#endif

#if PLATFORM(WIN)
typedef struct HWND__* HWND;
#endif

#if PLATFORM(GDK)
typedef struct _GdkDrawable GdkDrawable;
#endif

#if PLATFORM(QT)
class QWidget;
#endif

namespace WebCore {

    class Cursor;
    class Event;
    class Font;
    class GraphicsContext;
    class IntPoint;
    class IntRect;
    class IntSize;
    class PlatformMouseEvent;
#ifndef __OWB__
    class ScrollView;
#endif
    class WidgetClient;
    class WidgetPrivate;
#ifdef __OWB__
}
using WebCore::IntSize;
using WebCore::IntPoint;
using WebCore::IntRect;
using WebCore::Cursor;
using WebCore::WidgetClient;
using WebCore::WidgetPrivate;
using WebCore::Event;
using WebCore::Font;
using WebCore::GraphicsContext;
namespace BAL {
    class BIWindow;
#endif
    class BTWidget {
    public:
        BTWidget();
        virtual ~BTWidget();

        virtual void setEnabled(bool);
        virtual bool isEnabled() const;

        int x() const;
        int y() const;
        int width() const;
        int height() const;
        IntSize size() const;
        void resize(int, int);
        void resize(const IntSize&);
        IntPoint pos() const;
        void move(int, int);
        void move(const IntPoint&);

        virtual void paint(GraphicsContext*, const IntRect&);
        virtual void invalidate();
        virtual void invalidateRect(const IntRect&);

        virtual void setFrameGeometry(const IntRect&);
        virtual IntRect frameGeometry() const;

        virtual void setFocus();

        void setCursor(const Cursor&);
        Cursor cursor();

        virtual void show();
        virtual void hide();

        void setIsSelected(bool);

        void setClient(WidgetClient*);
        WidgetClient* client() const;

        virtual bool isFrameView() const;

        virtual void removeFromParent();

        // This method is used by plugins on all platforms to obtain a clip rect that includes clips set by WebCore,
        // e.g., in overflow:auto sections.  The clip rects coordinates are in the containing window's coordinate space.
        // This clip includes any clips that the widget itself sets up for its children.
        virtual IntRect windowClipRect() const;

        virtual void handleEvent(Event*) { }

#if PLATFORM(WIN)
        void setContainingWindow(HWND);
        HWND containingWindow() const;

        virtual void setParent(ScrollView*);
        ScrollView* parent() const;

        virtual void geometryChanged() const {};

        bool capturingMouse() const;
        void setCapturingMouse(bool);
        Widget* capturingTarget();
        Widget* capturingChild();
        void setCapturingChild(Widget*);

        IntRect convertToContainingWindow(const IntRect&) const;
        IntPoint convertToContainingWindow(const IntPoint&) const;
        IntPoint convertFromContainingWindow(const IntPoint&) const;

        virtual IntPoint convertChildToSelf(const Widget*, const IntPoint&) const;
        virtual IntPoint convertSelfToChild(const Widget*, const IntPoint&) const;

        bool suppressInvalidation() const;
        void setSuppressInvalidation(bool);

        // These methods will be called on a widget while it is capturing the mouse.
        virtual bool handleMouseMoveEvent(const PlatformMouseEvent&) { return false; }
        virtual bool handleMouseReleaseEvent(const PlatformMouseEvent&) { return false; }
#endif

#ifdef __OWB__
        virtual void setBackingStore(BINativeImage*) {}
        virtual BINativeImage* backingStore() const { return NULL; }
        virtual const IntRect* dirtyRect() const { return NULL; }
        virtual void setDirtyRect(IntRect) {}
        virtual void setParent(BTWidget* parent);
#endif

#if PLATFORM(GDK)
        Widget(GdkDrawable*);
        virtual void setDrawable(GdkDrawable*);
        GdkDrawable* drawable() const;
#endif

#if PLATFORM(QT)
        QWidget* qwidget() const;
        QWidget* canvas() const;
        void setQWidget(QWidget*);
#endif

#if PLATFORM(MAC)
        Widget(NSView*);

        NSView* getView() const;
        NSView* getOuterView() const;
        void setView(NSView*);

        void sendConsumedMouseUp();

        static void beforeMouseDown(NSView*, Widget*);
        static void afterMouseDown(NSView*, Widget*);

        void addToSuperview(NSView* superview);
        void removeFromSuperview();
#endif

        // To be deleted.
        enum FocusPolicy { NoFocus, TabFocus, ClickFocus, StrongFocus, WheelFocus };
        GraphicsContext* lockDrawingFocus();
        const Font& font() const;
        virtual FocusPolicy focusPolicy() const;
        virtual bool hasFocus() const;
        virtual void clearFocus();
        virtual void setFont(const Font&);
        void disableFlushDrawing();
        void enableFlushDrawing();
        void unlockDrawingFocus(GraphicsContext*);

    private:
        WidgetPrivate* data;
    };

} // namespace WebCore

#endif // BTWidget_h
