/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#if USE(PLUGIN_HOST_PROCESS)

#ifndef NetscapePluginInstanceProxy_h
#define NetscapePluginInstanceProxy_h

#include <JavaScriptCore/Protect.h>
#include <WebCore/Timer.h>
#include <WebKit/npapi.h>
#include <wtf/Deque.h>
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RetainPtr.h>
#include "WebKitPluginHostTypes.h"

namespace WebCore {
    class String;
}

namespace JSC {
    namespace Bindings {
        class Instance;
        class RootObject;
    }
}
@class WebHostedNetscapePluginView;

namespace WebKit {

class HostedNetscapePluginStream;
class NetscapePluginHostProxy;
class ProxyInstance;
    
class NetscapePluginInstanceProxy : public RefCounted<NetscapePluginInstanceProxy> {
public:
    static PassRefPtr<NetscapePluginInstanceProxy> create(NetscapePluginHostProxy* pluginHostProxy, WebHostedNetscapePluginView *pluginView)
    {
        return adoptRef(new NetscapePluginInstanceProxy(pluginHostProxy, pluginView));
    }
    ~NetscapePluginInstanceProxy();
    
    uint32_t pluginID() const { return m_pluginID; }
    uint32_t renderContextID() const { return m_renderContextID; }
    void setRenderContextID(uint32_t renderContextID) { m_renderContextID = renderContextID; }
    
    bool useSoftwareRenderer() const { return m_useSoftwareRenderer; }
    void setUseSoftwareRenderer(bool useSoftwareRenderer) { m_useSoftwareRenderer = useSoftwareRenderer; }
    
    WebHostedNetscapePluginView *pluginView() const { return m_pluginView; }
    NetscapePluginHostProxy* hostProxy() const { return m_pluginHostProxy; }
    
    HostedNetscapePluginStream *pluginStream(uint32_t streamID);
    void disconnectStream(HostedNetscapePluginStream*);
    
    void pluginHostDied();
    
    void resize(NSRect size, NSRect clipRect);
    void destroy();
    void focusChanged(bool hasFocus);
    void windowFocusChanged(bool hasFocus);
    void windowFrameChanged(NSRect frame);
    
    void mouseEvent(NSView *pluginView, NSEvent *, NPCocoaEventType);
    void keyEvent(NSView *pluginView, NSEvent *, NPCocoaEventType);
    void startTimers(bool throttleTimers);
    void stopTimers();
    
    // NPRuntime
    bool getWindowNPObject(uint32_t& objectID);
    bool getPluginElementNPObject(uint32_t& objectID);
    void releaseObject(uint32_t objectID);
    
    bool evaluate(uint32_t objectID, const WebCore::String& script, data_t& resultData, mach_msg_type_number_t& resultLength);
    bool invoke(uint32_t objectID, const JSC::Identifier& methodName, data_t argumentsData, mach_msg_type_number_t argumentsLength, data_t& resultData, mach_msg_type_number_t& resultLength);
    bool invokeDefault(uint32_t objectID, data_t argumentsData, mach_msg_type_number_t argumentsLength, data_t& resultData, mach_msg_type_number_t& resultLength);
    bool construct(uint32_t objectID, data_t argumentsData, mach_msg_type_number_t argumentsLength, data_t& resultData, mach_msg_type_number_t& resultLength);
    
    bool getProperty(uint32_t objectID, const JSC::Identifier& propertyName, data_t &resultData, mach_msg_type_number_t& resultLength);
    bool getProperty(uint32_t objectID, unsigned propertyName, data_t &resultData, mach_msg_type_number_t& resultLength);    
    bool setProperty(uint32_t objectID, const JSC::Identifier& propertyName, data_t valueData, mach_msg_type_number_t valueLength);
    bool setProperty(uint32_t objectID, unsigned propertyName, data_t valueData, mach_msg_type_number_t valueLength);
    bool removeProperty(uint32_t objectID, const JSC::Identifier& propertyName);
    bool removeProperty(uint32_t objectID, unsigned propertyName);
    bool hasProperty(uint32_t objectID, const JSC::Identifier& propertyName);
    bool hasProperty(uint32_t objectID, unsigned propertyName);
    bool hasMethod(uint32_t objectID, const JSC::Identifier& methodName);
    
    void status(const char* message);
    NPError loadURL(const char* url, const char* target, const char* postData, uint32_t postDataLength, LoadURLFlags, uint32_t& requestID);

    PassRefPtr<JSC::Bindings::Instance> createBindingsInstance(PassRefPtr<JSC::Bindings::RootObject>);
    RetainPtr<NSData *> marshalValues(JSC::ExecState*, const JSC::ArgList& args);
    void marshalValue(JSC::ExecState*, JSC::JSValuePtr value, data_t& resultData, mach_msg_type_number_t& resultLength);
    JSC::JSValuePtr demarshalValue(JSC::ExecState*, const char* valueData, mach_msg_type_number_t valueLength);

    void addInstance(ProxyInstance*);
    void removeInstance(ProxyInstance*);
    
    // Reply structs
    struct Reply {
        enum Type {
            InstantiatePlugin,
            GetScriptableNPObject,
            BooleanAndData,
            Boolean
        };
        
        Reply(Type type) : m_type(type) { }
        virtual ~Reply() { }
    
        Type m_type;
    };

    struct InstantiatePluginReply : public Reply {
        static const int ReplyType = InstantiatePlugin;
        
        InstantiatePluginReply(kern_return_t resultCode, uint32_t renderContextID, boolean_t useSoftwareRenderer)
            : Reply(InstantiatePlugin)
            , m_resultCode(resultCode)
            , m_renderContextID(renderContextID)
            , m_useSoftwareRenderer(useSoftwareRenderer)
        {
        }
                 
        kern_return_t m_resultCode;
        uint32_t m_renderContextID;
        boolean_t m_useSoftwareRenderer;
    };

    struct GetScriptableNPObjectReply : public Reply {
        static const Reply::Type ReplyType = GetScriptableNPObject;
        
        GetScriptableNPObjectReply(uint32_t objectID)
            : Reply(ReplyType)
            , m_objectID(objectID)
        {
        }
            
        uint32_t m_objectID;
    };
    
    struct BooleanReply : public Reply {
        static const Reply::Type ReplyType = Boolean;
        
        BooleanReply(boolean_t result)
            : Reply(ReplyType)
            , m_result(result)
        {
        }
        
        boolean_t m_result;
    };

    struct BooleanAndDataReply : public Reply {
        static const Reply::Type ReplyType = BooleanAndData;
        
        BooleanAndDataReply(boolean_t returnValue, RetainPtr<CFDataRef> result)
            : Reply(ReplyType)
            , m_returnValue(returnValue)
            , m_result(result)
        {
        }
        
        boolean_t m_returnValue;
        RetainPtr<CFDataRef> m_result;
    };
    
    void setCurrentReply(Reply* reply)
    {
        ASSERT(!m_currentReply.get());
        m_currentReply = std::auto_ptr<Reply>(reply);
    }
    
    template <typename T>
    std::auto_ptr<T> waitForReply()
    {
        m_waitingForReply = true;
        
        processRequestsAndWaitForReply();
        
        if (m_currentReply.get()) 
            ASSERT(m_currentReply->m_type == T::ReplyType);
        
        m_waitingForReply = false;
        return std::auto_ptr<T>(static_cast<T*>(m_currentReply.release()));
    }
    
private:
    NetscapePluginInstanceProxy(NetscapePluginHostProxy*, WebHostedNetscapePluginView *);

    NPError loadRequest(NSURLRequest *, const char* cTarget, bool currentEventIsUserGesture, uint32_t& streamID);
    
    class PluginRequest;
    void performRequest(PluginRequest*);
    void evaluateJavaScript(PluginRequest*);
    
    void stopAllStreams();
    void processRequestsAndWaitForReply();
    
    void cleanup();
    
    NetscapePluginHostProxy* m_pluginHostProxy;
    WebHostedNetscapePluginView *m_pluginView;

    void requestTimerFired(WebCore::Timer<NetscapePluginInstanceProxy>*);
    WebCore::Timer<NetscapePluginInstanceProxy> m_requestTimer;
    Deque<PluginRequest*> m_pluginRequests;
    
    HashMap<uint32_t, RefPtr<HostedNetscapePluginStream> > m_streams;

    uint32_t m_currentRequestID;
    
    uint32_t m_pluginID;
    uint32_t m_renderContextID;
    boolean_t m_useSoftwareRenderer;
    
    bool m_waitingForReply;
    std::auto_ptr<Reply> m_currentReply;
    
    // NPRuntime
    uint32_t idForObject(JSC::JSObject*);
    
    void addValueToArray(NSMutableArray *, JSC::ExecState* exec, JSC::JSValuePtr value);
    
    bool demarshalValueFromArray(JSC::ExecState*, NSArray *array, NSUInteger& index, JSC::JSValuePtr& result);
    void demarshalValues(JSC::ExecState*, data_t valuesData, mach_msg_type_number_t valuesLength, JSC::ArgList& result);

    uint32_t m_objectIDCounter;
    typedef HashMap<uint32_t, JSC::ProtectedPtr<JSC::JSObject> > ObjectMap;
    ObjectMap m_objects;
    
    typedef HashSet<ProxyInstance*> ProxyInstanceSet;
    ProxyInstanceSet m_instances;
};
    
} // namespace WebKit

#endif // NetscapePluginInstanceProxy_h
#endif // USE(PLUGIN_HOST_PROCESS)
