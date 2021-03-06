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

#include "config.h"
#include "WebPreferences.h"
#include "WebPreferenceKeysPrivate.h"

#include <CString.h>
#include <FileSystem.h>
#include <Font.h>
#include <PlatformString.h>
#include <StringHash.h>
#include "ObserverServiceData.h"

#include <wtf/HashMap.h>
#include <wtf/OwnArrayPtr.h>
#include DEEPSEE_INCLUDE

using namespace WebCore;

static HashMap<WebCore::String, WebCore::String> m_privatePrefs;
static HashMap<WebCore::String, WebPreferences*> webPreferencesInstances;

WebPreferences* WebPreferences::sharedStandardPreferences()
{
    static WebPreferences* standardPreferences;
    if (!standardPreferences) {
        standardPreferences = WebPreferences::createInstance();
        standardPreferences->setAutosaves(true);
        standardPreferences->load();
    }

    return standardPreferences;
}

WebPreferences::WebPreferences()
    : m_autoSaves(0)
    , m_automaticallyDetectsCacheModel(true)
    , m_numWebViews(0)
{
    DS_CONSTRUCT();
}

WebPreferences::~WebPreferences()
{
    DS_DESTRUCT();
}

WebPreferences* WebPreferences::createInstance()
{
    WebPreferences* instance = new WebPreferences();
    return instance;
}

void WebPreferences::postPreferencesChangesNotification()
{
    OWBAL::ObserverServiceData::createObserverService()->notifyObserver(webPreferencesChangedNotification(), "", this);
}

WebPreferences* WebPreferences::getInstanceForIdentifier(String identifier)
{
    if (!identifier)
        return sharedStandardPreferences();

    return webPreferencesInstances.get(identifier);
}

void WebPreferences::setInstance(WebPreferences* instance, String identifier)
{
    if (!identifier || !instance)
        return;
    webPreferencesInstances.add(identifier, instance);
}

void WebPreferences::removeReferenceForIdentifier(String identifier)
{
    if (!identifier || webPreferencesInstances.isEmpty())
        return;

    WebPreferences* webPreference = webPreferencesInstances.get(identifier);
    if (webPreference)
        webPreferencesInstances.remove(identifier);
}

void WebPreferences::initializeDefaultSettings()
{
    m_privatePrefs.add(WebKitStandardFontPreferenceKey, "Times New Roman");
    m_privatePrefs.add(WebKitFixedFontPreferenceKey, "Courier New");
    m_privatePrefs.add(WebKitSerifFontPreferenceKey, "Times New Roman");
    m_privatePrefs.add(WebKitSansSerifFontPreferenceKey, "Arial");
    m_privatePrefs.add(WebKitCursiveFontPreferenceKey, "Comic Sans MS");
    m_privatePrefs.add(WebKitFantasyFontPreferenceKey, "Comic Sans MS");
    m_privatePrefs.add(WebKitMinimumFontSizePreferenceKey, "1");
    m_privatePrefs.add(WebKitMinimumLogicalFontSizePreferenceKey, "9");
    m_privatePrefs.add(WebKitDefaultFontSizePreferenceKey, "16");
    m_privatePrefs.add(WebKitDefaultFixedFontSizePreferenceKey, "13");
    m_privatePrefs.add(WebKitDefaultTextEncodingNamePreferenceKey, "ISO-8859-1");

    m_privatePrefs.add(WebKitUserStyleSheetEnabledPreferenceKey, "0"); //FALSE
    m_privatePrefs.add(WebKitUserStyleSheetLocationPreferenceKey, "");
    m_privatePrefs.add(WebKitShouldPrintBackgroundsPreferenceKey, "0"); //FALSE
    m_privatePrefs.add(WebKitTextAreasAreResizablePreferenceKey, "0"); //FALSE
    m_privatePrefs.add(WebKitJavaEnabledPreferenceKey, "1"); //TRUE
    m_privatePrefs.add(WebKitJavaScriptEnabledPreferenceKey, "1"); //TRUE
    m_privatePrefs.add(WebKitJavaScriptCanOpenWindowsAutomaticallyPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitPluginsEnabledPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitAllowAnimatedImagesPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitAllowAnimatedImageLoopingPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitDisplayImagesKey, "1");//TRUE
    m_privatePrefs.add(WebKitBackForwardCacheExpirationIntervalKey, "1800");
    m_privatePrefs.add(WebKitTabToLinksPreferenceKey, "1"); //TRUE
    m_privatePrefs.add(WebKitPrivateBrowsingEnabledPreferenceKey, "0"); //FALSE
    m_privatePrefs.add(WebKitRespectStandardStyleKeyEquivalentsPreferenceKey, "0");//FALSE
    m_privatePrefs.add(WebKitShowsURLsInToolTipsPreferenceKey, "0");//FALSE
    m_privatePrefs.add(WebKitPDFDisplayModePreferenceKey, "1");
    m_privatePrefs.add(WebKitPDFScaleFactorPreferenceKey, "0");

    m_privatePrefs.add(WebKitEditableLinkBehaviorPreferenceKey, String::number(WebKitEditableLinkDefaultBehavior));

    m_privatePrefs.add(WebKitHistoryItemLimitKey, "1000");
    m_privatePrefs.add(WebKitHistoryAgeInDaysLimitKey, "7");
    m_privatePrefs.add(WebKitIconDatabaseLocationKey, "");
    m_privatePrefs.add(WebKitIconDatabaseEnabledPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitFontSmoothingTypePreferenceKey, "2");
    m_privatePrefs.add(WebKitCookieStorageAcceptPolicyPreferenceKey, "2");
    m_privatePrefs.add(WebContinuousSpellCheckingEnabledPreferenceKey, "0");//FALSE
    m_privatePrefs.add(WebGrammarCheckingEnabledPreferenceKey, "0");//FALSE
    m_privatePrefs.add(AllowContinuousSpellCheckingPreferenceKey, "1");//TRUE
    m_privatePrefs.add(WebKitUsesPageCachePreferenceKey, "1");//TRUE

    m_privatePrefs.add(WebKitCacheModelPreferenceKey, String::number(WebCacheModelDocumentViewer));

    m_privatePrefs.add(WebKitAuthorAndUserStylesEnabledPreferenceKey, "1");//TRUE
}

String WebPreferences::valueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return value;
}

void WebPreferences::setValueForKey(String key, String value)
{
    m_privatePrefs.set(key, value);
}

String WebPreferences::stringValueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return value;
}

int WebPreferences::integerValueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return value.toInt();
}

bool WebPreferences::boolValueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return (bool)value.toInt();
}

float WebPreferences::floatValueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return value.toFloat();
}

unsigned int WebPreferences::longlongValueForKey(String key)
{
    String value = m_privatePrefs.get(key);
    return value.toUInt();
}

void WebPreferences::setStringValue(String key, String value)
{
    m_privatePrefs.set(key, value);

    postPreferencesChangesNotification();
}

void WebPreferences::setIntegerValue(String key, int value)
{
    m_privatePrefs.set(key, String::number(value));

    postPreferencesChangesNotification();
}

void WebPreferences::setBoolValue(String key, bool value)
{
    m_privatePrefs.set(key, String::number(value));

    postPreferencesChangesNotification();
}

void WebPreferences::setLongLongValue(String key, unsigned int value)
{
    m_privatePrefs.set(key, String::number(value));

    postPreferencesChangesNotification();
}

String WebPreferences::webPreferencesChangedNotification()
{
    return "webPreferencesChangedNotification";
}

String WebPreferences::webPreferencesRemovedNotification()
{
    return "webPreferencesRemovedNotification";
}

void WebPreferences::save()
{
}

void WebPreferences::load()
{
    initializeDefaultSettings();
}

WebPreferences* WebPreferences::standardPreferences()
{
    return sharedStandardPreferences();
}

WebPreferences* WebPreferences::initWithIdentifier(String anIdentifier)
{
    WebPreferences *instance = getInstanceForIdentifier(anIdentifier);
    if (instance) {
        return instance;
    }

    load();


    m_identifier = anIdentifier;
    setInstance(this, m_identifier);

    this->postPreferencesChangesNotification();

    return this;
}

String WebPreferences::identifier()
{
    return m_identifier;
}

String WebPreferences::standardFontFamily()
{
    return stringValueForKey(WebKitStandardFontPreferenceKey);
}

void WebPreferences::setStandardFontFamily(String family)
{
    setStringValue(WebKitStandardFontPreferenceKey, family);
}

String WebPreferences::fixedFontFamily()
{
    return stringValueForKey(WebKitFixedFontPreferenceKey);
}

void WebPreferences::setFixedFontFamily(String family)
{
    setStringValue(WebKitFixedFontPreferenceKey, family);
}

String WebPreferences::serifFontFamily()
{
    return stringValueForKey(WebKitSerifFontPreferenceKey);
}

void WebPreferences::setSerifFontFamily(String family)
{
    setStringValue(WebKitSerifFontPreferenceKey, family);
}

String WebPreferences::sansSerifFontFamily()
{
    return stringValueForKey(WebKitSansSerifFontPreferenceKey);
}

void WebPreferences::setSansSerifFontFamily(String family)
{
    setStringValue(WebKitSansSerifFontPreferenceKey, family);
}

String WebPreferences::cursiveFontFamily()
{
    return stringValueForKey(WebKitCursiveFontPreferenceKey);
}

void WebPreferences::setCursiveFontFamily(String family)
{
    setStringValue(WebKitCursiveFontPreferenceKey, family);
}

String WebPreferences::fantasyFontFamily()
{
    return stringValueForKey(WebKitFantasyFontPreferenceKey);
}

void WebPreferences::setFantasyFontFamily(String family)
{
    setStringValue(WebKitFantasyFontPreferenceKey, family);
}

int WebPreferences::defaultFontSize()
{
    return integerValueForKey(WebKitDefaultFontSizePreferenceKey);
}

void WebPreferences::setDefaultFontSize(int fontSize)
{
    setIntegerValue(WebKitDefaultFontSizePreferenceKey, fontSize);
}

int WebPreferences::defaultFixedFontSize()
{
    return integerValueForKey(WebKitDefaultFixedFontSizePreferenceKey);
}

void WebPreferences::setDefaultFixedFontSize(int fontSize)
{
    setIntegerValue(WebKitDefaultFixedFontSizePreferenceKey, fontSize);
}

int WebPreferences::minimumFontSize()
{
    return integerValueForKey(WebKitMinimumFontSizePreferenceKey);
}

void WebPreferences::setMinimumFontSize(int fontSize)
{
    setIntegerValue(WebKitMinimumFontSizePreferenceKey, fontSize);
}

int WebPreferences::minimumLogicalFontSize()
{
    return integerValueForKey(WebKitMinimumLogicalFontSizePreferenceKey);
}

void WebPreferences::setMinimumLogicalFontSize(int fontSize)
{
    setIntegerValue(WebKitMinimumLogicalFontSizePreferenceKey, fontSize);
}

String WebPreferences::defaultTextEncodingName()
{
    return stringValueForKey(WebKitDefaultTextEncodingNamePreferenceKey);
}

void WebPreferences::setDefaultTextEncodingName(String name)
{
    setStringValue(WebKitDefaultTextEncodingNamePreferenceKey, name);
}

bool WebPreferences::userStyleSheetEnabled()
{
    return boolValueForKey(WebKitUserStyleSheetEnabledPreferenceKey);
}

void WebPreferences::setUserStyleSheetEnabled(bool enabled)
{
    setBoolValue(WebKitUserStyleSheetEnabledPreferenceKey, enabled);
}

String WebPreferences::userStyleSheetLocation()
{
    return stringValueForKey(WebKitUserStyleSheetLocationPreferenceKey);
}

void WebPreferences::setUserStyleSheetLocation(String location)
{
    setStringValue(WebKitUserStyleSheetLocationPreferenceKey, location);
}

bool WebPreferences::isJavaEnabled()
{
    return boolValueForKey(WebKitJavaEnabledPreferenceKey);
}

void WebPreferences::setJavaEnabled(bool enabled)
{
    setBoolValue(WebKitJavaEnabledPreferenceKey, enabled);
}

bool WebPreferences::isJavaScriptEnabled()
{
    return boolValueForKey(WebKitJavaScriptEnabledPreferenceKey);
}

void WebPreferences::setJavaScriptEnabled(bool enabled)
{
    setBoolValue(WebKitJavaScriptEnabledPreferenceKey, enabled);
}

bool WebPreferences::javaScriptCanOpenWindowsAutomatically()
{
    return boolValueForKey(WebKitJavaScriptCanOpenWindowsAutomaticallyPreferenceKey);
}

void WebPreferences::setJavaScriptCanOpenWindowsAutomatically(bool enabled)
{
    setBoolValue(WebKitJavaScriptCanOpenWindowsAutomaticallyPreferenceKey, enabled);
}

bool WebPreferences::arePlugInsEnabled()
{
    return boolValueForKey(WebKitPluginsEnabledPreferenceKey);
}

void WebPreferences::setPlugInsEnabled(bool enabled)
{
    setBoolValue(WebKitPluginsEnabledPreferenceKey, enabled);
}

bool WebPreferences::allowsAnimatedImages()
{
    return boolValueForKey(WebKitAllowAnimatedImagesPreferenceKey);
}

void WebPreferences::setAllowsAnimatedImages(bool enabled)
{
    setBoolValue(WebKitAllowAnimatedImagesPreferenceKey, enabled);
}

bool WebPreferences::allowAnimatedImageLooping()
{
    return boolValueForKey(WebKitAllowAnimatedImageLoopingPreferenceKey);
}

void WebPreferences::setAllowAnimatedImageLooping(bool enabled)
{
    setBoolValue(WebKitAllowAnimatedImageLoopingPreferenceKey, enabled);
}

void WebPreferences::setLoadsImagesAutomatically(bool enabled)
{
    setBoolValue(WebKitDisplayImagesKey, enabled);
}

bool WebPreferences::loadsImagesAutomatically()
{
    return boolValueForKey(WebKitDisplayImagesKey);
}

void WebPreferences::setAutosaves(bool enabled)
{
    m_autoSaves = !!enabled;
}

bool WebPreferences::autosaves()
{
    return m_autoSaves ? true : false;
}

void WebPreferences::setShouldPrintBackgrounds(bool enabled)
{
    setBoolValue(WebKitShouldPrintBackgroundsPreferenceKey, enabled);
}

bool WebPreferences::shouldPrintBackgrounds()
{
    return boolValueForKey(WebKitShouldPrintBackgroundsPreferenceKey);
}

void WebPreferences::setPrivateBrowsingEnabled(bool enabled)
{
    setBoolValue(WebKitPrivateBrowsingEnabledPreferenceKey, enabled);
}

bool WebPreferences::privateBrowsingEnabled()
{
    return boolValueForKey(WebKitPrivateBrowsingEnabledPreferenceKey);
}

void WebPreferences::setTabsToLinks(bool enabled)
{
    setBoolValue(WebKitTabToLinksPreferenceKey, enabled);
}

bool WebPreferences::tabsToLinks()
{
    return boolValueForKey(WebKitTabToLinksPreferenceKey);
}

void WebPreferences::setUsesPageCache(bool usesPageCache)
{
    setBoolValue(WebKitUsesPageCachePreferenceKey, usesPageCache);
}

bool WebPreferences::usesPageCache()
{
    return boolValueForKey(WebKitUsesPageCachePreferenceKey);
}

bool WebPreferences::textAreasAreResizable()
{
    return boolValueForKey(WebKitTextAreasAreResizablePreferenceKey);
}

void WebPreferences::setTextAreasAreResizable(bool enabled)
{
    setBoolValue(WebKitTextAreasAreResizablePreferenceKey, enabled);
}

int WebPreferences::historyItemLimit()
{
    return integerValueForKey(WebKitHistoryItemLimitKey);
}

void WebPreferences::setHistoryItemLimit(int limit)
{
    setIntegerValue(WebKitHistoryItemLimitKey, limit);
}

int WebPreferences::historyAgeInDaysLimit()
{
    return integerValueForKey(WebKitHistoryAgeInDaysLimitKey);
}

void WebPreferences::setHistoryAgeInDaysLimit(int limit)
{
    setIntegerValue(WebKitHistoryAgeInDaysLimitKey, limit);
}

String WebPreferences::iconDatabaseLocation()
{
    return stringValueForKey(WebKitIconDatabaseLocationKey);
}

void WebPreferences::setIconDatabaseLocation(String location)
{
    setStringValue(WebKitIconDatabaseLocationKey, location);
}

bool WebPreferences::iconDatabaseEnabled()
{
    return boolValueForKey(WebKitIconDatabaseEnabledPreferenceKey);
}

void WebPreferences::setIconDatabaseEnabled(bool enabled)
{
    setBoolValue(WebKitIconDatabaseEnabledPreferenceKey, enabled);
}

FontSmoothingType WebPreferences::fontSmoothing()
{
    return (FontSmoothingType) integerValueForKey(WebKitFontSmoothingTypePreferenceKey);
}

void WebPreferences::setFontSmoothing(FontSmoothingType smoothingType)
{
    setIntegerValue(WebKitFontSmoothingTypePreferenceKey, smoothingType);
}

WebKitEditableLinkBehavior WebPreferences::editableLinkBehavior()
{
    WebKitEditableLinkBehavior value = (WebKitEditableLinkBehavior) integerValueForKey(WebKitEditableLinkBehaviorPreferenceKey);
    switch (value) {
        case WebKitEditableLinkDefaultBehavior:
        case WebKitEditableLinkAlwaysLive:
        case WebKitEditableLinkOnlyLiveWithShiftKey:
        case WebKitEditableLinkLiveWhenNotFocused:
        case WebKitEditableLinkNeverLive:
            return value;
        default: // ensure that a valid result is returned
            return WebKitEditableLinkDefaultBehavior;
    }
}

void WebPreferences::setEditableLinkBehavior(WebKitEditableLinkBehavior behavior)
{
    setIntegerValue(WebKitEditableLinkBehaviorPreferenceKey, behavior);
}

WebKitCookieStorageAcceptPolicy WebPreferences::cookieStorageAcceptPolicy()
{
    return (WebKitCookieStorageAcceptPolicy)integerValueForKey(WebKitCookieStorageAcceptPolicyPreferenceKey);
}

void WebPreferences::setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicy acceptPolicy)
{
    setIntegerValue(WebKitCookieStorageAcceptPolicyPreferenceKey, acceptPolicy);
}


bool WebPreferences::continuousSpellCheckingEnabled()
{
    return boolValueForKey(WebContinuousSpellCheckingEnabledPreferenceKey);
}

void WebPreferences::setContinuousSpellCheckingEnabled(bool enabled)
{
    setBoolValue(WebContinuousSpellCheckingEnabledPreferenceKey, enabled);
}

bool WebPreferences::grammarCheckingEnabled()
{
    return boolValueForKey(WebGrammarCheckingEnabledPreferenceKey);
}

void WebPreferences::setGrammarCheckingEnabled(bool enabled)
{
    setBoolValue(WebGrammarCheckingEnabledPreferenceKey, enabled);
}

bool WebPreferences::allowContinuousSpellChecking()
{
    return boolValueForKey(AllowContinuousSpellCheckingPreferenceKey);
}

void WebPreferences::setAllowContinuousSpellChecking(bool enabled)
{
    setBoolValue(AllowContinuousSpellCheckingPreferenceKey, enabled);
}

bool WebPreferences::isDOMPasteAllowed()
{
    return boolValueForKey(WebKitDOMPasteAllowedPreferenceKey);
}

void WebPreferences::setDOMPasteAllowed(bool enabled)
{
    setBoolValue(WebKitDOMPasteAllowedPreferenceKey, enabled);
}

WebCacheModel WebPreferences::cacheModel()
{
    return (WebCacheModel)integerValueForKey(WebKitCacheModelPreferenceKey);
}

void WebPreferences::setCacheModel(WebCacheModel cacheModel)
{
    setIntegerValue(WebKitCacheModelPreferenceKey, cacheModel);
}

void WebPreferences::setShouldPaintCustomScrollbars(bool shouldPaint)
{
    setBoolValue(WebKitPaintCustomScrollbarsPreferenceKey, shouldPaint);
}

bool WebPreferences::shouldPaintCustomScrollbars()
{
    return boolValueForKey(WebKitPaintCustomScrollbarsPreferenceKey);
}

void WebPreferences::setDeveloperExtrasEnabled(bool enabled)
{
    setBoolValue(WebKitDeveloperExtrasEnabledPreferenceKey, enabled);
}

bool WebPreferences::developerExtrasEnabled()
{
    return boolValueForKey(WebKitDeveloperExtrasEnabledPreferenceKey);
}

bool WebPreferences::developerExtrasDisabledByOverride()
{
    return !!boolValueForKey(DisableWebKitDeveloperExtrasPreferenceKey);
}

void WebPreferences::setAutomaticallyDetectsCacheModel(bool automaticallyDetectsCacheModel)
{
    m_automaticallyDetectsCacheModel = !!automaticallyDetectsCacheModel;
}

bool WebPreferences::automaticallyDetectsCacheModel()
{
    return m_automaticallyDetectsCacheModel;
}

void WebPreferences::setAuthorAndUserStylesEnabled(bool enabled)
{
    setBoolValue(WebKitAuthorAndUserStylesEnabledPreferenceKey, enabled);
}

bool WebPreferences::authorAndUserStylesEnabled()
{
    return boolValueForKey(WebKitAuthorAndUserStylesEnabledPreferenceKey);
}

void WebPreferences::setZoomsTextOnly(bool zoomsTextOnly)
{
    setBoolValue(WebKitZoomsTextOnlyPreferenceKey, zoomsTextOnly);
}

bool WebPreferences::zoomsTextOnly()
{
    return boolValueForKey(WebKitZoomsTextOnlyPreferenceKey);
}

void WebPreferences::willAddToWebView()
{
    ++m_numWebViews;
}

void WebPreferences::didRemoveFromWebView()
{
    ASSERT(m_numWebViews);
    if (--m_numWebViews == 0)
        OWBAL::ObserverServiceData::createObserverService()->notifyObserver(webPreferencesRemovedNotification(), "", this);
}
