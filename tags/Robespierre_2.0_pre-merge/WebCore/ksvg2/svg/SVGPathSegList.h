/*
    Copyright (C) 2007 Eric Seidel <eric@webkit.org>

    This file is part of the WebKit project

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

#ifndef SVGPathSegList_h
#define SVGPathSegList_h

#ifdef SVG_SUPPORT

#include "SVGList.h"
#include "SVGPathSeg.h"

namespace WebCore
{
    class Path;
    class SVGElement;
 
    class SVGPathSegList : public SVGList<RefPtr<SVGPathSeg> >
    {
    public:
        SVGPathSegList(const SVGElement* context);
        virtual ~SVGPathSegList();

        const SVGElement* context() const;
        
        unsigned getPathSegAtLength(double);
        Path toPathData();

    private:
        const SVGElement* m_context;
    };

} // namespace WebCore

#endif // SVG_SUPPORT
#endif

// vim:ts=4:noet
