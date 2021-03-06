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

#ifndef InspectorClientBal_h
#define InspectorClientBal_h

#include "InspectorClient.h"
#include "InspectorFrontendClientLocal.h"

#include "SharedPtr.h"

namespace WebCore {
    class Node;
    class Page;
    class String;
}

class WebView;

#if ENABLE(DAE)
class WebApplication;
#endif

class WebInspectorClient : public WebCore::InspectorClient {
public:
    WebInspectorClient(WebView*);
    virtual ~WebInspectorClient();

    virtual void inspectorDestroyed();

    virtual void openInspectorFrontend(WebCore::InspectorController*);

    virtual void highlight(WebCore::Node*);
    virtual void hideHighlight();

    virtual void populateSetting(const WebCore::String& key, WebCore::String* value);
    virtual void storeSetting(const WebCore::String& key, const WebCore::String& value);

    void updateHighlight();

private:
    void loadSettings();
    void saveSettings();

    WebView* m_webView;

    typedef HashMap<WebCore::String, WebCore::String> SettingsMap;
    OwnPtr<SettingsMap> m_settings;
#if ENABLE(DAE)
    SharedPtr<WebApplication> m_application;
#endif
};

class WebInspectorFrontendClient : public WebCore::InspectorFrontendClientLocal {
public:
    WebInspectorFrontendClient(WebView* inspectedWebView, WebView* frontendWebView, WebInspectorClient* inspectorClient);

    virtual void frontendLoaded();
    
    virtual WebCore::String localizedStringsURL();
    virtual WebCore::String hiddenPanels();
    
    virtual void bringToFront();
    virtual void closeWindow();
    
    virtual void attachWindow();
    virtual void detachWindow();
    
    virtual void setAttachedWindowHeight(unsigned height);
    virtual void inspectedURLChanged(const WebCore::String& newURL);

private:
    ~WebInspectorFrontendClient();

    void destroyInspectorView();

    WebView* m_inspectedWebView;
    WebInspectorClient* m_inspectorClient;
    WebView* m_frontendWebView;

    bool m_shouldAttachWhenShown;
    bool m_attached;

    WebCore::String m_inspectedURL;
    bool m_destroyingInspectorView;
#if ENABLE(DAE)
    SharedPtr<WebApplication> m_application;
#endif
};

#endif
