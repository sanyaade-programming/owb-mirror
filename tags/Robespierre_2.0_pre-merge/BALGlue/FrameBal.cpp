/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
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

#include "config.h"
#include "FrameBal.h"

#include "BALConfiguration.h"
#include "BTLogHelper.h"
#include "CString.h"
#include "Document.h"
#include "EventHandler.h"
#include "FocusController.h"
#include "FrameLoader.h"
#include "FrameLoaderClientBal.h"
#include "FramePrivate.h"
#include "FrameView.h"
#include "KeyboardEvent.h"
#include "Page.h"
#include "PlatformWheelEvent.h"
#include "RenderTreeAsText.h"
#include "SelectionController.h"
#include "Settings.h"
#include "TypingCommand.h"

#ifdef __OWB_JS__
#include "bindings/runtime.h"
#include "bindings/runtime_root.h"
#include "ExecState.h"
#include "object.h"
#endif //__OWB_JS__

// This function loads resources from WebKit
// This does not belong here and I'm not sure where
// it should go
// I don't know what the plans or design is
// for none code resources
Vector<char> loadResourceIntoArray(const char* resourceName)
{
    Vector<char> resource;
    //if (strcmp(resourceName,"missingImage") == 0) {
    //}
    return resource;
}

namespace WebCore {

FrameBal::FrameBal(Page* p, HTMLFrameOwnerElement* ownerElement, FrameLoaderClientBal* frameLoader)
    : Frame(p, ownerElement, frameLoader)
{
    Settings* settings = new Settings;
    settings->setLoadsImagesAutomatically(true);
    settings->setMinimumFontSize(5);
    settings->setMinimumLogicalFontSize(5);
    settings->setShouldPrintBackgrounds(true);
#ifdef __OWB_JS__
    settings->setJavaScriptEnabled(true);
#else
    settings->setJavaScriptEnabled(false);
#endif

    settings->setPluginsEnabled(true);
    settings->setDefaultFixedFontSize(14);
    settings->setDefaultFontSize(14);
    setSettings(settings);
	
#ifdef __OWB_WEB_UI__
	if(!ownerElement)
		tree()->setName("_ui");
#endif
    
    RefPtr<WebCore::FrameView> frameView;
    frameView = new FrameView(this);
    frameView->deref();
    setView(frameView.get());
}

FrameBal::~FrameBal()
{
    //hack to free frame
#ifndef NDEBUG
    cancelAllKeepAlive();
#endif //NDEBUG
    //loader()->cancelAndClear();
    //setView(0);
}

void Frame::print()
{
    BALNotImplemented();
}

void Frame::issueTransposeCommand()
{
    BALNotImplemented();
}

void Frame::respondToChangedSelection(WebCore::Selection const&, bool)
{
    // FIXME: If we want continous spell checking, we need to implement this.
}

#ifdef __OWB_JS__
void Frame::cleanupPlatformScriptObjects()
{
}
#endif //__OWB_JS__

bool Frame::isCharacterSmartReplaceExempt(UChar, bool)
{
    // no smart replace
    return true;
}

DragImageRef Frame::dragImageForSelection()
{
    return 0;
}

void Frame::setNeedsReapplyStyles()
{
    BALNotImplemented();
}

#ifndef NDEBUG
void dumpRenderer(WebCore::RenderObject* aRenderObject)
{
    WebCore::DeprecatedString str;
    WebCore::DeprecatedString ind;
    WebCore::TextStream ts(&str);
    if (aRenderObject) {
        logml(MODULE_GLUE, LEVEL_INFO, "DUMP RENDER TREE");
        aRenderObject->dump( &ts, ind );
        log( str.ascii() );
    }
    else
        logml(MODULE_GLUE, LEVEL_INFO, "No renderer");
}

void dumpShowTree(WebCore::RenderObject* aRenderObject)
{
    if (aRenderObject) {
        logm(MODULE_GLUE, "SHOW TREE FOR THIS" );
        aRenderObject->showTreeForThis();
    }
    else
        logm(MODULE_GLUE, "No object");
}

void dumpDOM(WebCore::Node* document)
{
    WebCore::DeprecatedString str;
    WebCore::DeprecatedString ind;
    WebCore::TextStream ts(&str);
    if (document)
        document->dump(&ts, ind);
    else
        ts << "No document\n";

    log(str.ascii());
}

void dumpDebugData(BAL::BIEvent* event, Frame* f)
{
    BAL::BIKeyboardEvent* aKeyboardEvent = event->queryIsKeyboardEvent();
    if (aKeyboardEvent) {
        switch (aKeyboardEvent->virtualKeyCode()) {
        case BAL::BIKeyboardEvent::VK_T:
            if(aKeyboardEvent->ctrlKey()) {
                DeprecatedString txt = WebCore::externalRepresentation(f->renderer());
                WebCore::CString utf8Str = txt.utf8();
                const char *utf8 = utf8Str.data();
                if (utf8)
                    printf("%s", utf8);
            }
            break;
        case BAL::BIKeyboardEvent::VK_D:
            if (aKeyboardEvent->ctrlKey())
                dumpDOM(f->document());
            break;
        case BAL::BIKeyboardEvent::VK_S:
            if (aKeyboardEvent->ctrlKey())
                dumpShowTree(f->renderer());
            break;
        case BAL::BIKeyboardEvent::VK_I:
            if (aKeyboardEvent->ctrlKey() && f->document())
                dumpDOM(f->document()->hoverNode());
            break;
        }
    }
}
#endif

void Frame::handleEvent(BAL::BIEvent *event)
{
//    printf("handleEvent for frame %p\n", this);
    bool isHandled = false;
    EventHandler* handler = eventHandler();
    BAL::BIKeyboardEvent* key = 0;
    BAL::BIMouseEvent* mouseEvent = 0;
    BAL::BIWheelEvent* wheelEvent = 0;
    if ((key = event->queryIsKeyboardEvent()))
        isHandled = handler->keyEvent(*key);
    else if ((mouseEvent = (event->queryIsMouseEvent()))) {
        switch (mouseEvent->eventType()) {
        case BAL::BIMouseEvent::MouseEventPressed:
            isHandled = handler->handleMousePressEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventReleased:
            isHandled = handler->handleMouseReleaseEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventMoved:
            isHandled =  handler->handleMouseMoveEvent(*mouseEvent);
            break;
        case BAL::BIMouseEvent::MouseEventScroll:
            break;
        }
    }
    else if ((wheelEvent = event->queryIsWheelEvent()))
        isHandled = handler->handleWheelEvent(*wheelEvent);

    if (isHandled)
        return;

    WebCore::RenderLayer* renderLayer = 0;
    if (renderer())
        renderLayer = renderer()->layer();

    if (key && !key->isKeyUp() && renderLayer) {
#ifndef NDEBUG
    // first fire debug hook if needed
    dumpDebugData(key, this);
#endif
    switch (key->virtualKeyCode()) {
        // NOTE in all this scrolling we must find a way to scroll content also
        // like m_mainFrame->view()->setContentsPos(renderLayer->scrollXOffset(), renderLayer->scrollYOffset());
        // or else mouse interaction will be shifted after a scroll
        case BAL::BIKeyboardEvent::VK_LEFT:
            if (key->altKey())
                loader()->goBackOrForward(-1);
            else
                view()->scrollBy(-30, 0);
            break;
        case BAL::BIKeyboardEvent::VK_RIGHT:
            if (key->altKey())
                loader()->goBackOrForward(1);
            else
                view()->scrollBy(30, 0);
            break;
        case BAL::BIKeyboardEvent::VK_UP:
//            renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByLine);
            view()->scrollBy(0, -30);
            break;
        case BAL::BIKeyboardEvent::VK_PRIOR:
//            renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByPage);
            view()->scrollBy(0, -300);
            break;
        case BAL::BIKeyboardEvent::VK_NEXT:
//            renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByPage);
            view()->scrollBy(0, 300);
            break;
        case BAL::BIKeyboardEvent::VK_DOWN:
//            renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByLine);
            view()->scrollBy(0, 30);
            break;
        case BAL::BIKeyboardEvent::VK_HOME:
            //renderLayer->scroll(WebCore::ScrollUp, WebCore::ScrollByDocument);
            view()->setContentsPos(0, 0);
            break;
        case BAL::BIKeyboardEvent::VK_END:
            //renderLayer->scroll(WebCore::ScrollDown, WebCore::ScrollByDocument);
            view()->setContentsPos(0, view()->contentsHeight() - view()->visibleHeight());
            break;
        case BAL::BIKeyboardEvent::VK_SPACE:
            if (key->shiftKey())
                view()->scrollBy(0, -300);
            else
                view()->scrollBy(0, 300);
            break;
        case BAL::BIKeyboardEvent::VK_TAB: // On tab, change focus
        {
            WebCore::KeyboardEvent* keyEvent = new WebCore::KeyboardEvent(*key, document()->defaultView());
            page()->focusController()->advanceFocus(keyEvent);
            delete keyEvent;
        }
            break;
        case BAL::BIKeyboardEvent::VK_RETURN: // On enter, dispatch event to the focused node
            break;
        case BAL::BIKeyboardEvent::VK_BACK:
            loader()->goBackOrForward(-1);
            break;
        default:
            break;
        } // end switch
    } // end if key

    // Window events are handled in WindowManager
}

#ifdef __OWB_JS__
void FrameBal::addToJSWindowObject(const char* name, void *object)
{
    KJS::Bindings::RootObject *root = this->bindingRootObject();
    KJS::ExecState *exec = root->interpreter()->globalExec();
    KJS::JSObject *rootObject = root->interpreter()->globalObject();
    KJS::JSObject *window = rootObject->get(exec, KJS::Identifier("window"))->getObject();
    if (!window) {
        return;
    }

    KJS::JSObject *testController =
        KJS::Bindings::Instance::createRuntimeObject(KJS::Bindings::Instance::BalLanguage,
                                                     object, root);

    window->put(exec, KJS::Identifier(name), testController);

}
#endif //__OWB_JS__


}
