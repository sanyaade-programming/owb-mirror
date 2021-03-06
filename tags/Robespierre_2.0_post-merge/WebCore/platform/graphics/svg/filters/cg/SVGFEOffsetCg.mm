/*
    Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>

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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "config.h"

#if ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)
#include "SVGFEOffset.h"
#include "SVGFEHelpersCg.h"

namespace WebCore {

CIFilter* SVGFEOffset::getCIFilter(SVGResourceFilter* svgFilter) const
{
    FE_QUARTZ_SETUP_INPUT(@"CIAffineTransform");
    NSAffineTransform* offsetTransform = [NSAffineTransform transform];
    [offsetTransform translateXBy:dx() yBy:dy()];
    [filter setValue:offsetTransform  forKey:@"inputTransform"];
    FE_QUARTZ_OUTPUT_RETURN;
}

}

#endif // ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)
