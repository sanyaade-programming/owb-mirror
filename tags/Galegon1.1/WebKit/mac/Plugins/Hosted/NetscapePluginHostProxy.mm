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

#import "NetscapePluginHostProxy.h"

#import <mach/mach.h>
#import <wtf/StdLibExtras.h>

#import "HostedNetscapePluginStream.h"
#import "NetscapePluginHostManager.h"
#import "NetscapePluginInstanceProxy.h"
#import "WebFrameInternal.h"
#import "WebHostedNetscapePluginView.h"
#import "WebKitSystemInterface.h"
#import <WebCore/Frame.h>
#import <WebCore/IdentifierRep.h>
#import <WebCore/ScriptController.h>

extern "C" {
#import "WebKitPluginHost.h"
#import "WebKitPluginClientServer.h"
}

using namespace std;
using namespace JSC;
using namespace WebCore;

namespace WebKit {

typedef HashMap<mach_port_t, NetscapePluginHostProxy*> PluginProxyMap;
static PluginProxyMap& pluginProxyMap()
{
    DEFINE_STATIC_LOCAL(PluginProxyMap, pluginProxyMap, ());
    
    return pluginProxyMap;
}

NetscapePluginHostProxy::NetscapePluginHostProxy(mach_port_t clientPort, mach_port_t pluginHostPort, const ProcessSerialNumber& pluginHostPSN)
    : m_clientPort(clientPort)
    , m_portSet(MACH_PORT_NULL)
    , m_pluginHostPort(pluginHostPort)
    , m_isModal(false)
    , m_menuBarIsVisible(true)
    , m_pluginHostPSN(pluginHostPSN)
{
    pluginProxyMap().add(m_clientPort, this);
    
    // FIXME: We should use libdispatch for this.
    CFMachPortContext context = { 0, this, 0, 0, 0 };
    m_deadNameNotificationPort.adoptCF(CFMachPortCreate(0, deadNameNotificationCallback, &context, 0));

    mach_port_t previous;
    mach_port_request_notification(mach_task_self(), pluginHostPort, MACH_NOTIFY_DEAD_NAME, 0, 
                                   CFMachPortGetPort(m_deadNameNotificationPort.get()), MACH_MSG_TYPE_MAKE_SEND_ONCE, &previous);
    ASSERT(previous == MACH_PORT_NULL);
    
    RetainPtr<CFRunLoopSourceRef> deathPortSource(AdoptCF, CFMachPortCreateRunLoopSource(0, m_deadNameNotificationPort.get(), 0));
    
    CFRunLoopAddSource(CFRunLoopGetCurrent(), deathPortSource.get(), kCFRunLoopDefaultMode);
    
#ifdef USE_LIBDISPATCH
    // FIXME: Unfortunately we can't use a dispatch source here until <rdar://problem/6393180> has been resolved.
    m_clientPortSource = dispatch_source_mig_create(m_clientPort, WKWebKitPluginClient_subsystem.maxsize, 0, 
                                                    dispatch_get_main_queue(), WebKitPluginClient_server);
#else
    m_clientPortSource.adoptCF(WKCreateMIGServerSource((mig_subsystem_t)&WKWebKitPluginClient_subsystem, m_clientPort));
    CFRunLoopAddSource(CFRunLoopGetCurrent(), m_clientPortSource.get(), kCFRunLoopDefaultMode);
#endif
}

NetscapePluginHostProxy::~NetscapePluginHostProxy()
{
    pluginProxyMap().remove(m_clientPort);

    // Free the port set
    if (m_portSet) {
        mach_port_extract_member(mach_task_self(), m_clientPort, m_portSet);
        mach_port_extract_member(mach_task_self(), CFMachPortGetPort(m_deadNameNotificationPort.get()), m_portSet);
        mach_port_destroy(mach_task_self(), m_portSet);
    }
    
    ASSERT(m_clientPortSource);
#ifdef USE_LIBDISPATCH
    dispatch_release(m_clientPortSource);
#else
    CFRunLoopSourceInvalidate(m_clientPortSource.get());
    m_clientPortSource = 0;
#endif
}

void NetscapePluginHostProxy::pluginHostDied()
{
    PluginInstanceMap instances;    
    m_instances.swap(instances);
  
    PluginInstanceMap::const_iterator end = instances.end();
    for (PluginInstanceMap::const_iterator it = instances.begin(); it != end; ++it)
        it->second->pluginHostDied();
    
    NetscapePluginHostManager::shared().pluginHostDied(this);
    
    // The plug-in crashed while its menu bar was hidden. Make sure to show it.
    if (!m_menuBarIsVisible)
        setMenuBarVisible(true);

    // The plug-in crashed while it had a modal dialog up.
    if (m_isModal)
        endModal();
    
    delete this;
}
    
void NetscapePluginHostProxy::addPluginInstance(NetscapePluginInstanceProxy* instance)
{
    ASSERT(!m_instances.contains(instance->pluginID()));
    
    m_instances.set(instance->pluginID(), instance);
}
    
void NetscapePluginHostProxy::removePluginInstance(NetscapePluginInstanceProxy* instance)
{
    ASSERT(m_instances.get(instance->pluginID()) == instance);

    m_instances.remove(instance->pluginID());
}

NetscapePluginInstanceProxy* NetscapePluginHostProxy::pluginInstance(uint32_t pluginID)
{
    return m_instances.get(pluginID).get();
}

void NetscapePluginHostProxy::deadNameNotificationCallback(CFMachPortRef port, void *msg, CFIndex size, void *info)
{
    ASSERT(msg && static_cast<mach_msg_header_t*>(msg)->msgh_id == MACH_NOTIFY_DEAD_NAME);
    
    static_cast<NetscapePluginHostProxy*>(info)->pluginHostDied();
}

void NetscapePluginHostProxy::setMenuBarVisible(bool visible)
{
    m_menuBarIsVisible = visible;
    
    [NSMenu setMenuBarVisible:visible];
    if (visible) {
        // Make ourselves the front app
        ProcessSerialNumber psn;
        GetCurrentProcess(&psn);
        SetFrontProcess(&psn);
    }
}

void NetscapePluginHostProxy::applicationDidBecomeActive()
{
    SetFrontProcess(&m_pluginHostPSN);
}

void NetscapePluginHostProxy::beginModal()
{
    ASSERT(!m_placeholderWindow);
    ASSERT(!m_activationObserver);
    
    m_placeholderWindow.adoptNS([[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1, 1) styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:YES]);
    
    m_activationObserver = [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationWillBecomeActiveNotification object:NSApp queue:nil
                                                                         usingBlock:^(NSNotification *){ applicationDidBecomeActive(); }];
    
    // We need to be able to get the setModal(false) call from the plug-in host.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), m_clientPortSource.get(), (CFStringRef)NSModalPanelRunLoopMode);
    
    [NSApp runModalForWindow:m_placeholderWindow.get()];
}
    
void NetscapePluginHostProxy::endModal()
{
    ASSERT(m_placeholderWindow);
    ASSERT(m_activationObserver);
    
    [[NSNotificationCenter defaultCenter] removeObserver:m_activationObserver.get()];
    m_activationObserver = nil;
    
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m_clientPortSource.get(), (CFStringRef)NSModalPanelRunLoopMode);
    
    [NSApp stopModal];
    [m_placeholderWindow.get() orderOut:nil];
    m_placeholderWindow = 0;
    
    // Make ourselves the front process.
    ProcessSerialNumber psn;
    GetCurrentProcess(&psn);
    SetFrontProcess(&psn);            
}
    

void NetscapePluginHostProxy::setModal(bool modal)
{
    if (modal == m_isModal) 
        return;
    
    m_isModal = modal;
    
    if (m_isModal)
        beginModal();
    else
        endModal();
}
    
bool NetscapePluginHostProxy::processRequests()
{
    if (!m_portSet) {
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET, &m_portSet);
        mach_port_insert_member(mach_task_self(), m_clientPort, m_portSet);
        mach_port_insert_member(mach_task_self(), CFMachPortGetPort(m_deadNameNotificationPort.get()), m_portSet);
    }
    
    char buffer[4096];
    
    mach_msg_header_t* msg = reinterpret_cast<mach_msg_header_t*>(buffer);
    
    kern_return_t kr = mach_msg(msg, MACH_RCV_MSG, 0, sizeof(buffer), m_portSet, 0, MACH_PORT_NULL);
    
    if (kr != KERN_SUCCESS) {
        LOG_ERROR("Could not receive mach message, error %x", kr);
        return false;
    }
    
    if (msg->msgh_local_port == m_clientPort) {
        __ReplyUnion__WKWebKitPluginClient_subsystem reply;
        mach_msg_header_t* replyHeader = reinterpret_cast<mach_msg_header_t*>(&reply);
        
        if (WebKitPluginClient_server(msg, replyHeader) && replyHeader->msgh_remote_port != MACH_PORT_NULL) {
            kr = mach_msg(replyHeader, MACH_SEND_MSG, replyHeader->msgh_size, 0, MACH_PORT_NULL, 0, MACH_PORT_NULL);
            
            if (kr != KERN_SUCCESS) {
                LOG_ERROR("Could not send mach message, error %x", kr);
                return false;
            }
        }
        
        return true;
    }
    
    if (msg->msgh_local_port == CFMachPortGetPort(m_deadNameNotificationPort.get())) {
        ASSERT(msg->msgh_id == MACH_NOTIFY_DEAD_NAME);
        pluginHostDied();
        return false;
    }
    
    ASSERT_NOT_REACHED();
    return false;
}

} // namespace WebKit

using namespace WebKit;

// MiG callbacks
kern_return_t WKPCStatusText(mach_port_t clientPort, uint32_t pluginID, data_t text, mach_msg_type_number_t textCnt)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    instanceProxy->status(text);
    return KERN_SUCCESS;
}

kern_return_t WKPCLoadURL(mach_port_t clientPort, uint32_t pluginID, data_t url, mach_msg_type_number_t urlLength, data_t target, mach_msg_type_number_t targetLength, 
                          data_t postData, mach_msg_type_number_t postDataLength, uint32_t flags,
                          uint16_t* outResult, uint32_t* outStreamID)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    uint32_t streamID = 0;
    NPError result = instanceProxy->loadURL(url, target, postData, postDataLength, static_cast<LoadURLFlags>(flags), streamID);
    
    *outResult = result;
    *outStreamID = streamID;
    return KERN_SUCCESS;
}

kern_return_t WKPCCancelLoadURL(mach_port_t clientPort, uint32_t pluginID, uint32_t streamID, int16_t reason)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    HostedNetscapePluginStream* pluginStream = instanceProxy->pluginStream(streamID);
    if (!pluginStream)
        return KERN_FAILURE;

    pluginStream->cancelLoad(reason);
    return KERN_SUCCESS;
}

kern_return_t WKPCInvalidateRect(mach_port_t clientPort, uint32_t pluginID, double x, double y, double width, double height)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    [instanceProxy->pluginView() setNeedsDisplayInRect:NSMakeRect(x, y, width, height)];
    
    return KERN_SUCCESS;
}

kern_return_t WKPCGetScriptableNPObjectReply(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    instanceProxy->setCurrentReply(new NetscapePluginInstanceProxy::GetScriptableNPObjectReply(objectID));
    return KERN_SUCCESS;
}

kern_return_t WKPCBooleanReply(mach_port_t clientPort, uint32_t pluginID, boolean_t result)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    instanceProxy->setCurrentReply(new NetscapePluginInstanceProxy::BooleanReply(result));
    return KERN_SUCCESS;
}

kern_return_t WKPCBooleanAndDataReply(mach_port_t clientPort, uint32_t pluginID, boolean_t returnValue, data_t resultData, mach_msg_type_number_t resultLength)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    RetainPtr<CFDataRef> result = CFDataCreate(0, reinterpret_cast<UInt8*>(resultData), resultLength);
    instanceProxy->setCurrentReply(new NetscapePluginInstanceProxy::BooleanAndDataReply(returnValue, result));
    
    return KERN_SUCCESS;
}

kern_return_t WKPCInstantiatePluginReply(mach_port_t clientPort, uint32_t pluginID, kern_return_t result, uint32_t renderContextID, boolean_t useSoftwareRenderer)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    instanceProxy->setCurrentReply(new NetscapePluginInstanceProxy::InstantiatePluginReply(result, renderContextID, useSoftwareRenderer));
    return KERN_SUCCESS;
}

kern_return_t WKPCGetWindowNPObject(mach_port_t clientPort, uint32_t pluginID, uint32_t* outObjectID)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    uint32_t objectID;
    if (!instanceProxy->getWindowNPObject(objectID))
        return KERN_FAILURE;
    
    *outObjectID = objectID;    
    return KERN_SUCCESS;
}

kern_return_t WKPCGetPluginElementNPObject(mach_port_t clientPort, uint32_t pluginID, uint32_t* outObjectID)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    uint32_t objectID;
    if (!instanceProxy->getPluginElementNPObject(objectID))
        return KERN_FAILURE;
    
    *outObjectID = objectID;    
    return KERN_SUCCESS;
}

kern_return_t WKPCReleaseObject(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    instanceProxy->releaseObject(objectID);
    return KERN_SUCCESS;
}

kern_return_t WKPCEvaluate(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, data_t scriptData, mach_msg_type_number_t scriptLength)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    String script = String::fromUTF8WithLatin1Fallback(scriptData, scriptLength);
    
    data_t resultData;
    mach_msg_type_number_t resultLength;
    
    boolean_t returnValue = instanceProxy->evaluate(objectID, script, resultData, resultLength);
    
    _WKPHBooleanAndDataReply(hostProxy->port(), instanceProxy->pluginID(), returnValue, resultData, resultLength);
    mig_deallocate(reinterpret_cast<vm_address_t>(resultData), resultLength);
        
    return KERN_SUCCESS;
}

kern_return_t WKPCGetStringIdentifier(mach_port_t clientPort, data_t name, mach_msg_type_number_t nameCnt, uint64_t* identifier)
{
    COMPILE_ASSERT(sizeof(*identifier) == sizeof(IdentifierRep*), identifier_sizes);
    
    *identifier = reinterpret_cast<uint64_t>(IdentifierRep::get(name));
    return KERN_SUCCESS;
}

kern_return_t WKPCGetIntIdentifier(mach_port_t clientPort, int32_t value, uint64_t* identifier)
{
    COMPILE_ASSERT(sizeof(*identifier) == sizeof(NPIdentifier), identifier_sizes);
    
    *identifier = reinterpret_cast<uint64_t>(IdentifierRep::get(value));
    return KERN_SUCCESS;
}

static Identifier identifierFromIdentifierRep(IdentifierRep* identifier)
{
    ASSERT(IdentifierRep::isValid(identifier));
    ASSERT(identifier->isString());
  
    const char* str = identifier->string();    
    return Identifier(JSDOMWindow::commonJSGlobalData(), String::fromUTF8WithLatin1Fallback(str, strlen(str)));
}

kern_return_t WKPCInvoke(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier,
                         data_t argumentsData, mach_msg_type_number_t argumentsLength) 
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier)) {
        _WKPHBooleanAndDataReply(hostProxy->port(), instanceProxy->pluginID(), false, 0, 0);
        return KERN_SUCCESS;
    }

    Identifier methodNameIdentifier = identifierFromIdentifierRep(identifier);

    data_t resultData;
    mach_msg_type_number_t resultLength;
    
    boolean_t returnValue = instanceProxy->invoke(objectID, methodNameIdentifier, argumentsData, argumentsLength, resultData, resultLength);
    
    _WKPHBooleanAndDataReply(hostProxy->port(), instanceProxy->pluginID(), returnValue, resultData, resultLength);
    mig_deallocate(reinterpret_cast<vm_address_t>(resultData), resultLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCInvokeDefault(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID,
                                data_t argumentsData, mach_msg_type_number_t argumentsLength, 
                                boolean_t*returnValue, data_t* resultData, mach_msg_type_number_t* resultLength)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    *returnValue = instanceProxy->invokeDefault(objectID, argumentsData, argumentsLength, *resultData, *resultLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCConstruct(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID,
                            data_t argumentsData, mach_msg_type_number_t argumentsLength, 
                            boolean_t*returnValue, data_t* resultData, mach_msg_type_number_t* resultLength)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    *returnValue = instanceProxy->construct(objectID, argumentsData, argumentsLength, *resultData, *resultLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCGetProperty(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier))
        return KERN_FAILURE;
    
    data_t resultData;
    mach_msg_type_number_t resultLength;
    boolean_t returnValue;
    
    if (identifier->isString()) {
        Identifier propertyNameIdentifier = identifierFromIdentifierRep(identifier);        
        returnValue = instanceProxy->getProperty(objectID, propertyNameIdentifier, resultData, resultLength);
    } else 
        returnValue = instanceProxy->setProperty(objectID, identifier->number(), resultData, resultLength);
    
    _WKPHBooleanAndDataReply(hostProxy->port(), instanceProxy->pluginID(), returnValue, resultData, resultLength);
    mig_deallocate(reinterpret_cast<vm_address_t>(resultData), resultLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCSetProperty(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier, data_t valueData, mach_msg_type_number_t valueLength, boolean_t*returnValue)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;

    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier))
        *returnValue = false;
    
    if (identifier->isString()) {
        Identifier propertyNameIdentifier = identifierFromIdentifierRep(identifier);        
        *returnValue = instanceProxy->setProperty(objectID, propertyNameIdentifier, valueData, valueLength);
    } else 
        *returnValue = instanceProxy->setProperty(objectID, identifier->number(), valueData, valueLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCRemoveProperty(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier, boolean_t* returnValue)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier))
        return KERN_FAILURE;
        
    if (identifier->isString()) {
        Identifier propertyNameIdentifier = identifierFromIdentifierRep(identifier);        
        *returnValue = instanceProxy->removeProperty(objectID, propertyNameIdentifier);
    } else 
        *returnValue = instanceProxy->removeProperty(objectID, identifier->number());
    
    return KERN_SUCCESS;
}

kern_return_t WKPCHasProperty(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier)) {
        _WKPHBooleanReply(hostProxy->port(), instanceProxy->pluginID(), false);
        return KERN_SUCCESS;
    }    
    
    boolean_t returnValue;
    if (identifier->isString()) {
        Identifier propertyNameIdentifier = identifierFromIdentifierRep(identifier);        
        returnValue = instanceProxy->hasProperty(objectID, propertyNameIdentifier);
    } else 
        returnValue = instanceProxy->hasProperty(objectID, identifier->number());
    
    _WKPHBooleanReply(hostProxy->port(), instanceProxy->pluginID(), returnValue);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCHasMethod(mach_port_t clientPort, uint32_t pluginID, uint32_t objectID, uint64_t serverIdentifier)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    NetscapePluginInstanceProxy* instanceProxy = hostProxy->pluginInstance(pluginID);
    if (!instanceProxy)
        return KERN_FAILURE;
    
    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier)) {
        _WKPHBooleanReply(hostProxy->port(), instanceProxy->pluginID(), false);
        return KERN_SUCCESS;
    }
    
    Identifier methodNameIdentifier = identifierFromIdentifierRep(identifier);        
    boolean_t returnValue = instanceProxy->hasMethod(objectID, methodNameIdentifier);

    _WKPHBooleanReply(hostProxy->port(), instanceProxy->pluginID(), returnValue);

    return KERN_SUCCESS;
}

kern_return_t WKPCIdentifierInfo(mach_port_t clientPort, uint64_t serverIdentifier, data_t* infoData, mach_msg_type_number_t* infoLength)
{
    IdentifierRep* identifier = reinterpret_cast<IdentifierRep*>(serverIdentifier);
    if (!IdentifierRep::isValid(identifier))
        return KERN_FAILURE;
    
    id info;
    if (identifier->isString()) {
        const char* str = identifier->string();
        info = [NSData dataWithBytesNoCopy:(void*)str length:strlen(str) freeWhenDone:NO];
    } else 
        info = [NSNumber numberWithInt:identifier->number()];

    RetainPtr<NSData*> data = [NSPropertyListSerialization dataFromPropertyList:info format:NSPropertyListBinaryFormat_v1_0 errorDescription:0];
    ASSERT(data);
    
    *infoLength = [data.get() length];
    mig_allocate(reinterpret_cast<vm_address_t*>(infoData), *infoLength);
    
    memcpy(*infoData, [data.get() bytes], *infoLength);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCSetMenuBarVisible(mach_port_t clientPort, boolean_t menuBarVisible)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;

    hostProxy->setMenuBarVisible(menuBarVisible);
    
    return KERN_SUCCESS;
}

kern_return_t WKPCSetModal(mach_port_t clientPort, boolean_t modal)
{
    NetscapePluginHostProxy* hostProxy = pluginProxyMap().get(clientPort);
    if (!hostProxy)
        return KERN_FAILURE;
    
    hostProxy->setModal(modal);
    
    return KERN_SUCCESS;
}

#endif // USE(PLUGIN_HOST_PROCESS)
