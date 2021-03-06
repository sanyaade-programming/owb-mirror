/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005, 2006 Rob Buis <buis@kde.org>

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

#include "config.h"

#ifdef SVG_SUPPORT
#include "SVGClipPathElement.h"

#include "Document.h"
#include "RenderView.h"
#include "SVGNames.h"
#include "SVGUnitTypes.h"
#include "cssstyleselector.h"

namespace WebCore {

SVGClipPathElement::SVGClipPathElement(const QualifiedName& tagName, Document* doc)
    : SVGStyledTransformableElement(tagName, doc)
    , SVGTests()
    , SVGLangSpace()
    , SVGExternalResourcesRequired()
    , m_clipPathUnits(SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE)
{
}

SVGClipPathElement::~SVGClipPathElement()
{
}

ANIMATED_PROPERTY_DEFINITIONS(SVGClipPathElement, int, Enumeration, enumeration, ClipPathUnits, clipPathUnits, SVGNames::clipPathUnitsAttr.localName(), m_clipPathUnits)

void SVGClipPathElement::parseMappedAttribute(MappedAttribute* attr)
{
    const String& value = attr->value();
    if (attr->name() == SVGNames::clipPathUnitsAttr) {
        if (value == "userSpaceOnUse")
            setClipPathUnitsBaseValue(SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE);
        else if (value == "objectBoundingBox")
            setClipPathUnitsBaseValue(SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
    } else {
        if (SVGTests::parseMappedAttribute(attr))
            return;
        if (SVGLangSpace::parseMappedAttribute(attr))
            return;
        if (SVGExternalResourcesRequired::parseMappedAttribute(attr))
            return;
        SVGStyledTransformableElement::parseMappedAttribute(attr);
    }
}

SVGResource* SVGClipPathElement::canvasResource()
{
    if (!view())
        return 0;

    if (!m_clipper)
        m_clipper = new SVGResourceClipper();
    else
        m_clipper->resetClipData();

    bool bbox = clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX;

    RenderStyle* clipPathStyle = styleForRenderer(parent()->renderer()); // FIXME: Manual style resolution is a hack
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        SVGElement* e = svg_dynamic_cast(n);
        if (e && e->isStyled()) {
            SVGStyledElement* styled = static_cast<SVGStyledElement*>(e);
            RenderStyle* pathStyle = document()->styleSelector()->styleForElement(styled, clipPathStyle);
            Path pathData = styled->toPathData();
            if (e->isStyledTransformable())
                pathData.transform(static_cast<SVGStyledTransformableElement*>(e)->localMatrix());
            if (!pathData.isEmpty())
                m_clipper->addClipData(pathData, pathStyle->svgStyle()->clipRule(), bbox);
            pathStyle->deref(view()->renderArena());
        }
    }
    clipPathStyle->deref(view()->renderArena());
    return m_clipper.get();
}

void SVGClipPathElement::notifyAttributeChange() const
{
    if (!m_clipper || !attached() || ownerDocument()->parsing())
        return;

    m_clipper->invalidate();
    m_clipper->repaintClients();
}

}

#endif // SVG_SUPPORT

// vim:ts=4:noet
