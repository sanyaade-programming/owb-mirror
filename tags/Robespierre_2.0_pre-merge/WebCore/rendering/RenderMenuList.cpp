/**
 * This file is part of the select element renderer in WebCore.
 *
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
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
#include "RenderMenuList.h"

#include "Document.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HTMLNames.h"
#include "HTMLOptionElement.h"
#include "HTMLOptGroupElement.h"
#include "HTMLSelectElement.h"
#include "PopupMenu.h"
#include "RenderBR.h"
#include "RenderText.h"
#include "RenderTheme.h"
#include "TextStyle.h"

#ifdef __OWB__
#include <BIMath.h>
#else
#include <math.h>
#endif


using namespace std;

namespace WebCore {

using namespace HTMLNames;

RenderMenuList::RenderMenuList(HTMLSelectElement* element)
    : RenderFlexibleBox(element)
    , m_buttonText(0)
    , m_innerBlock(0)
    , m_optionsChanged(true)
    , m_optionsWidth(0)
    , m_popup(0)
    , m_popupIsVisible(false)
{
}

RenderMenuList::~RenderMenuList()
{
    if (m_popup)
        m_popup->disconnectClient();
    m_popup = 0;
}

void RenderMenuList::createInnerBlock()
{
    if (m_innerBlock) {
        ASSERT(firstChild() == m_innerBlock);
        ASSERT(!m_innerBlock->nextSibling());
        return;
    }

    // Create an anonymous block.
    ASSERT(!firstChild());
    m_innerBlock = createAnonymousBlock();
    m_innerBlock->style()->setBoxFlex(1.0f);
    RenderFlexibleBox::addChild(m_innerBlock);
}

void RenderMenuList::addChild(RenderObject* newChild, RenderObject* beforeChild)
{
    createInnerBlock();
    m_innerBlock->addChild(newChild, beforeChild);
}

void RenderMenuList::removeChild(RenderObject* oldChild)
{
    if (oldChild == m_innerBlock || !m_innerBlock) {
        RenderFlexibleBox::removeChild(oldChild);
        m_innerBlock = 0;
    } else
        m_innerBlock->removeChild(oldChild);
}

void RenderMenuList::setStyle(RenderStyle* newStyle)
{
    bool fontChanged = !style() || style()->font() != newStyle->font();

    RenderBlock::setStyle(newStyle);

    if (m_buttonText)
        m_buttonText->setStyle(newStyle);
    if (m_innerBlock) // RenderBlock handled updating the anonymous block's style.
        m_innerBlock->style()->setBoxFlex(1.0f);
    setReplaced(isInline());
    if (fontChanged)
        updateOptionsWidth();
}

void RenderMenuList::updateOptionsWidth()
{
    // FIXME: There is no longer any reason to use a text style with the font hacks disabled.
    // It is a leftover from when the text was not drawn with the engine -- now that we render
    // with the engine, we can just use the default style as the engine does.
    TextStyle textStyle(0, 0, 0, false, false, false, false);

    float maxOptionWidth = 0;
    const Vector<HTMLElement*>& listItems = static_cast<HTMLSelectElement*>(node())->listItems();
    int size = listItems.size();    
    for (int i = 0; i < size; ++i) {
        HTMLElement* element = listItems[i];
        if (element->hasTagName(optionTag)) {
            String text = static_cast<HTMLOptionElement*>(element)->optionText();
            if (!text.isEmpty())
                maxOptionWidth = max(maxOptionWidth, style()->font().floatWidth(text.impl(), textStyle));
        }
    }

    int width = static_cast<int>(ceilf(maxOptionWidth));
    if (m_optionsWidth == width)
        return;

    m_optionsWidth = width;
    setNeedsLayoutAndMinMaxRecalc();
}

void RenderMenuList::updateFromElement()
{
    if (m_optionsChanged) {
        updateOptionsWidth();
        m_optionsChanged = false;
    }

    setTextFromOption(static_cast<HTMLSelectElement*>(node())->selectedIndex());

    if (m_popupIsVisible)
        m_popup->updateFromElement();
}

void RenderMenuList::setTextFromOption(int optionIndex)
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    const Vector<HTMLElement*>& listItems = select->listItems();
    int size = listItems.size();

    int i = select->optionToListIndex(optionIndex);
    String text = "";
    if (i >= 0 && i < size) {
        HTMLElement* element = listItems[i];
        if (element->hasTagName(optionTag))
            text = static_cast<HTMLOptionElement*>(listItems[i])->optionText();
    }
    setText(text.stripWhiteSpace());
}

void RenderMenuList::setText(const String& s)
{
    if (s.isEmpty()) {
        if (!m_buttonText || !m_buttonText->isBR()) {
            if (m_buttonText)
                m_buttonText->destroy();
            m_buttonText = new (renderArena()) RenderBR(document());
            m_buttonText->setStyle(style());
            addChild(m_buttonText);
        }
    } else {
        if (m_buttonText && !m_buttonText->isBR())
            m_buttonText->setText(s.impl());
        else {
            if (m_buttonText)
                m_buttonText->destroy();
            m_buttonText = new (renderArena()) RenderText(document(), s.impl());
            m_buttonText->setStyle(style());
            addChild(m_buttonText);
        }
    }
}

String RenderMenuList::text() const
{
    return m_buttonText ? m_buttonText->text() : 0;
}

IntRect RenderMenuList::controlClipRect(int tx, int ty) const
{
    // Clip to the content box, since the arrow sits in the padding space, and we don't want to draw over it.
    return IntRect(tx + borderLeft() + paddingLeft(), 
                   ty + borderTop() + paddingTop(),
                   contentWidth(), contentHeight());
}

void RenderMenuList::calcMinMaxWidth()
{
    m_minWidth = 0;
    m_maxWidth = 0;

    if (style()->width().isFixed() && style()->width().value() > 0)
        m_minWidth = m_maxWidth = calcContentBoxWidth(style()->width().value());
    else
        m_maxWidth = max(m_optionsWidth, theme()->minimumMenuListSize(style()));

    if (style()->minWidth().isFixed() && style()->minWidth().value() > 0) {
        m_maxWidth = max(m_maxWidth, calcContentBoxWidth(style()->minWidth().value()));
        m_minWidth = max(m_minWidth, calcContentBoxWidth(style()->minWidth().value()));
    } else if (style()->width().isPercent() || (style()->width().isAuto() && style()->height().isPercent()))
        m_minWidth = 0;
    else
        m_minWidth = m_maxWidth;

    if (style()->maxWidth().isFixed() && style()->maxWidth().value() != undefinedLength) {
        m_maxWidth = min(m_maxWidth, calcContentBoxWidth(style()->maxWidth().value()));
        m_minWidth = min(m_minWidth, calcContentBoxWidth(style()->maxWidth().value()));
    }

    int toAdd = paddingLeft() + paddingRight() + borderLeft() + borderRight();
    m_minWidth += toAdd;
    m_maxWidth += toAdd;

    setMinMaxKnown();
}

void RenderMenuList::showPopup()
{
    if (m_popupIsVisible)
        return;

    // Create m_innerBlock here so it ends up as the first child.
    // This is important because otherwise we might try to create m_innerBlock
    // inside the showPopup call and it would fail.
    createInnerBlock();
    if (!m_popup)
        m_popup = PopupMenu::create(this);
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    m_popupIsVisible = true;
    m_popup->show(absoluteBoundingBoxRect(), document()->view(),
        select->optionToListIndex(select->selectedIndex()));
}

void RenderMenuList::hidePopup()
{
    if (m_popup)
        m_popup->hide();
    m_popupIsVisible = false;
}

void RenderMenuList::valueChanged(unsigned listIndex, bool fireOnChange)
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    select->setSelectedIndex(select->listToOptionIndex(listIndex), true, fireOnChange);
}

String RenderMenuList::itemText(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    if (element->hasTagName(optgroupTag))
        return static_cast<HTMLOptGroupElement*>(element)->groupLabelText();
    else if (element->hasTagName(optionTag))
        return static_cast<HTMLOptionElement*>(element)->optionText();
    return String();
}

bool RenderMenuList::itemIsEnabled(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    if (!element->hasTagName(optionTag))
        return false;
    bool groupEnabled = true;
    if (element->parentNode() && element->parentNode()->hasTagName(optgroupTag))
        groupEnabled = element->parentNode()->isEnabled();
    return element->isEnabled() && groupEnabled;
}

RenderStyle* RenderMenuList::itemStyle(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    
    return element->renderStyle() ? element->renderStyle() : style();
}

RenderStyle* RenderMenuList::clientStyle() const
{
    return style();
}

Document* RenderMenuList::clientDocument() const
{
    return document();
}

int RenderMenuList::clientPaddingLeft() const
{
    return paddingLeft();
}

int RenderMenuList::clientPaddingRight() const
{
    return paddingRight();
}

unsigned RenderMenuList::listSize() const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    return select->listItems().size();
}

int RenderMenuList::selectedIndex() const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    return select->optionToListIndex(select->selectedIndex());
}

bool RenderMenuList::itemIsSeparator(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    return element->hasTagName(hrTag);
}

bool RenderMenuList::itemIsLabel(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    return element->hasTagName(optgroupTag);
}

bool RenderMenuList::itemIsSelected(unsigned listIndex) const
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    HTMLElement* element = select->listItems()[listIndex];
    return element->hasTagName(optionTag)&& static_cast<HTMLOptionElement*>(element)->selected();
}

void RenderMenuList::setTextFromItem(unsigned listIndex)
{
    HTMLSelectElement* select = static_cast<HTMLSelectElement*>(node());
    setTextFromOption(select->listToOptionIndex(listIndex));
}

}