// -*- c-basic-offset: 4 -*-
/*
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999-2001 Lars Knoll <knoll@kde.org>
 *                     1999-2001 Antti Koivisto <koivisto@kde.org>
 *                     2000-2001 Simon Hausmann <hausmann@kde.org>
 *                     2000-2001 Dirk Mueller <mueller@kde.org>
 *                     2000 Stefan Schimanski <1Stein@gmx.de>
 * Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Trolltech ASA
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef Frame_h
#define Frame_h

#ifdef __OWB__
#include "BIEvent.h"
#include "BIPainter.h"
#include "GraphicsContext.h"
#include "Widget.h"
#endif //__OWB__
#include "Color.h"
#include "EditAction.h"
#include "DragImage.h"
#include "RenderLayer.h"
#include "TextGranularity.h"
#include "VisiblePosition.h"
#include <wtf/unicode/Unicode.h>
#include <wtf/Forward.h>
#include <wtf/Vector.h>

#ifdef __OWB_JS__
struct NPObject;

namespace KJS {
    class Interpreter;
    
    namespace Bindings {
        class Instance;
        class RootObject;
    }
}
#endif //__OWB_JS__

#if PLATFORM(MAC)
#ifdef __OBJC__
@class NSArray;
@class NSDictionary;
@class NSMenu;
@class NSMutableDictionary;
@class NSString;
@class WebCoreFrameBridge;
@class WebScriptObject;
#else
class NSArray;
class NSDictionary;
class NSMenu;
class NSMutableDictionary;
class NSString;
class WebCoreFrameBridge;
class WebScriptObject;
typedef int NSWritingDirection;
#endif
#endif

namespace WebCore {

class CSSComputedStyleDeclaration;
class CSSMutableStyleDeclaration;
class CSSStyleDeclaration;
class CommandByName;
class DOMWindow;
class Document;
class Editor;
class Element;
class EventHandler;
class FloatRect;
class FrameLoader;
class FrameLoaderClient;
class HTMLFrameOwnerElement;
class HTMLTableCellElement;
class FramePrivate;
class FrameTree;
class FrameView;
class GraphicsContext;
class HTMLFormElement;
class IntRect;
#ifdef __OWB_JS__
class KJSProxy;
#endif //__OWB_JS__
class KURL;
class Node;
class Page;
class Range;
class RenderPart;
class Selection;
class SelectionController;
class Settings;
class Widget;

struct FrameLoadRequest;

template <typename T> class Timer;

#ifdef __OWB__
class Frame : public Shared<Frame>, public BAL::BIPainter {
#else
class Frame : public Shared<Frame> {
#endif
public:
    static double currentPaintTimeStamp() { return s_currentPaintTimeStamp; } // returns 0 if not painting
    
    Frame(Page*, HTMLFrameOwnerElement*, FrameLoaderClient*);
    virtual void setView(FrameView*);
    virtual ~Frame();
    
    void init();

#if PLATFORM(MAC)    
    void setBridge(WebCoreFrameBridge*);
    WebCoreFrameBridge* bridge() const;
#endif

#ifdef __OWB__
    void handleEvent(BAL::BIEvent *event);
#endif
    Page* page() const;
    HTMLFrameOwnerElement* ownerElement() const;

    void pageDestroyed();
    void disconnectOwnerElement();

    Document* document() const;
    FrameView* view() const;

    CommandByName* command() const;
    DOMWindow* domWindow() const;
    Editor* editor() const;
    EventHandler* eventHandler() const;
    FrameLoader* loader() const;
    SelectionController* selectionController() const;
    FrameTree* tree() const;

    RenderObject* renderer() const; // root renderer for the document contained in this frame
    RenderPart* ownerRenderer(); // renderer for the element that contains this frame

    friend class FramePrivate;

    DragImageRef dragImageForSelection();
    
private:
    static double s_currentPaintTimeStamp; // used for detecting decoded resource thrash in the cache
    
    FramePrivate* d;
    
// === undecided, may or may not belong here

public:
    static Frame* frameForWidget(const Widget*);

    Settings* settings() const; // can be NULL
    void reparseConfiguration();

    // should move to FrameView
    void paint(GraphicsContext*, const IntRect&);
    void setPaintRestriction(PaintRestriction);
    bool isPainting() const;

    void setUserStyleSheetLocation(const KURL&);
    void setUserStyleSheet(const String& styleSheetData);

    void setZoomFactor(int percent);
    int zoomFactor() const;

    void setPrinting(bool printing, float minPageWidth, float maxPageWidth, bool adjustViewSize);

    bool inViewSourceMode() const;
    void setInViewSourceMode(bool = true) const;

// #ifdef __OWB_JS__
    void setJSStatusBarText(const String&);
    void setJSDefaultStatusBarText(const String&);
    String jsStatusBarText() const;
    String jsDefaultStatusBarText() const;
// #endif //__OWB_JS__

    void keepAlive(); // Used to keep the frame alive when running a script that might destroy it.
#ifndef NDEBUG
    static void cancelAllKeepAlive();
#endif

#ifdef __OWB_JS__
    KJS::Bindings::Instance* createScriptInstanceForWidget(Widget*);
    KJS::Bindings::RootObject* bindingRootObject();
    
    PassRefPtr<KJS::Bindings::RootObject> createRootObject(void* nativeHandle, PassRefPtr<KJS::Interpreter>);
#endif //__OWB_JS__

#if PLATFORM(MAC)
    WebScriptObject* windowScriptObject();
#endif

#if USE(NPOBJECT)
    NPObject* windowScriptNPObject();
#endif    
    
    void setDocument(PassRefPtr<Document>);
#ifdef __OWB_JS__
    KJSProxy* scriptProxy();
#endif
    bool isFrameSet() const;

    void adjustPageHeight(float* newBottom, float oldTop, float oldBottom, float bottomLimit);

    void forceLayout(bool allowSubtree = false);
    void forceLayoutWithPageWidthRange(float minPageWidth, float maxPageWidth, bool adjustViewSize);

    void sendResizeEvent();
    void sendScrollEvent();

    void clearTimers();
    static void clearTimers(FrameView*);

    bool isActive() const;
    void setIsActive(bool flag);
    void setWindowHasFocus(bool flag);

    // Convenience, to avoid repeating the code to dig down to get this.
    UChar backslashAsCurrencySymbol() const;

    void setNeedsReapplyStyles();
    String documentTypeString() const;

    bool prohibitsScrolling() const;
    void setProhibitsScrolling(const bool);

    void dashboardRegionsChanged();

    void clearScriptProxy();
    void clearDOMWindow();
#ifdef __OWB_JS__
    void clearScriptObjects();
    void cleanupScriptObjectsForPlugin(void*);
#endif
private:
#ifdef __OWB_JS__
    void clearPlatformScriptObjects();
#endif
    void lifeSupportTimerFired(Timer<Frame>*);
    
// === to be moved into Chrome

public:
    void focusWindow();
    void unfocusWindow();
    bool shouldClose();
    void scheduleClose();

// === to be moved into Editor

public:
    String selectedText() const;  
    bool findString(const String&, bool forward, bool caseFlag, bool wrapFlag, bool startInSelection);

    const Selection& mark() const; // Mark, to be used as emacs uses it.
    void setMark(const Selection&);

    void transpose();

    void computeAndSetTypingStyle(CSSStyleDeclaration* , EditAction = EditActionUnspecified);
    enum TriState { falseTriState, trueTriState, mixedTriState };
    TriState selectionHasStyle(CSSStyleDeclaration*) const;
    String selectionStartStylePropertyValue(int stylePropertyID) const;
    void applyEditingStyleToBodyElement() const;
    void removeEditingStyleFromBodyElement() const;
    void applyEditingStyleToElement(Element*) const;
    void removeEditingStyleFromElement(Element*) const;

    IntRect firstRectForRange(Range*) const;
    
#if PLATFORM(MAC)
    void issuePasteCommand();
#endif
    void issueTransposeCommand();
    void respondToChangedSelection(const Selection& oldSelection, bool closeTyping);
    bool shouldChangeSelection(const Selection& oldSelection, const Selection& newSelection, EAffinity, bool stillSelecting) const;

    RenderStyle* styleForSelectionStart(Node*& nodeToRemove) const;

    unsigned markAllMatchesForText(const String&, bool caseFlag, unsigned limit);
    bool markedTextMatchesAreHighlighted() const;
    void setMarkedTextMatchesAreHighlighted(bool flag);

    CSSComputedStyleDeclaration* selectionComputedStyle(Node*& nodeToRemove) const;

    void textFieldDidBeginEditing(Element*);
    void textFieldDidEndEditing(Element*);
    void textDidChangeInTextField(Element*);
    bool doTextFieldCommandFromEvent(Element*, KeyboardEvent*);
    void textWillBeDeletedInTextField(Element* input);
    void textDidChangeInTextArea(Element*);

// === to be moved into SelectionController

public:
    TextGranularity selectionGranularity() const;
    void setSelectionGranularity(TextGranularity) const;

    bool shouldChangeSelection(const Selection&) const;
    bool shouldDeleteSelection(const Selection&) const;
    void clearCaretRectIfNeeded();
    void setFocusedNodeIfNeeded();
    void selectionLayoutChanged();
    void notifyRendererOfSelectionChange(bool userTriggered);

    void invalidateSelection();

    void setCaretVisible(bool = true);
    void paintCaret(GraphicsContext*, const IntRect&) const;  
    void paintDragCaret(GraphicsContext*, const IntRect&) const;

    bool isContentEditable() const; // if true, everything in frame is editable

    void updateSecureKeyboardEntryIfActive();

    CSSMutableStyleDeclaration* typingStyle() const;
    void setTypingStyle(CSSMutableStyleDeclaration*);
    void clearTypingStyle();

    FloatRect selectionRect(bool clipToVisibleContent = true) const;
    void selectionTextRects(Vector<FloatRect>&, bool clipToVisibleContent = true) const;

    HTMLFormElement* currentForm() const;

    void revealSelection(const RenderLayer::ScrollAlignment& = RenderLayer::gAlignCenterIfNeeded) const;
    void revealCaret(const RenderLayer::ScrollAlignment& = RenderLayer::gAlignCenterIfNeeded) const;
    void setSelectionFromNone();

private:
    void caretBlinkTimerFired(Timer<Frame>*);
    void setUseSecureKeyboardEntry(bool);

public:
    SelectionController* dragCaretController() const;

    String searchForLabelsAboveCell(RegularExpression*, HTMLTableCellElement*);
    String searchForLabelsBeforeElement(const Vector<String>& labels, Element*);
    String matchLabelsAgainstElement(const Vector<String>& labels, Element*);
    
    VisiblePosition visiblePositionForPoint(const IntPoint& framePoint);
    Document* documentAtPoint(const IntPoint& windowPoint);
#if PLATFORM(MAC)

// === undecided, may or may not belong here

public:
    NSString* searchForNSLabelsAboveCell(RegularExpression*, HTMLTableCellElement*);
    NSString* searchForLabelsBeforeElement(NSArray* labels, Element*);
    NSString* matchLabelsAgainstElement(NSArray* labels, Element*);

    NSMutableDictionary* dashboardRegionsDictionary();

    void willPopupMenu(NSMenu*);

    NSImage* selectionImage(bool forceBlackText = false) const;
    NSImage* snapshotDragImage(Node*, NSRect* imageRect, NSRect* elementRect) const;

private:    
    NSImage* imageFromRect(NSRect) const;

// === to be moved into Chrome

public:
    FloatRect customHighlightLineRect(const AtomicString& type, const FloatRect& lineRect, Node*);
    void paintCustomHighlight(const AtomicString& type, const FloatRect& boxRect, const FloatRect& lineRect, bool text, bool line, Node*);

// === to be moved into Editor

public:
    NSDictionary* fontAttributesForSelectionStart() const;
    NSWritingDirection baseWritingDirectionForSelectionStart() const;

#endif

};

} // namespace WebCore

#endif // Frame_h
