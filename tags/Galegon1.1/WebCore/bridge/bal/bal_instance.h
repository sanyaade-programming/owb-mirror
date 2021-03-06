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
 * @file  bal_instance.h
 *
 * Header file for bal_instance.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BINDINGS_BAL_INSTANCE_H_
#define BINDINGS_BAL_INSTANCE_H_

#include "runtime.h"
#include "runtime_root.h"

class WebObject;

namespace JSC {

namespace Bindings {

class BalClass;

class BalInstance : public Instance
{
public:
    static PassRefPtr<BalInstance> create(WebObject* object, PassRefPtr<RootObject> rootObject)
    {
        return adoptRef(new BalInstance(object, rootObject));
    }

    ~BalInstance ();
    
    virtual Class* getClass() const;

    virtual JSValuePtr valueOf(ExecState*) const;
    virtual JSValuePtr defaultValue(ExecState*, PreferredPrimitiveType) const;
    
    virtual JSValuePtr invokeMethod (ExecState *exec, const MethodList &method, const ArgList &args);
    virtual bool supportsInvokeDefaultMethod() const;
    virtual JSValuePtr invokeDefaultMethod (ExecState *exec, const ArgList &args);
    virtual void getPropertyNames(ExecState*, PropertyNameArray&);

    JSValuePtr stringValue(ExecState*) const;
    JSValuePtr numberValue(ExecState*) const;
    JSValuePtr booleanValue() const; 
    WebObject* getObject() const { return m_object; }

    static PassRefPtr<BalInstance> getBalInstance(WebObject*, PassRefPtr<RootObject>);
    virtual RuntimeObjectImp* createRuntimeObject(ExecState*);
private:
    BalInstance(WebObject*, PassRefPtr<RootObject>);

    mutable BalClass* m_class;
    WebObject *m_object;
};


} // namespace Bindings

} // namespace KJS

#endif
