/*
 * Copyright (C) 2009 Joerg Strohmayer.
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
#ifndef WebViewPrivate_H
#define WebViewPrivate_H

#include "WebView.h"
#include "IntRect.h"
#include "FrameView.h"
#include "Frame.h"
#include "BALBase.h"
#include "cairo.h"
#include "WebNotificationDelegate.h"

class AmigaWebNotificationDelegate : public WebNotificationDelegate
{
public:
    AmigaWebNotificationDelegate();
    ~AmigaWebNotificationDelegate();

    virtual void startLoadNotification(WebFrame* webFrame);
    virtual void progressNotification(WebFrame* webFrame);
    virtual void finishedLoadNotification(WebFrame* webFrame);

private:
    bool m_OS41;
};

class WebViewPrivate {
public:
    WebViewPrivate(WebView *webView);
    ~WebViewPrivate();

    void show()
    {
    }
    
    void hide()
    {
    }

    void setFrameRect(WebCore::IntRect r)
    {
        m_rect = r;
    }

    WebCore::IntRect frameRect()
    { 
        return m_rect; 
    }
    
    BalWidget *createWindow(BalRectangle r)
    {
        WebCore::IntRect rect(r.x, r.y, r.w, r.h);
        if(rect != m_rect)
            m_rect = rect;
    

        return 0;
    }

    void initWithFrameView(WebCore::FrameView *frameView)
    {
        m_webView->setCustomHTMLTokenizerTimeDelay(1.0);       // seconds, default: 0.500
        m_webView->setCustomHTMLTokenizerChunkSize(16 * 1024); // bytes, default: 4096
    }

    void clearDirtyRegion()
    {
        m_backingStoreDirtyRegion.setX(0);
        m_backingStoreDirtyRegion.setY(0);
        m_backingStoreDirtyRegion.setWidth(0);
        m_backingStoreDirtyRegion.setHeight(0);
    }

    BalRectangle dirtyRegion()
    {
        BalRectangle rect = {m_backingStoreDirtyRegion.x(), m_backingStoreDirtyRegion.y(), m_backingStoreDirtyRegion.width(), m_backingStoreDirtyRegion.height()};
        return rect;
    }

    void addToDirtyRegion(const BalRectangle& dirtyRect)
    {
        m_backingStoreDirtyRegion.unite(dirtyRect);
    }

    void onExpose(BalEventExpose event);
    void onKeyDown(BalEventKey event);
    void onKeyUp(BalEventKey event);
    void onMouseMotion(BalEventMotion event);
    void onMouseButtonDown(BalEventButton event);
    void onMouseButtonUp(BalEventButton event);
    void onScroll(BalEventScroll event);
    void onResize(BalResizeEvent event);
    void onQuit(BalQuitEvent);
    void onUserEvent(BalUserEvent);
    void popupMenuHide();
    void popupMenuShow(void *popupInfo);

    void sendExposeEvent(WebCore::IntRect);
    
    void repaint(const WebCore::IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false);
    
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);
    
    void fireWebKitEvents();
    void runJavaScriptAlert(WebFrame* frame, const char* message);
    
private:
    void updateView(BalWidget *widget, WebCore::IntRect rect);
    WebCore::IntRect m_rect;
    WebView *m_webView;
    bool isInitialized;

    WebCore::IntPoint m_backingStoreSize;
    WebCore::IntRect m_backingStoreDirtyRegion;
    AmigaWebNotificationDelegate* m_amigaWebNotificationDelegate;
};


#endif
