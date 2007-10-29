/*
 * Copyright (C) 2003, 2004, 2005 Apple Computer, Inc.  All rights reserved.
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

#import <Foundation/Foundation.h>

@class WebPreferencesPrivate;

extern NSString *WebPreferencesChangedNotification;

/*!
    @class WebPreferences
*/
@interface WebPreferences: NSObject <NSCoding>
{
@private
    WebPreferencesPrivate *_private;
}

/*!
    @method standardPreferences
*/
+ (WebPreferences *)standardPreferences;

/*!
    @method initWithIdentifier:
    @param anIdentifier A string used to identify the WebPreferences.
    @discussion WebViews can share instances of WebPreferences by using an instance of WebPreferences with
    the same identifier.  Typically, instance are not created directly.  Instead you set the preferences
    identifier on a WebView.  The identifier is used as a prefix that is added to the user defaults keys
    for the WebPreferences.
    @result Returns a new instance of WebPreferences or a previously allocated instance with the same identifier.
*/
- (id)initWithIdentifier:(NSString *)anIdentifier;

/*!
    @method identifier
    @result Returns the identifier for this WebPreferences.
*/
- (NSString *)identifier;

/*!
    @method standardFontFamily
*/
- (NSString *)standardFontFamily;

/*!
    @method setStandardFontFamily:
    @param family
*/
- (void)setStandardFontFamily:(NSString *)family;

/*!
    @method fixedFontFamily
*/
- (NSString *)fixedFontFamily;

/*!
    @method setFixedFontFamily:
    @param family
*/
- (void)setFixedFontFamily:(NSString *)family;

/*!
    @method serifFontFamily
*/
- (NSString *)serifFontFamily;

/*!
    @method setSerifFontFamily:
    @param family
*/
- (void)setSerifFontFamily:(NSString *)family;

/*!
    @method sansSerifFontFamily
*/
- (NSString *)sansSerifFontFamily;

/*!
    @method setSansSerifFontFamily:
    @param family
*/
- (void)setSansSerifFontFamily:(NSString *)family;

/*!
    @method cursiveFontFamily
*/
- (NSString *)cursiveFontFamily;

/*!
    @method setCursiveFontFamily:
    @param family
*/
- (void)setCursiveFontFamily:(NSString *)family;

/*!
    @method fantasyFontFamily
*/
- (NSString *)fantasyFontFamily;

/*!
    @method setFantasyFontFamily:
    @param family
*/
- (void)setFantasyFontFamily:(NSString *)family;

/*!
    @method defaultFontSize
*/
- (int)defaultFontSize;

/*!
    @method setDefaultFontSize:
    @param size
*/
- (void)setDefaultFontSize:(int)size;

/*!
    @method defaultFixedFontSize
*/
- (int)defaultFixedFontSize;

/*!
    @method setDefaultFixedFontSize:
    @param size
*/
- (void)setDefaultFixedFontSize:(int)size;

/*!
    @method minimumFontSize
*/
- (int)minimumFontSize;

/*!
    @method setMinimumFontSize:
    @param size
*/
- (void)setMinimumFontSize:(int)size;

/*!
    @method minimumLogicalFontSize
*/
- (int)minimumLogicalFontSize;

/*!
    @method setMinimumLogicalFontSize:
    @param size
*/
- (void)setMinimumLogicalFontSize:(int)size;

/*!
    @method defaultTextEncodingName
*/
- (NSString *)defaultTextEncodingName;

/*!
    @method setDefaultTextEncodingName:
    @param encoding
*/
- (void)setDefaultTextEncodingName:(NSString *)encoding;

/*!
    @method userStyleSheetEnabled
*/
- (BOOL)userStyleSheetEnabled;

/*!
    @method setUserStyleSheetEnabled:
    @param flag
*/
- (void)setUserStyleSheetEnabled:(BOOL)flag;

/*!
    @method userStyleSheetLocation
    @discussion The location of the user style sheet.
*/
- (NSURL *)userStyleSheetLocation;

/*!
    @method setUserStyleSheetLocation:
    @param URL The location of the user style sheet.
*/
- (void)setUserStyleSheetLocation:(NSURL *)URL;

/*!
    @method isJavaEnabled
*/
- (BOOL)isJavaEnabled;

/*!
    @method setJavaEnabled:
    @param flag
*/
- (void)setJavaEnabled:(BOOL)flag;

/*!
    @method isJavaScriptEnabled
*/
- (BOOL)isJavaScriptEnabled;

/*!
    @method setJavaScriptEnabled:
    @param flag
*/
- (void)setJavaScriptEnabled:(BOOL)flag;

/*!
    @method JavaScriptCanOpenWindowsAutomatically
*/
- (BOOL)javaScriptCanOpenWindowsAutomatically;

/*!
    @method setJavaScriptCanOpenWindowsAutomatically:
    @param flag
*/
- (void)setJavaScriptCanOpenWindowsAutomatically:(BOOL)flag;

/*!
    @method arePlugInsEnabled
*/
- (BOOL)arePlugInsEnabled;

/*!
    @method setPlugInsEnabled:
    @param flag
*/
- (void)setPlugInsEnabled:(BOOL)flag;

/*!
    @method allowAnimatedImages
*/
- (BOOL)allowsAnimatedImages;

/*!
    @method setAllowAnimatedImages:
    @param flag
*/
- (void)setAllowsAnimatedImages:(BOOL)flag;

/*!
    @method allowAnimatedImageLooping
*/
- (BOOL)allowsAnimatedImageLooping;

/*!
    @method setAllowAnimatedImageLooping:
    @param flag
*/
- (void)setAllowsAnimatedImageLooping: (BOOL)flag;

/*!
    @method setWillLoadImagesAutomatically:
    @param flag
*/
- (void)setLoadsImagesAutomatically: (BOOL)flag;

/*!
    @method willLoadImagesAutomatically
*/
- (BOOL)loadsImagesAutomatically;

/*!
    @method setAutosaves:
    @param flag 
    @discussion If autosave preferences is YES the settings represented by
    WebPreferences will be stored in the user defaults database.
*/
- (void)setAutosaves:(BOOL)flag;

/*!
    @method autosaves
    @result The value of the autosave preferences flag.
*/
- (BOOL)autosaves;

/*!
    @method setShouldPrintBackgrounds:
    @param flag
*/
- (void)setShouldPrintBackgrounds:(BOOL)flag;

/*!
    @method shouldPrintBackgrounds
    @result The value of the shouldPrintBackgrounds preferences flag
*/
- (BOOL)shouldPrintBackgrounds;

/*!
    @method setPrivateBrowsingEnabled:
    @param flag 
    @abstract If private browsing is enabled, WebKit will not store information
    about sites the user visits.
 */
- (void)setPrivateBrowsingEnabled:(BOOL)flag;

/*!
    @method privateBrowsingEnabled
 */
- (BOOL)privateBrowsingEnabled;

/*!
    @method setTabsToLinks:
    @param flag 
    @abstract If tabsToLinks is YES, the tab key will focus links and form controls. 
    The option key temporarily reverses this preference.
*/
- (void)setTabsToLinks:(BOOL)flag;

/*!
    @method tabsToLinks
*/
- (BOOL)tabsToLinks;

/*!
    @method setUsesPageCache:
    @abstract Sets whether the receiver's associated WebViews use the shared 
    page cache.
    @param UsesPageCache Whether the receiver's associated WebViews use the 
    shared page cache.
    @discussion Pages are cached as they are added to a WebBackForwardList, and
    removed from the cache as they are removed from a WebBackForwardList. Because 
    the page cache is global, caching a page in one WebBackForwardList may cause
    a page in another WebBackForwardList to be evicted from the cache.
*/
- (void)setUsesPageCache:(BOOL)usesPageCache;

/*!
    @method usesPageCache
    @abstract Returns whether the receiver should use the shared page cache.
    @result Whether the receiver should use the shared page cache.
    @discussion Pages are cached as they are added to a WebBackForwardList, and
    removed from the cache as they are removed from a WebBackForwardList. Because 
    the page cache is global, caching a page in one WebBackForwardList may cause
    a page in another WebBackForwardList to be evicted from the cache.
*/
- (BOOL)usesPageCache;

@end
