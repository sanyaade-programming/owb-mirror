/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
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

#ifndef WebPreferences_H
#define WebPreferences_H

#include "IWebPreferences.h"
#include "IWebPreferencesPrivate.h"
#include <CoreFoundation/CoreFoundation.h>
#include <WebCore/BString.h>
#include <wtf/RetainPtr.h>

class WebPreferences : public IWebPreferences, public IWebPreferencesPrivate {
public:
    static WebPreferences* createInstance();
protected:
    WebPreferences();
    ~WebPreferences();

public:
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // IWebPreferences
    virtual HRESULT STDMETHODCALLTYPE standardPreferences( 
        /* [retval][out] */ IWebPreferences** standardPreferences);
    
    virtual HRESULT STDMETHODCALLTYPE initWithIdentifier( 
        /* [in] */ BSTR anIdentifier,
        /* [retval][out] */ IWebPreferences** preferences);
    
    virtual HRESULT STDMETHODCALLTYPE identifier( 
        /* [retval][out] */ BSTR* ident);
    
    virtual HRESULT STDMETHODCALLTYPE standardFontFamily( 
        /* [retval][out] */ BSTR* family);
    
    virtual HRESULT STDMETHODCALLTYPE setStandardFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE fixedFontFamily( 
        /* [retval][out] */ BSTR* family);
    
    virtual HRESULT STDMETHODCALLTYPE setFixedFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE serifFontFamily( 
        /* [retval][out] */ BSTR* fontFamily);
    
    virtual HRESULT STDMETHODCALLTYPE setSerifFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE sansSerifFontFamily( 
        /* [retval][out] */ BSTR* family);
    
    virtual HRESULT STDMETHODCALLTYPE setSansSerifFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE cursiveFontFamily( 
        /* [retval][out] */ BSTR* family);
    
    virtual HRESULT STDMETHODCALLTYPE setCursiveFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE fantasyFontFamily( 
        /* [retval][out] */ BSTR* family);
    
    virtual HRESULT STDMETHODCALLTYPE setFantasyFontFamily( 
        /* [in] */ BSTR family);
    
    virtual HRESULT STDMETHODCALLTYPE defaultFontSize( 
        /* [retval][out] */ int* fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE setDefaultFontSize( 
        /* [in] */ int fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE defaultFixedFontSize( 
        /* [retval][out] */ int* fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE setDefaultFixedFontSize( 
        /* [in] */ int fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE minimumFontSize( 
        /* [retval][out] */ int* fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE setMinimumFontSize( 
        /* [in] */ int fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE minimumLogicalFontSize( 
        /* [retval][out] */ int* fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE setMinimumLogicalFontSize( 
        /* [in] */ int fontSize);
    
    virtual HRESULT STDMETHODCALLTYPE defaultTextEncodingName( 
        /* [retval][out] */ BSTR* name);
    
    virtual HRESULT STDMETHODCALLTYPE setDefaultTextEncodingName( 
        /* [in] */ BSTR name);
    
    virtual HRESULT STDMETHODCALLTYPE userStyleSheetEnabled( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setUserStyleSheetEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE userStyleSheetLocation( 
        /* [retval][out] */ BSTR* location);
    
    virtual HRESULT STDMETHODCALLTYPE setUserStyleSheetLocation( 
        /* [in] */ BSTR location);
    
    virtual HRESULT STDMETHODCALLTYPE isJavaEnabled( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setJavaEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE isJavaScriptEnabled( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setJavaScriptEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE javaScriptCanOpenWindowsAutomatically( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setJavaScriptCanOpenWindowsAutomatically( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE arePlugInsEnabled( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setPlugInsEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE allowsAnimatedImages( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setAllowsAnimatedImages( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE allowAnimatedImageLooping( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setAllowAnimatedImageLooping( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setLoadsImagesAutomatically( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE loadsImagesAutomatically( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setAutosaves( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE autosaves( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setShouldPrintBackgrounds( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE shouldPrintBackgrounds( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setPrivateBrowsingEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE privateBrowsingEnabled( 
        /* [retval][out] */ BOOL* enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setTabsToLinks( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE tabsToLinks( 
        /* [retval][out] */ BOOL* enabled);

    virtual HRESULT STDMETHODCALLTYPE textAreasAreResizable( 
        /* [retval][out] */ BOOL *enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setTextAreasAreResizable( 
        /* [in] */ BOOL enabled);

    virtual HRESULT STDMETHODCALLTYPE usesPageCache( 
        /* [retval][out] */ BOOL *usesPageCache);

    virtual HRESULT STDMETHODCALLTYPE setUsesPageCache( 
        /* [in] */ BOOL usesPageCache);

    virtual HRESULT STDMETHODCALLTYPE unused1();
    virtual HRESULT STDMETHODCALLTYPE unused2();

    virtual HRESULT STDMETHODCALLTYPE iconDatabaseLocation(
        /* [retval][out] */ BSTR* location);

    virtual HRESULT STDMETHODCALLTYPE setIconDatabaseLocation(
        /* [in] */ BSTR location);

    virtual HRESULT STDMETHODCALLTYPE iconDatabaseEnabled(
        /* [retval][out] */ BOOL* enabled);

    virtual HRESULT STDMETHODCALLTYPE setIconDatabaseEnabled(
        /* [in] */ BOOL enabled);

    virtual HRESULT STDMETHODCALLTYPE fontSmoothing( 
        /* [retval][out] */ FontSmoothingType* smoothingType);
    
    virtual HRESULT STDMETHODCALLTYPE setFontSmoothing( 
        /* [in] */ FontSmoothingType smoothingType);

    virtual HRESULT STDMETHODCALLTYPE editableLinkBehavior( 
        /* [retval][out] */ WebKitEditableLinkBehavior* behavior);
    
    virtual HRESULT STDMETHODCALLTYPE setEditableLinkBehavior( 
        /* [in] */ WebKitEditableLinkBehavior behavior);

    virtual HRESULT STDMETHODCALLTYPE cookieStorageAcceptPolicy( 
        /* [retval][out] */ WebKitCookieStorageAcceptPolicy *acceptPolicy);
        
    virtual HRESULT STDMETHODCALLTYPE setCookieStorageAcceptPolicy( 
        /* [in] */ WebKitCookieStorageAcceptPolicy acceptPolicy);

    virtual HRESULT STDMETHODCALLTYPE continuousSpellCheckingEnabled( 
        /* [retval][out] */ BOOL *enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setContinuousSpellCheckingEnabled( 
        /* [in] */ BOOL enabled);
    
    virtual HRESULT STDMETHODCALLTYPE grammarCheckingEnabled( 
        /* [retval][out] */ BOOL *enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setGrammarCheckingEnabled( 
        /* [in] */ BOOL enabled);

    virtual HRESULT STDMETHODCALLTYPE allowContinuousSpellChecking( 
        /* [retval][out] */ BOOL *enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setAllowContinuousSpellChecking( 
        /* [in] */ BOOL enabled);

    virtual HRESULT STDMETHODCALLTYPE isDOMPasteAllowed( 
        /* [retval][out] */ BOOL *enabled);
    
    virtual HRESULT STDMETHODCALLTYPE setDOMPasteAllowed( 
        /* [in] */ BOOL enabled);

     virtual HRESULT STDMETHODCALLTYPE cacheModel(
         /* [retval][out] */ WebCacheModel* cacheModel);

     virtual HRESULT STDMETHODCALLTYPE setCacheModel(
         /* [in] */ WebCacheModel cacheModel);

    // IWebPreferencesPrivate
    virtual HRESULT STDMETHODCALLTYPE setDeveloperExtrasEnabled(
        /* [in] */ BOOL);

    virtual HRESULT STDMETHODCALLTYPE developerExtrasEnabled(
        /* [retval][out] */ BOOL*);

     virtual HRESULT STDMETHODCALLTYPE setAutomaticallyDetectsCacheModel(
         /* [in] */ BOOL automaticallyDetectsCacheModel);

     virtual HRESULT STDMETHODCALLTYPE automaticallyDetectsCacheModel(
         /* [out, retval] */ BOOL* automaticallyDetectsCacheModel);

    // WebPreferences

    // This method accesses a different preference key than developerExtrasEnabled.
    // See <rdar://5343767> for the justification.
    bool developerExtrasDisabledByOverride();

    static BSTR webPreferencesChangedNotification();
    static BSTR webPreferencesRemovedNotification();

    static void setInstance(WebPreferences* instance, BSTR identifier);
    static void removeReferenceForIdentifier(BSTR identifier);
    static WebPreferences* sharedStandardPreferences();

    // From WebHistory.h
    HRESULT historyItemLimit(int* limit);
    HRESULT setHistoryItemLimit(int limit);
    HRESULT historyAgeInDaysLimit(int* limit);
    HRESULT setHistoryAgeInDaysLimit(int limit);

     void willAddToWebView();
     void didRemoveFromWebView();

    HRESULT postPreferencesChangesNotification();

protected:
    const void* valueForKey(CFStringRef key);
    BSTR stringValueForKey(CFStringRef key);
    int integerValueForKey(CFStringRef key);
    BOOL boolValueForKey(CFStringRef key);
    float floatValueForKey(CFStringRef key);
    void setStringValue(CFStringRef key, LPCTSTR value);
    void setIntegerValue(CFStringRef key, int value);
    void setBoolValue(CFStringRef key, BOOL value);
    static WebPreferences* getInstanceForIdentifier(BSTR identifier);
    static void initializeDefaultSettings();
    void save();
    void load();
    void migrateDefaultSettingsFromSafari3Beta();
    void removeValuesMatchingDefaultSettings();

protected:
    ULONG m_refCount;
    RetainPtr<CFMutableDictionaryRef> m_privatePrefs;
    WebCore::BString m_identifier;
    bool m_autoSaves;
    bool m_automaticallyDetectsCacheModel;
    unsigned m_numWebViews;

    static CFDictionaryRef s_defaultSettings;
    static WebPreferences* s_standardPreferences;
};

#endif