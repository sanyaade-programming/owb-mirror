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
#include "Font.h"

#include "GraphicsContext.h"
#include "BALBase.h"
#include "SimpleFontData.h"
#include "SDL.h"
#include "fonts/pixelfont.h"

namespace WKAL {

#define IS_HIGH_SURROGATE(u)  ((UChar)(u) >= (UChar)0xd800 && (UChar)(u) <= (UChar)0xdbff)
#define IS_LOW_SURROGATE(u)  ((UChar)(u) >= (UChar)0xdc00 && (UChar)(u) <= (UChar)0xdfff)



void Font::drawComplexText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    printf("Font::drawComplexText\n");
    BalNotImplemented();
}


float Font::floatWidthForComplexText(const TextRun& run) const
{
    printf("Font::floatWidthForComplexText\n");
    BalNotImplemented();
    return 0.0;
}

int Font::offsetForPositionForComplexText(const TextRun& run, int x, bool includePartialGlyphs) const
{
    printf("Font::offsetForPositionForComplexText\n");
    BalNotImplemented();
    return 0;
}

FloatRect Font::selectionRectForComplexText(const TextRun& run, const IntPoint& point, int h, int from, int to) const
{
    printf("Font::selectionRectForComplexText\n");
    BalNotImplemented();
    return FloatRect();
}

void Font::drawGlyphs(GraphicsContext* context, const SimpleFontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
    SDL_Surface *surface = context->platformContext();
    // Set the text color to use for drawing.
    Color penColor = context->fillColor();
    unsigned pixelColor = (penColor.alpha() << 24) | (penColor.red() << 16) | (penColor.green() << 8) | penColor.blue();
    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
    PixelFontType *fontPixel = font->m_font.m_pixelFont;
    uint8_t w = fontPixel->width;
    uint8_t h = fontPixel->height;
    IntRect dstRect(0, 0, w * numGlyphs , h);

    Vector<unsigned> *glyphRGBABuffer = new Vector<unsigned>(h * w * numGlyphs);
    IntPoint aPoint(static_cast<uint16_t> (point.x()), static_cast<uint16_t> (point.y() - h));
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < numGlyphs; i++) {
            for (int k = 0; k < w; k++) {
                uint16_t glyphOffset = glyphs[i] * h + j;
                if (fontPixel->buffer[glyphOffset] & (1 << (w - k)))
                    (*glyphRGBABuffer)[(j * numGlyphs * w) + k + (i * w)] = pixelColor;
                else
                    (*glyphRGBABuffer)[(j * numGlyphs * w) + k + (i * w)] = 0;
            }
        }
    }

    SDL_Surface* img;
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
    on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
#endif
        
    img = SDL_CreateRGBSurfaceFrom((void*)glyphRGBABuffer->data(), w * numGlyphs, h,
                                   32, w * numGlyphs * 4, rmask, gmask, bmask, amask);
    SDL_Rect sdlSrc, sdlDest;
    sdlDest.x = aPoint.x() + context->origin().width();
    sdlDest.y = aPoint.y() + context->origin().height();
    sdlDest.w = dstRect.width();
    sdlDest.h = dstRect.height();
    sdlSrc.w = dstRect.width();
    sdlSrc.h = dstRect.height();
    sdlSrc.x = dstRect.x();
    sdlSrc.y = dstRect.y();

    SDL_BlitSurface(img, &sdlSrc, surface, &sdlDest);

    /* use SDL_Flip only if double buffering is available */
    /*if (surface->flags & SDL_DOUBLEBUF) {
       SDL_Flip(surface);
    }
    else {
       SDL_UpdateRect(surface, sdlDest.x + origin().width(), sdlDest.y + origin().height(), sdlDest.w, sdlDest.h);
    }*/
    SDL_FreeSurface(img);
}

}
