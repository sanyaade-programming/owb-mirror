/*
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
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

#ifndef RenderWidget_h
#define RenderWidget_h

#include "RenderReplaced.h"
#ifdef __OWB__
#include "Widget.h"
#endif

namespace WebCore {

class Widget;

class RenderWidget : public RenderReplaced {
public:
    RenderWidget(Node*);
    virtual ~RenderWidget();

    virtual bool isWidget() const { return true; }

    virtual void setStyle(RenderStyle*);

    virtual void paint(PaintInfo&, int tx, int ty);

    virtual void destroy();
    virtual void layout();

    Widget* widget() const { return m_widget; }
    static RenderWidget* find(const Widget*);

    RenderArena* ref() { ++m_refCount; return renderArena(); }
    void deref(RenderArena*);

    virtual void setSelectionState(SelectionState);

    virtual void updateWidgetPosition();

    virtual void setWidget(Widget*);

    using RenderReplaced::element;

private:
    void resizeWidget(Widget*, int w, int h);

    virtual void deleteWidget();

protected:
    Widget* m_widget;
    FrameView* m_view;

private:
    int m_refCount;
};

} // namespace WebCore

#endif // RenderWidget_h
