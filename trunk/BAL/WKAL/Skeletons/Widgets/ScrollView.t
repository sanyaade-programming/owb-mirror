/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef ScrollView_h
#define ScrollView_h
/**
 *  @file  ScrollView.t
 *  ScrollView description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "IntRect.h"
#include "ScrollTypes.h"
#include "Widget.h"
#include <wtf/HashSet.h>





namespace WKAL {

    class FloatRect;
    class PlatformWheelEvent;
    class Scrollbar;

    class ScrollView : public WKALBase, public Widget {
    public:
    /**
     * ScrollView default constructor
     * @code
     * ScrollView s;
     * @endcode
     */
        ScrollView();

    /**
     * ScrollView destructor
     * @code
     * delete s;
     * @endcode
     */
        ~ScrollView();

    /**
     * get visible width
     * @param[out] : width
     * @code
     * int w = s->visibleWidth();
     * @endcode
     */
        int visibleWidth() const;

    /**
     * get visible height
     * @param[out] : height
     * @code
     * int h = s->visibleHeight();
     * @endcode
     */
        int visibleHeight() const;

    /**
     * get visible content rect 
     * @param[out] : rect
     * @code
     * FloatRect fp = s->visibleContentRect();
     * @endcode
     */
        FloatRect visibleContentRect() const;

    /**
     * get visible content rect considering external scrollers
     * @param[out] : rect
     * @code
     * FloatRect fp = s->visibleContentRectConsideringExternalScrollers();
     * @endcode
     */
        FloatRect visibleContentRectConsideringExternalScrollers() const;

    /**
     * get contents width
     * @param[out] : width
     * @code
     * int w = s->contentsWidth();
     * @endcode
     */
        int contentsWidth() const;

    /**
     * get contents height
     * @param[out] : height
     * @code
     * int h = s->contentsHeight();
     * @endcode
     */
        int contentsHeight() const;

    /**
     * get contents x
     * @param[out] : x value
     * @code
     * int x = s->contentsX();
     * @endcode
     */
        int contentsX() const;

    /**
     * get contents y
     * @param[out] : y value
     * @code
     * int y = s->contentsY();
     * @endcode
     */
        int contentsY() const;

    /**
     * get scroll offset 
     * @param[out] : offset
     * @code
     * IntSize is = s->scrollOffset();
     * @endcode
     */
        IntSize scrollOffset() const;

    /**
     * scroll By 
     * @param[in] : delta x
     * @param[in] : delta y
     * @code
     * s->scrollBy(dx, dy);
     * @endcode
     */
        void scrollBy(int dx, int dy);

    /**
     * scroll rect into view recursively
     * @param[in] : rect
     * @code
     * s->scrollRectIntoViewRecursively(r);
     * @endcode
     */
        virtual void scrollRectIntoViewRecursively(const IntRect&);

    /**
     * set contents pos
     * @param[in] : x value
     * @param[in] : y value
     * @code
     * s->setContentsPos(x, y);
     * @endcode
     */
        virtual void setContentsPos(int x, int y);

    /**
     * set Vertical scrollbar mode
     * @param[in] : scrollbal mode
     * @code
     * s->setVScrollbarMode(m);
     * @endcode
     */
        virtual void setVScrollbarMode(ScrollbarMode);

    /**
     * set horizontal scrollbar mode
     * @param[in] : scrollbar mode
     * @code
     * s->setHScrollbarMode(m);
     * @endcode
     */
        virtual void setHScrollbarMode(ScrollbarMode);

        // Set the mode for both scrollbars at once.
    /**
     * Set the mode for both scrollbars at once.
     * @param[in] : scrollbar mode
     * @code
     * s->setScrollbarsMode(m);
     * @endcode
     */
        virtual void setScrollbarsMode(ScrollbarMode);

    /**
     * This gives us a means of blocking painting on our scrollbars until the first layout has occurred.
     * @param[in] : suppressed
     * @param[in] : repaint on unsuppress
     * @code
     * s->suppressScrollbars(su, r);
     * @endcode
     */
        void suppressScrollbars(bool suppressed, bool repaintOnUnsuppress = false);

    /**
     * get vertical scrollbar mode
     * @param[out] : scrollbar mode
     * @code
     * ScrollbarMode m = s->vScrollbarMode();
     * @endcode
     */
        ScrollbarMode vScrollbarMode() const;

    /**
     * get horizontal scrollbar mode
     * @param[out] : scrollbar mode
     * @code
     * ScrollbarMode m = s->hScrollbarMode();
     * @endcode
     */
        ScrollbarMode hScrollbarMode() const;

    /**
     * Check wether the scrollView is scrollable or not.
     * @param[out] : a boolean which represent scrollable status for the scrollView.
     * @code
     * @endcode
     */
        bool isScrollable();
    /**
     * add child 
     * @param[in] : widget
     * @code
     * s->addChild(w);
     * @endcode
     */
        void addChild(Widget*);

    /**
     * remove child
     * @param[in] : widget
     * @code
     * s->removeChild(w);
     * @endcode
     */
        void removeChild(Widget*);

    /**
     * @see Widget
     */
        virtual void resizeContents(int w, int h);

    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : point
     * @param[out] : result point
     * @code
     * IntPoint p = s->windowToContents(p1);
     * @endcode
     */
        IntPoint windowToContents(const IntPoint&) const;

    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : point
     * @param[out] : result rect
     * @code
     * IntRect p = s->windowToContents(p1);
     * @endcode
     */
        IntRect contentsToWindow(const IntPoint&) const;

    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect p = s->windowToContents(p1);
     * @endcode
     */
        IntRect windowToContents(const IntRect&) const;

    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect p = s->windowToContents(p1);
     * @endcode
     */
        IntRect contentsToWindow(const IntRect&) const;

    /**
     * set static background
     * @param[in] : status
     * @code
     * s->setStaticBackground(true);
     * @endcode
     */
        void setStaticBackground(bool);

    /**
     * in window
     * @param[out] : status
     * @code
     * bool w = s->inWindow();
     * @endcode
     */
        bool inWindow() const;



    /**
     * should Update while offscreen
     */
        virtual bool shouldUpdateWhileOffscreen() const = 0;

    /**
     * scrollbarUnderMouse 
     */
     Scrollbar* scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent);

    /**
     * This method exists for scrollviews that need to handle wheel events manually.
     * On Mac the underlying NSScrollView just does the scrolling, but on other platforms
     * (like Windows), we need this method in order to do the scroll ourselves.
     * @param[in] : platform wheel event
     * @code
     * s->wheelEvent(w);
     * @endcode
     */
        void wheelEvent(PlatformWheelEvent&);

    /**
     * scroll
     * @param[in] : direction
     * @param[in] : granularity
     * @param[out] : status
     * @code
     * bool sc = s->scroll(d, g);
     * @endcode
     */
        bool scroll(ScrollDirection, ScrollGranularity);

    /**
     * convert child to self. 
     * @param[in] : a pointer to a widget.
     * @param[in] : an IntPoint.
     * @param[out] : an IntPoint.
     * @code
     * @endcode
     */
        IntPoint convertChildToSelf(const Widget* child, const IntPoint& point) const;


    /**
     * convert self to child. 
     * @param[in] : a pointer to a widget.
     * @param[in] : an IntPoint.
     * @param[out] : an IntPoint.
     * @code
     * @endcode
     */
        IntPoint convertSelfToChild(const Widget* child, const IntPoint& point) const;


    /**
     * is scrollView scrollbar.
     * @param[in] : a pointer to a widget.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
        bool isScrollViewScrollbar(const Widget*) const;

#if HAVE(ACCESSIBILITY)


    /**
     * get contents to screen 
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect r = s->contentsToScreen(r1);
     * @endcode
     */
        IntRect contentsToScreen(const IntRect&) const;

    /**
     * get screen to contents 
     * @param[in] : point
     * @param[out] : result point
     * @code
     * IntPoint p = s->screenToContents(p1);
     * @endcode
     */
        IntPoint screenToContents(const IntPoint&) const;
#endif

   protected:
       /**
        * updateContents
	*/
        void updateContents(const IntRect&, bool now = false);
       /**
        * updateWindowRect
	*/
        void updateWindowRect(const IntRect&, bool now = false);
    public:
       /**
        * update
	*/
        void update();

    public:
       /**
        * children
        */
        HashSet<Widget*>* children();

    private:
       /**
        * maximumScroll
        */
        IntSize maximumScroll() const;

        class ScrollViewPrivate;
        ScrollViewPrivate* m_data;

    public:
    /**
     * @see Widget
     */
        virtual void paint(GraphicsContext*, const IntRect&);

    /**
     * geometry changed.
     * @code
     * @endcode
     */
        virtual void geometryChanged() const;

    /**
     * setFrameGeometry
     */
        virtual void setFrameGeometry(const IntRect&);

    /**
     * add to dirty region 
     * @param[in] : dirty region
     * @code
     * s->addToDirtyRegion(r);
     * @endcode
     */
        void addToDirtyRegion(const IntRect&);

    /**
     * scrollBackingStore
     */
        void scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect);

    /**
     * update backing store
     * @code
     * s->updateBackingStore();
     * @endcode
     */
        void updateBackingStore();

    private:
     /**
      * updateScrollbars
      */
        void updateScrollbars(const IntSize& desiredOffset);


    public:

    /**
     * windowResizerRect 
     * @param[out] : a rectangle.
     * @code
     * @endcode
     */
        IntRect windowResizerRect() ;

    /**
     * resizerOverlapsContent
     */
        bool resizerOverlapsContent() const;

    /**
     * adjustOverlappingScrollbarCount.
     * @param[in] : a integer representing the overlap delta.
     * @code
     * @endcode
     */
        void adjustOverlappingScrollbarCount(int overlapDelta) ;

    public:
    /**
     * setGtkAdjustments
     */
        void setBalAdjustments(BalAdjustment* hadj, BalAdjustment* vadj);

    };

    /**
     *  ScrollView::contentsToWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline IntRect ScrollView::contentsToWindow(const IntRect& rect) const;
    /**
     *  ScrollView::windowToContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline IntRect ScrollView::windowToContents(const IntRect& rect) const;
    
    
} // namespace
#endif // ScrollView_h

