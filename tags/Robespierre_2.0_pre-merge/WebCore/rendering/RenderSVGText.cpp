/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 *               2006 Alexander Kellett <lypanov@kde.org>
 *               2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 *               2007 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"

#ifdef SVG_SUPPORT
#include "RenderSVGText.h"

#include "GraphicsContext.h"
#include "KCanvasRenderingStyle.h"
#include "PointerEventsHitRules.h"
#include "SVGLength.h"
#include "SVGLengthList.h"
#include "SVGTextElement.h"
#include "SVGRootInlineBox.h"

#include <wtf/OwnPtr.h>

namespace WebCore {

RenderSVGText::RenderSVGText(SVGTextElement* node) 
    : RenderSVGBlock(node)
{
}

IntRect RenderSVGText::getAbsoluteRepaintRect()
{
    return enclosingIntRect(absoluteTransform().mapRect(relativeBBox(true)));
}

bool RenderSVGText::requiresLayer()
{
    return false;
}

void RenderSVGText::layout()
{
    ASSERT(needsLayout());
    ASSERT(minMaxKnown());

    IntRect oldBounds;
    bool checkForRepaint = checkForRepaintDuringLayout();
    if (checkForRepaint)
        oldBounds = m_absoluteBounds;

    // FIXME: need to allow floating point positions 
    SVGTextElement* text = static_cast<SVGTextElement*>(element());
    int xOffset = (int)(text->x()->getFirst().value());
    int yOffset = (int)(text->y()->getFirst().value());
    setPos(xOffset, yOffset);

    RenderBlock::layout();

    m_absoluteBounds = getAbsoluteRepaintRect();

    bool repainted = false;
    if (checkForRepaint)
        repainted = repaintAfterLayoutIfNeeded(oldBounds);
    
    setNeedsLayout(false);
}

InlineBox* RenderSVGText::createInlineBox(bool makePlaceHolderBox, bool isRootLineBox, bool isOnlyRun)
{
    ASSERT(!isInlineFlow());
    InlineFlowBox* flowBox = new (renderArena()) SVGRootInlineBox(this);
    
    if (!m_firstLineBox)
        m_firstLineBox = m_lastLineBox = flowBox;
    else {
        m_lastLineBox->setNextLineBox(flowBox);
        flowBox->setPreviousLineBox(m_lastLineBox);
        m_lastLineBox = flowBox;
    }
    
    return flowBox;
}

bool RenderSVGText::nodeAtPoint(const HitTestRequest& request, HitTestResult& result, int _x, int _y, int _tx, int _ty, HitTestAction hitTestAction)
{
    PointerEventsHitRules hitRules(PointerEventsHitRules::SVG_TEXT_HITTESTING, style()->svgStyle()->pointerEvents());
    bool isVisible = (style()->visibility() == VISIBLE);
    if (isVisible || !hitRules.requireVisible) {
        if ((hitRules.canHitStroke && (style()->svgStyle()->hasStroke() || !hitRules.requireStroke))
            || (hitRules.canHitFill && (style()->svgStyle()->hasFill() || !hitRules.requireFill))) {
#ifdef __OWB__
	    BAL::BTAffineTransform totalTransform = absoluteTransform();
#else
            AffineTransform totalTransform = absoluteTransform();
#endif
	    double localX, localY;
            totalTransform.inverse().map(_x, _y, &localX, &localY);
            FloatPoint hitPoint(_x, _y);
            return RenderBlock::nodeAtPoint(request, result, (int)localX, (int)localY, _tx, _ty, hitTestAction);
        }
    }
    return false;
}

void RenderSVGText::absoluteRects(Vector<IntRect>& rects, int, int)
{
    rects.append(getAbsoluteRepaintRect());
}

void RenderSVGText::paint(PaintInfo& paintInfo, int tx, int ty)
{   
    RenderObject::PaintInfo pi(paintInfo);
    pi.rect = absoluteTransform().inverse().mapRect(pi.rect);
    RenderBlock::paint(pi, tx, ty);
}

FloatRect RenderSVGText::relativeBBox(bool includeStroke) const
{
    FloatRect repaintRect;

    for (InlineRunBox* runBox = firstLineBox(); runBox; runBox = runBox->nextLineBox()) {
        ASSERT(runBox->isInlineFlowBox());

        InlineFlowBox* flowBox = static_cast<InlineFlowBox*>(runBox);
        for (InlineBox* box = flowBox->firstChild(); box; box = box->nextOnLine())
            repaintRect.unite(FloatRect(box->xPos(), box->yPos(), box->width(), box->height()));
    }

    // SVG needs to include the strokeWidth(), not the textStrokeWidth().
    if (includeStroke && style()->svgStyle()->hasStroke())
        repaintRect.inflate(KSVGPainterFactory::cssPrimitiveToLength(this, style()->svgStyle()->strokeWidth(), 0.0));

    repaintRect.move(xPos(), yPos());
    return repaintRect;
}

}

#endif // SVG_SUPPORT

// vim:ts=4:noet
