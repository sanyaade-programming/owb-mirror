/*
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
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

#ifndef Image_h
#define Image_h

#include "Color.h"
#ifdef __OWB__
#include "AffineTransform.h"
namespace BAL { class BIGraphicsContext; }
#define GraphicsContext BIGraphicsContext
namespace WebCore { using ::BAL::BIGraphicsContext; }
#endif
#include "GraphicsTypes.h"
#include "ImageSource.h"

#if PLATFORM(MAC)
#ifdef __OBJC__
@class NSImage;
#else
class NSImage;
#endif
#endif

#if PLATFORM(CG)
struct CGContext;
#endif

#if PLATFORM(WIN)
typedef struct HBITMAP__ *HBITMAP;
#endif

#if PLATFORM(QT)
class QPixmap;
#endif

namespace WebCore {

class AffineTransform;
class FloatPoint;
class FloatRect;
class FloatSize;
class GraphicsContext;
class IntRect;
class IntSize;
class String;

// This class gets notified when an image advances animation frames.
class ImageAnimationObserver;

class Image : Noncopyable {
    friend class GraphicsContext;
public:
    Image(ImageAnimationObserver* = 0);
    virtual ~Image();

    static Image* loadPlatformResource(const char* name);
    static bool supportsType(const String&);

    bool isNull() const;

    virtual IntSize size() const = 0;
    IntRect rect() const;
    int width() const;
    int height() const;

    virtual bool setData(bool allDataReceived);
    virtual bool setNativeData(NativeBytePtr, bool allDataReceived) { return false; }

    Vector<char>& dataBuffer() { return m_data; }

    // It may look unusual that there is no start animation call as public API.  This is because
    // we start and stop animating lazily.  Animation begins whenever someone draws the image.  It will
    // automatically pause once all observers no longer want to render the image anywhere.
    virtual void stopAnimation() {}
    virtual void resetAnimation() {}

    // Typically the CachedImage that owns us.
    ImageAnimationObserver* animationObserver() const { return m_animationObserver; }

    enum TileRule { StretchTile, RepeatTile };

#if PLATFORM(MAC)
    // Accessors for native image formats.
    virtual NSImage* getNSImage() { return 0; }
    virtual CFDataRef getTIFFRepresentation() { return 0; }
#endif

#if PLATFORM(CG)
    virtual CGImageRef getCGImageRef() { return 0; }
#endif

#if PLATFORM(QT)
    virtual QPixmap* getPixmap() const { return 0; }
#endif

#if PLATFORM(WIN)
    virtual bool getHBITMAP(HBITMAP) { return false; }
#endif

#ifdef __OWB__
    virtual NativeImagePtr nativeImageForCurrentFrame() { return 0; }
    virtual void startAnimation() { }
#endif

protected:
    static void fillWithSolidColor(GraphicsContext* ctxt, const FloatRect& dstRect, const Color& color, CompositeOperator op);

private:
    // every drawing functions should be in GraphicsContext
#ifndef __OWB__
    virtual void draw(GraphicsContext*, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator) = 0;
    void drawTiled(GraphicsContext*, const FloatRect& dstRect, const FloatPoint& srcPoint, const FloatSize& tileSize, CompositeOperator);
    void drawTiled(GraphicsContext*, const FloatRect& dstRect, const FloatRect& srcRect, TileRule hRule, TileRule vRule, CompositeOperator);
#endif
    // Supporting tiled drawing
    virtual bool mayFillWithSolidColor() const { return false; }
    virtual Color solidColor() const { return Color(); }
#ifndef __OWB__
    virtual NativeImagePtr nativeImageForCurrentFrame() { return 0; }
    virtual void startAnimation() { }
#endif

    virtual void drawPattern(GraphicsContext*, const FloatRect& srcRect, const AffineTransform& patternTransform,
                             const FloatPoint& phase, CompositeOperator, const FloatRect& destRect);
#if PLATFORM(CG)
    // These are private to CG.  Ideally they would be only in the .cpp file, but the callback requires access
    // to the private function nativeImageForCurrentFrame()
    static void drawPatternCallback(void* info, CGContext*);
#endif

    Vector<char> m_data; // The encoded raw data for the image.
    ImageAnimationObserver* m_animationObserver;
};

}

#endif
