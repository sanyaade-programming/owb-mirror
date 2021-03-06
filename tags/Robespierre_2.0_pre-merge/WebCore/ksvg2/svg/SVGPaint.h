/*
    Copyright (C) 2004, 2005 Nikolas Zimmermann <wildfox@kde.org>
                  2004, 2005, 2006 Rob Buis <buis@kde.org>
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmial.com)

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef SVGPaint_h
#define SVGPaint_h
#ifdef SVG_SUPPORT

#include "SVGColor.h"

namespace WebCore {

    class SVGPaint : public SVGColor {
    public:
        enum SVGPaintType {
            SVG_PAINTTYPE_UNKNOWN               = 0,
            SVG_PAINTTYPE_RGBCOLOR              = 1,
            SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR     = 2,
            SVG_PAINTTYPE_NONE                  = 101,
            SVG_PAINTTYPE_CURRENTCOLOR          = 102,
            SVG_PAINTTYPE_URI_NONE              = 103,
            SVG_PAINTTYPE_URI_CURRENTCOLOR      = 104,
            SVG_PAINTTYPE_URI_RGBCOLOR          = 105,
            SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR = 106,
            SVG_PAINTTYPE_URI                   = 107
        };

        SVGPaint();
        SVGPaint(const String& uri);
        SVGPaint(SVGPaintType);
        SVGPaint(SVGPaintType, const String& uri, const String& rgbPaint = String(), const String& iccPaint = String());
        SVGPaint(const Color& c);
        virtual ~SVGPaint();

        // 'SVGPaint' functions
        SVGPaintType paintType() const { return m_paintType; }
        String uri() const;

        void setUri(const String&);
        void setPaint(SVGPaintType, const String& uri, const String& rgbPaint, const String& iccPaint, ExceptionCode&);

        virtual String cssText() const;
        
        static SVGPaint* defaultFill();
        static SVGPaint* defaultStroke();

        virtual bool isSVGPaint() const { return true; }
    private:
        SVGPaintType m_paintType;
        String m_uri;
    };

} // namespace WebCore

#endif // SVG_SUPPORT
#endif // SVGPaint_h

// vim:ts=4:noet
