/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
/** 
 * @file pixelfont.h
 *
 * Bitmap fonts definition for SDL
 */
#ifndef PIXELFONT_H
#define PIXELFONT_H



#define NORMAL_SIZE_LOW_LIMIT 12
#define NORMAL_SIZE_HIGH_LIMIT 24

#include "Font5x7.h"
#include "Font5x8.h"
#include "Font6x10.h"
#include "Font7x13.h"
//#include "Font9x15.h"
#include "Font9x18.h"
#include "Font10x20.h"

struct PixelFont {
  int width;
  int height;
  const char* normal;
  const char* bold;
  const char* italic;
};

typedef struct PixelFont PixelFontType;



PixelFontType NormalPixelFont = {
  7,
  13,
  font7x13_normal,
  font7x13_bold,
  font7x13_italic,

};

PixelFontType SmallPixelFont = {
  5,
  7,
  font5x7_normal,
  font5x7_bold,
  font5x7_italic

};


PixelFontType BigPixelFont = {
  10,
  20,
  font10x20_normal,
  font10x20_bold,
  font10x20_italic
};


#endif
