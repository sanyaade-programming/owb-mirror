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
 * @file  BCWheelEvent.h
 *
 * @brief Concretisation of wheel event
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 * This header file is private. Only IDL interface is public.
 *
 */

#include "BIWheelEvent.h"
#include "../Common/BCCommonInputEventData.h"

namespace BC
{

/**
 * @brief the WheelEvent
 *
 * The wheel event
 *
 * @see BIEvent, BIEventLoop
 */
class BCWheelEvent : public BAL::BIWheelEvent, public BCCommonInputEventData {
public:
    BCWheelEvent(const IntPoint& pos, const IntPoint& globalPos,
                                int deltaX, int deltaY, bool isAccepted,
                                bool shift, bool ctrl, bool alt, bool meta);

    virtual const IntPoint& pos() const;
    virtual const IntPoint& globalPos() const;
    virtual int deltaX() const;
    virtual int deltaY() const;
    virtual bool isAccepted() const;

    virtual void accept();
    virtual void ignore();

    virtual BIEvent* clone() const;

    virtual bool shiftKey() const { return BCCommonInputEventData::shiftKey(); }
    virtual bool ctrlKey() const { return BCCommonInputEventData::ctrlKey(); }
    virtual bool altKey() const { return BCCommonInputEventData::altKey(); }
    virtual bool metaKey() const { return BCCommonInputEventData::metaKey(); }

private:
    IntPoint m_position;
    IntPoint m_globalPosition;
    int m_deltaX;
    int m_deltaY;
    bool m_isAccepted;
};

}
