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

#ifndef BCOBSERVERSERVICE_H
#define BCOBSERVERSERVICE_H

#include "BIObserver.h"
#include "BIObserverService.h"
#include "StringHash.h"
#include <wtf/HashMap.h>
#include <wtf/Vector.h>

using BAL::BIObserver;
using BAL::BIObserverService;

namespace BC {

    /**
     * @brief the BCObserverService
     *
     * The observer service implementation
     *
     */
    class BCObserverService : public BIObserverService {
        public:
            BCObserverService();
            virtual ~BCObserverService() {};

            virtual void registerObserver(const String& topic, BIObserver* observer);
            virtual void notifyObserver(const String& topic, const String& data);
            virtual void removeObserver(const String& topic, BIObserver* observer);
        private:
            HashMap<String, Vector<BIObserver*> > m_topic;
    };
}

#endif //BCOBSERVERSERVICE_H
