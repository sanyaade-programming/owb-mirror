/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004 Apple Computer, Inc.
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

#ifndef HTMLImageLoader_h
#define HTMLImageLoader_h

#include "CachedResourceClient.h"

namespace WebCore {

class Element;

class HTMLImageLoader : public CachedResourceClient {
public:
    HTMLImageLoader(Element*);
    virtual ~HTMLImageLoader();

    virtual void updateFromElement();

    virtual void dispatchLoadEvent();

    Element* element() const { return m_element; }
    bool imageComplete() const { return m_imageComplete; }

    CachedImage* image() const { return m_image; }
    void setImage(CachedImage*);

    void setLoadManually(bool loadManually) { m_loadManually = loadManually; }

    // CachedResourceClient API
    virtual void notifyFinished(CachedResource*);

protected:
    void setLoadingImage(CachedImage*);
    
    bool haveFiredLoadEvent() { return m_firedLoad; }
    void setHaveFiredLoadEvent(bool firedLoad) { m_firedLoad = firedLoad; }

private:
    Element* m_element;
    CachedImage* m_image;
    bool m_firedLoad : 1;
    bool m_imageComplete : 1;
    bool m_loadManually : 1;
};

} //namespace

#endif