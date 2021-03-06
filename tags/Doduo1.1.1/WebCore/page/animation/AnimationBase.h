/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AnimationBase_h
#define AnimationBase_h

#include "Timer.h"
#include "wtf/HashMap.h"
#include "AtomicString.h"

namespace WebCore {

class AnimationController;
class AnimationBase;
class ImplicitAnimation;
class KeyframeAnimation;
class CompositeAnimation;
class RenderObject;
class RenderStyle;
class AtomicStringImpl;
class Animation;
class Element;
class Node;

class AnimationTimerBase {
public:
    AnimationTimerBase(AnimationBase* anim)
    : m_timer(this, &AnimationTimerBase::timerFired)
    , m_anim(anim)
    {
        m_timer.startOneShot(0);
    }
    virtual ~AnimationTimerBase()
    {
    }
    
    void startTimer(double timeout=0)
    {
        m_timer.startOneShot(timeout);
    }
    
    void cancelTimer()
    {
        m_timer.stop();
    }
    
    virtual void timerFired(Timer<AnimationTimerBase>*) = 0;
    
private:
    Timer<AnimationTimerBase> m_timer;
    
protected:
    AnimationBase* m_anim;
};

class AnimationEventDispatcher : public AnimationTimerBase {
public:
    AnimationEventDispatcher(AnimationBase* anim);    
    virtual ~AnimationEventDispatcher()
    {
    }
    
    void startTimer(Element* element, const AtomicString& name, int property, bool reset,
                    const AtomicString& eventType, double elapsedTime)
    {
        m_element = element;
        m_name = name;
        m_property = property;
        m_reset = reset;
        m_eventType = eventType;
        m_elapsedTime = elapsedTime;
        AnimationTimerBase::startTimer();
    }
    
    virtual void timerFired(Timer<AnimationTimerBase>*);
    
private:
    RefPtr<Element> m_element;
    AtomicString m_name;
    int m_property;
    bool m_reset;
    AtomicString m_eventType;
    double m_elapsedTime;
};

class AnimationTimerCallback : public AnimationTimerBase {
public:
    AnimationTimerCallback(AnimationBase* anim) 
    : AnimationTimerBase(anim)
    , m_elapsedTime(0)
    { }
    virtual ~AnimationTimerCallback() { }
    
    virtual void timerFired(Timer<AnimationTimerBase>*);
    
    void startTimer(double timeout, const AtomicString& eventType, double elapsedTime)
    {
        m_eventType = eventType;
        m_elapsedTime = elapsedTime;
        AnimationTimerBase::startTimer(timeout);
    }
    
private:
    AtomicString m_eventType;
    double m_elapsedTime;
};

class AnimationBase : public Noncopyable {
    friend class CompositeAnimation;
    
public:
    AnimationBase(const Animation* transition, RenderObject* renderer, CompositeAnimation* compAnim);
    virtual ~AnimationBase();
        
    RenderObject* renderer() const { return m_object; }
    double startTime() const { return m_startTime; }
    double duration() const;
    
    void cancelTimers()
    {
        m_animationTimerCallback.cancelTimer();
        m_animationEventDispatcher.cancelTimer();
    }
    
    // Animations and Transitions go through the states below. When entering the STARTED state
    // the animation is started. This may or may not require deferred response from the animator.
    // If so, we stay in this state until that response is received (and it returns the start time).
    // Otherwise, we use the current time as the start time and go immediately to the LOOPING or
    // ENDING state.
    enum AnimState { 
        STATE_NEW,                      // animation just created, animation not running yet
        STATE_START_WAIT_TIMER,         // start timer running, waiting for fire
        STATE_START_WAIT_STYLE_AVAILABLE,   // waiting for style setup so we can start animations
        STATE_START_WAIT_RESPONSE,      // animation started, waiting for response
        STATE_LOOPING,                  // response received, animation running, loop timer running, waiting for fire
        STATE_ENDING,                   // response received, animation running, end timer running, waiting for fire
        STATE_PAUSED_WAIT_TIMER,        // animation in pause mode when animation started
        STATE_PAUSED_WAIT_RESPONSE,     // animation paused when in STARTING state
        STATE_PAUSED_RUN,               // animation paused when in LOOPING or ENDING state
        STATE_DONE                      // end timer fired, animation finished and removed
    };
    
    enum AnimStateInput {
        STATE_INPUT_MAKE_NEW,           // reset back to new from any state
        STATE_INPUT_START_ANIMATION,    // animation requests a start
        STATE_INPUT_RESTART_ANIMATION,  // force a restart from any state
        STATE_INPUT_START_TIMER_FIRED,  // start timer fired
        STATE_INPUT_STYLE_AVAILABLE,        // style is setup, ready to start animating
        STATE_INPUT_START_TIME_SET,     // m_startTime was set
        STATE_INPUT_LOOP_TIMER_FIRED,   // loop timer fired
        STATE_INPUT_END_TIMER_FIRED,    // end timer fired
        STATE_INPUT_PAUSE_OVERRIDE,     // pause an animation due to override
        STATE_INPUT_RESUME_OVERRIDE,    // resume an overridden animation
        STATE_INPUT_PLAY_STATE_RUNNING, // play state paused -> running
        STATE_INPUT_PLAY_STATE_PAUSED,  // play state running -> paused
        STATE_INPUT_END_ANIMATION       // force an end from any state
    };
    
    // Called when animation is in NEW state to start animation
    void updateStateMachine(AnimStateInput input, double param);
    
    // Animation has actually started, at passed time
    void onAnimationStartResponse(double startTime);
    
    // Called to change to or from paused state
    void updatePlayState(bool running);
    bool playStatePlaying() const;
    
    bool waitingToStart() const { return m_animState == STATE_NEW || m_animState == STATE_START_WAIT_TIMER; }
    bool preActive() const
    { return m_animState == STATE_NEW ||
             m_animState == STATE_START_WAIT_TIMER ||
             m_animState == STATE_START_WAIT_STYLE_AVAILABLE ||
             m_animState == STATE_START_WAIT_RESPONSE;
    }
    bool postActive() const { return m_animState == STATE_DONE; }
    bool active() const { return !postActive() && !preActive(); }
    bool running() const { return !isNew() && !postActive(); }
    bool paused() const { return m_pauseTime >= 0; }
    bool isNew() const { return m_animState == STATE_NEW; }
    bool waitingForStartTime() const { return m_animState == STATE_START_WAIT_RESPONSE; }
    bool waitingForStyleAvailable() const { return m_animState == STATE_START_WAIT_STYLE_AVAILABLE; }
    bool waitingForEndEvent() const  { return m_waitingForEndEvent; }
    
    // "animating" means that something is running that requires a timer to keep firing
    // (e.g. a software animation)
    void setAnimating(bool inAnimating = true) { m_animating = inAnimating; }
    bool animating() const { return m_animating; }
    
    double progress(double scale, double offset) const;
    
    virtual void animate(CompositeAnimation*, RenderObject*, const RenderStyle* currentStyle, 
                         const RenderStyle* targetStyle, RenderStyle*& animatedStyle) { }
    virtual void reset(RenderObject* renderer, const RenderStyle* from = 0, const RenderStyle* to = 0) { }
    
    virtual bool shouldFireEvents() const { return false; }
    
    void animationTimerCallbackFired(const AtomicString& eventType, double elapsedTime);
    void animationEventDispatcherFired(Element* element, const AtomicString& name, int property, bool reset,
                                       const AtomicString& eventType, double elapsedTime);
    
    bool animationsMatch(const Animation* anim) const;
    
    void setAnimation(const Animation* anim) { m_animation = const_cast<Animation*>(anim); }
    
    // Return true if this animation is overridden. This will only be the case for
    // ImplicitAnimations and is used to determine whether or not we should force
    // set the start time. If an animation is overridden, it will probably not get
    // back the START_TIME event.
    virtual bool overridden() const { return false; }
    
    // Does this animation/transition involve the given property?
    virtual bool affectsProperty(int property) const { return false; }
    bool isAnimatingProperty(int property, bool isRunningNow) const
    {
        if (isRunningNow)
            return (!waitingToStart() && !postActive()) && affectsProperty(property);

        return !postActive() && affectsProperty(property);
    }
        
protected:
    Element* elementForEventDispatch();
    
    virtual void overrideAnimations() { }
    virtual void resumeOverriddenAnimations() { }
    
    CompositeAnimation* compositeAnimation() { return m_compAnim; }
    
    // These are called when the corresponding timer fires so subclasses can do any extra work
    virtual void onAnimationStart(double elapsedTime) { }
    virtual void onAnimationIteration(double elapsedTime) { }
    virtual void onAnimationEnd(double elapsedTime) { }
    virtual bool startAnimation(double beginTime) { return false; }
    virtual void endAnimation(bool reset) { }
    
    void primeEventTimers();
    
    static bool propertiesEqual(int prop, const RenderStyle* a, const RenderStyle* b);
    static int getPropertyAtIndex(int i);
    static int getNumProperties();
    
    // Return true if we need to start software animation timers
    static bool blendProperties(int prop, RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog);
    
    static void setChanged(Node* node);
    
protected:
    AnimState m_animState;
    int m_iteration;
    
    bool m_animating;       // transition/animation requires continual timer firing
    bool m_waitedForResponse;
    double m_startTime;
    double m_pauseTime;
    RenderObject* m_object;
    
    AnimationTimerCallback m_animationTimerCallback;
    AnimationEventDispatcher m_animationEventDispatcher;
    RefPtr<Animation> m_animation;
    CompositeAnimation* m_compAnim;
    bool m_waitingForEndEvent;

private:
};

}

#endif
