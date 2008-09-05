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

#include "config.h"
#include "AnimationBase.h"
#include "AnimationController.h"
#include "KeyframeAnimation.h"
#include "ImplicitAnimation.h"
#include "CompositeAnimation.h"

#include "CSSPropertyNames.h"
#include "CString.h"
#include "Document.h"
#include "EventNames.h"
#include "FloatConversion.h"
#include "Frame.h"
#include "RenderObject.h"
#include "RenderStyle.h"
#include "SystemTime.h"
#include "UnitBezier.h"

namespace WebCore {

static const double cAnimationTimerDelay = 0.025;

// The epsilon value we pass to UnitBezier::solve given that the animation is going to run over |dur| seconds. The longer the
// animation, the more precision we need in the timing function result to avoid ugly discontinuities.
static inline double solveEpsilon(double duration) { return 1. / (200. * duration); }

static inline double solveCubicBezierFunction(double p1x, double p1y, double p2x, double p2y, double t, double duration)
{
    // Convert from input time to parametric value in curve, then from
    // that to output time.
    UnitBezier bezier(p1x, p1y, p2x, p2y);
    return bezier.solve(t, solveEpsilon(duration));
}

void AnimationTimerCallback::timerFired(Timer<AnimationTimerBase>*)
{
    m_anim->animationTimerCallbackFired(m_eventType, m_elapsedTime);
}

AnimationEventDispatcher::AnimationEventDispatcher(AnimationBase* anim) 
: AnimationTimerBase(anim)
, m_property(CSSPropertyInvalid)
, m_reset(false)
, m_elapsedTime(-1)
{
}

void AnimationEventDispatcher::timerFired(Timer<AnimationTimerBase>*)
{
    m_anim->animationEventDispatcherFired(m_element.get(), m_name, m_property, m_reset, m_eventType, m_elapsedTime);
}

static inline int blendFunc(int from, int to, double progress)
{  
    return int(from + (to - from) * progress);
}

static inline double blendFunc(double from, double to, double progress)
{  
    return from + (to - from) * progress;
}

static inline float blendFunc(float from, float to, double progress)
{  
    return narrowPrecisionToFloat(from + (to - from) * progress);
}

static inline Color blendFunc(const Color& from, const Color& to, double progress)
{  
    return Color(blendFunc(from.red(), to.red(), progress),
                 blendFunc(from.green(), to.green(), progress),
                 blendFunc(from.blue(), to.blue(), progress),
                 blendFunc(from.alpha(), to.alpha(), progress));
}

static inline Length blendFunc(const Length& from, const Length& to, double progress)
{  
    return to.blend(from, progress);
}

static inline IntSize blendFunc(const IntSize& from, const IntSize& to, double progress)
{  
    return IntSize(blendFunc(from.width(), to.width(), progress),
                   blendFunc(from.height(), to.height(), progress));
}

static inline ShadowData* blendFunc(const ShadowData* from, const ShadowData* to, double progress)
{  
    ASSERT(from && to);
    return new ShadowData(blendFunc(from->x, to->x, progress), blendFunc(from->y, to->y, progress), 
                          blendFunc(from->blur, to->blur, progress), blendFunc(from->color, to->color, progress));
}

static inline TransformOperations blendFunc(const TransformOperations& from, const TransformOperations& to, double progress)
{    
    // Blend any operations whose types actually match up.  Otherwise don't bother.
    unsigned fromSize = from.size();
    unsigned toSize = to.size();
    unsigned size = max(fromSize, toSize);
    TransformOperations result;
    for (unsigned i = 0; i < size; i++) {
        RefPtr<TransformOperation> fromOp = i < fromSize ? from[i] : 0;
        RefPtr<TransformOperation> toOp = i < toSize ? to[i] : 0;
        RefPtr<TransformOperation> blendedOp = toOp ? toOp->blend(fromOp.get(), progress) : fromOp->blend(0, progress, true);
        if (blendedOp)
            result.append(blendedOp);
    }
    return result;
}

static inline EVisibility blendFunc(EVisibility from, EVisibility to, double progress)
{
    // Any non-zero result means we consider the object to be visible.  Only at 0 do we consider the object to be
    // invisible.   The invisible value we use (HIDDEN vs. COLLAPSE) depends on the specified from/to values.
    double fromVal = from == VISIBLE ? 1. : 0.;
    double toVal = to == VISIBLE ? 1. : 0.;
    if (fromVal == toVal)
        return to;
    double result = blendFunc(fromVal, toVal, progress);
    return result > 0. ? VISIBLE : (to != VISIBLE ? to : from);
}

class PropertyWrapperBase {
public:
    PropertyWrapperBase(int prop)
    : m_prop(prop)
    { }
    virtual ~PropertyWrapperBase() { }
    virtual bool equals(const RenderStyle* a, const RenderStyle* b) const=0;
    virtual void blend(RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog) const=0;
    
    int property() const { return m_prop; }
    
private:
    int m_prop;
};

template <typename T>
class PropertyWrapperGetter : public PropertyWrapperBase {
public:
    PropertyWrapperGetter(int prop, T (RenderStyle::*getter)() const)
    : PropertyWrapperBase(prop)
    , m_getter(getter)
    { }
    
    virtual bool equals(const RenderStyle* a, const RenderStyle* b) const
    {
        return (a->*m_getter)() == (b->*m_getter)();
    }
    
protected:
    T (RenderStyle::*m_getter)() const;
};

template <typename T>
class PropertyWrapper : public PropertyWrapperGetter<T> {
public:
    PropertyWrapper(int prop, T (RenderStyle::*getter)() const, void (RenderStyle::*setter)(T))
    : PropertyWrapperGetter<T>(prop, getter)
    , m_setter(setter)
    { }
    
    virtual void blend(RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog) const
    {
        (dst->*m_setter)(blendFunc((a->*PropertyWrapperGetter<T>::m_getter)(), (b->*PropertyWrapperGetter<T>::m_getter)(), prog));
    }
    
protected:
    void (RenderStyle::*m_setter)(T);
};

class PropertyWrapperShadow : public PropertyWrapperGetter<ShadowData*> {
public:
    PropertyWrapperShadow(int prop, ShadowData* (RenderStyle::*getter)() const, void (RenderStyle::*setter)(ShadowData*, bool))
    : PropertyWrapperGetter<ShadowData*>(prop, getter)
    , m_setter(setter)
    { }
    
    virtual bool equals(const RenderStyle* a, const RenderStyle* b) const
    {
        ShadowData* shadowa = (a->*m_getter)();
        ShadowData* shadowb = (b->*m_getter)();
        
        if (!shadowa && shadowb || shadowa && !shadowb)
            return false;
        if (shadowa && shadowb && (*shadowa != *shadowb))
            return false;
        return true;
    }
    
    virtual void blend(RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog) const
    {
        ShadowData* shadowa = (a->*m_getter)();
        ShadowData* shadowb = (b->*m_getter)();
        ShadowData defaultShadowData(0, 0, 0, Color::transparent);
        
        if (!shadowa)
            shadowa = &defaultShadowData;
        if (!shadowb)
            shadowb = &defaultShadowData;
        
        (dst->*m_setter)(blendFunc(shadowa, shadowb, prog), false);
    }
    
private:
    void (RenderStyle::*m_setter)(ShadowData*, bool);
};

class PropertyWrapperMaybeInvalidColor : public PropertyWrapperBase {
public:
    PropertyWrapperMaybeInvalidColor(int prop, const Color& (RenderStyle::*getter)() const, void (RenderStyle::*setter)(const Color&))
    : PropertyWrapperBase(prop)
    , m_getter(getter)
    , m_setter(setter)
    { }
    
    virtual bool equals(const RenderStyle* a, const RenderStyle* b) const
    {
        Color fromColor = (a->*m_getter)();
        Color toColor = (b->*m_getter)();
        if (!fromColor.isValid())
            fromColor = a->color();
        if (!toColor.isValid())
            toColor = b->color();
        
        return fromColor == toColor;
    }
    
    virtual void blend(RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog) const
    {
        Color fromColor = (a->*m_getter)();
        Color toColor = (b->*m_getter)();
        if (!fromColor.isValid())
            fromColor = a->color();
        if (!toColor.isValid())
            toColor = b->color();
        (dst->*m_setter)(blendFunc(fromColor, toColor, prog));
    }
    
private:
    const Color& (RenderStyle::*m_getter)() const;
    void (RenderStyle::*m_setter)(const Color&);
};

Vector<PropertyWrapperBase*>* gPropertyWrappers = 0;
int gPropertyWrapperMap[numCSSProperties];

static void ensurePropertyMap()
{
    // FIXME: This data is never destroyed. Maybe we should ref count it and toss it when the last AnimationController is destroyed?
    if (gPropertyWrappers == 0) {
        gPropertyWrappers = new Vector<PropertyWrapperBase*>();
        
        // build the list of property wrappers to do the comparisons and blends
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyLeft, &RenderStyle::left, &RenderStyle::setLeft));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyRight, &RenderStyle::right, &RenderStyle::setRight));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyTop, &RenderStyle::top, &RenderStyle::setTop));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyBottom, &RenderStyle::bottom, &RenderStyle::setBottom));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyWidth, &RenderStyle::width, &RenderStyle::setWidth));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyHeight, &RenderStyle::height, &RenderStyle::setHeight));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyBorderLeftWidth, &RenderStyle::borderLeftWidth, &RenderStyle::setBorderLeftWidth));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyBorderRightWidth, &RenderStyle::borderRightWidth, &RenderStyle::setBorderRightWidth));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyBorderTopWidth, &RenderStyle::borderTopWidth, &RenderStyle::setBorderTopWidth));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyBorderBottomWidth, &RenderStyle::borderBottomWidth, &RenderStyle::setBorderBottomWidth));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyMarginLeft, &RenderStyle::marginLeft, &RenderStyle::setMarginLeft));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyMarginRight, &RenderStyle::marginRight, &RenderStyle::setMarginRight));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyMarginTop, &RenderStyle::marginTop, &RenderStyle::setMarginTop));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyMarginBottom, &RenderStyle::marginBottom, &RenderStyle::setMarginBottom));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyPaddingLeft, &RenderStyle::paddingLeft, &RenderStyle::setPaddingLeft));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyPaddingRight, &RenderStyle::paddingRight, &RenderStyle::setPaddingRight));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyPaddingTop, &RenderStyle::paddingTop, &RenderStyle::setPaddingTop));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyPaddingBottom, &RenderStyle::paddingBottom, &RenderStyle::setPaddingBottom));
        gPropertyWrappers->append(new PropertyWrapper<float>(CSSPropertyOpacity, &RenderStyle::opacity, &RenderStyle::setOpacity));
        gPropertyWrappers->append(new PropertyWrapper<const Color&>(CSSPropertyColor, &RenderStyle::color, &RenderStyle::setColor));
        gPropertyWrappers->append(new PropertyWrapper<const Color&>(CSSPropertyBackgroundColor, &RenderStyle::backgroundColor, &RenderStyle::setBackgroundColor));
        gPropertyWrappers->append(new PropertyWrapper<int>(CSSPropertyFontSize, &RenderStyle::fontSize, &RenderStyle::setBlendedFontSize));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyWebkitColumnRuleWidth, &RenderStyle::columnRuleWidth, &RenderStyle::setColumnRuleWidth));
        gPropertyWrappers->append(new PropertyWrapper<float>(CSSPropertyWebkitColumnGap, &RenderStyle::columnGap, &RenderStyle::setColumnGap));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyWebkitColumnCount, &RenderStyle::columnCount, &RenderStyle::setColumnCount));
        gPropertyWrappers->append(new PropertyWrapper<float>(CSSPropertyWebkitColumnWidth, &RenderStyle::columnWidth, &RenderStyle::setColumnWidth));
        gPropertyWrappers->append(new PropertyWrapper<short>(CSSPropertyWebkitBorderHorizontalSpacing, &RenderStyle::horizontalBorderSpacing, &RenderStyle::setHorizontalBorderSpacing));
        gPropertyWrappers->append(new PropertyWrapper<short>(CSSPropertyWebkitBorderVerticalSpacing, &RenderStyle::verticalBorderSpacing, &RenderStyle::setVerticalBorderSpacing));
        gPropertyWrappers->append(new PropertyWrapper<int>(CSSPropertyZIndex, &RenderStyle::zIndex, &RenderStyle::setZIndex));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyLineHeight, &RenderStyle::lineHeight, &RenderStyle::setLineHeight));
        gPropertyWrappers->append(new PropertyWrapper<int>(CSSPropertyOutlineOffset, &RenderStyle::outlineOffset, &RenderStyle::setOutlineOffset));
        gPropertyWrappers->append(new PropertyWrapper<unsigned short>(CSSPropertyOutlineWidth, &RenderStyle::outlineWidth, &RenderStyle::setOutlineWidth));
        gPropertyWrappers->append(new PropertyWrapper<int>(CSSPropertyLetterSpacing, &RenderStyle::letterSpacing, &RenderStyle::setLetterSpacing));
        gPropertyWrappers->append(new PropertyWrapper<int>(CSSPropertyWordSpacing, &RenderStyle::wordSpacing, &RenderStyle::setWordSpacing));
        gPropertyWrappers->append(new PropertyWrapper<const TransformOperations&>(CSSPropertyWebkitTransform, &RenderStyle::transform, &RenderStyle::setTransform));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyWebkitTransformOriginX, &RenderStyle::transformOriginX, &RenderStyle::setTransformOriginX));
        gPropertyWrappers->append(new PropertyWrapper<Length>(CSSPropertyWebkitTransformOriginY, &RenderStyle::transformOriginY, &RenderStyle::setTransformOriginY));
        gPropertyWrappers->append(new PropertyWrapper<const IntSize&>(CSSPropertyWebkitBorderTopLeftRadius, &RenderStyle::borderTopLeftRadius, &RenderStyle::setBorderTopLeftRadius));
        gPropertyWrappers->append(new PropertyWrapper<const IntSize&>(CSSPropertyWebkitBorderTopRightRadius, &RenderStyle::borderTopRightRadius, &RenderStyle::setBorderTopRightRadius));
        gPropertyWrappers->append(new PropertyWrapper<const IntSize&>(CSSPropertyWebkitBorderBottomLeftRadius, &RenderStyle::borderBottomLeftRadius, &RenderStyle::setBorderBottomLeftRadius));
        gPropertyWrappers->append(new PropertyWrapper<const IntSize&>(CSSPropertyWebkitBorderBottomRightRadius, &RenderStyle::borderBottomRightRadius, &RenderStyle::setBorderBottomRightRadius));
        gPropertyWrappers->append(new PropertyWrapper<EVisibility>(CSSPropertyVisibility, &RenderStyle::visibility, &RenderStyle::setVisibility));
        gPropertyWrappers->append(new PropertyWrapper<float>(CSSPropertyZoom, &RenderStyle::zoom, &RenderStyle::setZoom));
        
        // FIXME: these might be invalid colors, need to check for that
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyWebkitColumnRuleColor, &RenderStyle::columnRuleColor, &RenderStyle::setColumnRuleColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyWebkitTextStrokeColor, &RenderStyle::textStrokeColor, &RenderStyle::setTextStrokeColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyWebkitTextFillColor, &RenderStyle::textFillColor, &RenderStyle::setTextFillColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyBorderLeftColor, &RenderStyle::borderLeftColor, &RenderStyle::setBorderLeftColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyBorderRightColor, &RenderStyle::borderRightColor, &RenderStyle::setBorderRightColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyBorderTopColor, &RenderStyle::borderTopColor, &RenderStyle::setBorderTopColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyBorderBottomColor, &RenderStyle::borderBottomColor, &RenderStyle::setBorderBottomColor));
        gPropertyWrappers->append(new PropertyWrapperMaybeInvalidColor(CSSPropertyOutlineColor, &RenderStyle::outlineColor, &RenderStyle::setOutlineColor));
        
        // These are for shadows
        gPropertyWrappers->append(new PropertyWrapperShadow(CSSPropertyWebkitBoxShadow, &RenderStyle::boxShadow, &RenderStyle::setBoxShadow));
        gPropertyWrappers->append(new PropertyWrapperShadow(CSSPropertyTextShadow, &RenderStyle::textShadow, &RenderStyle::setTextShadow));
        
        // Make sure unused slots have a value
        for (unsigned int i = 0; i < (unsigned int) numCSSProperties; ++i)
            gPropertyWrapperMap[i] = CSSPropertyInvalid;
        
        size_t n = gPropertyWrappers->size();
        for (unsigned int i = 0; i < n; ++i) {
            ASSERT((*gPropertyWrappers)[i]->property() - firstCSSProperty < numCSSProperties);
            gPropertyWrapperMap[(*gPropertyWrappers)[i]->property() - firstCSSProperty] = i;
        }
    }
}

AnimationBase::AnimationBase(const Animation* transition, RenderObject* renderer, CompositeAnimation* compAnim)
: m_animState(STATE_NEW)
, m_iteration(0)
, m_animating(false)
, m_waitedForResponse(false)
, m_startTime(0)
, m_pauseTime(-1)
, m_object(renderer)
, m_animationTimerCallback(const_cast<AnimationBase*>(this))
, m_animationEventDispatcher(const_cast<AnimationBase*>(this))
, m_animation(const_cast<Animation*>(transition))
, m_compAnim(compAnim)
, m_waitingForEndEvent(false)
{
    ensurePropertyMap();
}

AnimationBase::~AnimationBase()
{
    if (m_animState == STATE_START_WAIT_STYLE_AVAILABLE)
        m_compAnim->setWaitingForStyleAvailable(false);
}
// static
bool AnimationBase::propertiesEqual(int prop, const RenderStyle* a, const RenderStyle* b)
{
    if (prop == cAnimateAll) {
        size_t n = gPropertyWrappers->size();
        for (unsigned int i = 0; i < n; ++i) {
            if (!(*gPropertyWrappers)[i]->equals(a, b))
                return false;
        }
    }
    else {
        int propIndex = prop - firstCSSProperty;
        
        if (propIndex >= 0 && propIndex < numCSSProperties) {
            int i = gPropertyWrapperMap[propIndex];
            return (i >= 0) ? (*gPropertyWrappers)[i]->equals(a, b) : true;
        }
    }
    return true;
}

// static
int AnimationBase::getPropertyAtIndex(int i)
{
    if (i < 0 || i >= (int) gPropertyWrappers->size())
        return CSSPropertyInvalid;
        
    return (*gPropertyWrappers)[i]->property();
}

//static
int AnimationBase::getNumProperties()
{
    return gPropertyWrappers->size();
}

    
// static - return true if we need to start software animation timers
bool AnimationBase::blendProperties(int prop, RenderStyle* dst, const RenderStyle* a, const RenderStyle* b, double prog)
{
    if (prop == cAnimateAll) {
        ASSERT(0);
        bool needsTimer = false;
    
        size_t n = gPropertyWrappers->size();
        for (unsigned int i = 0; i < n; ++i) {
            PropertyWrapperBase* wrapper = (*gPropertyWrappers)[i];
            if (!wrapper->equals(a, b)) {
                wrapper->blend(dst, a, b, prog);
                needsTimer = true;
            }
        }
        return needsTimer;
    }
    
    int propIndex = prop - firstCSSProperty;
    if (propIndex >= 0 && propIndex < numCSSProperties) {
        int i = gPropertyWrapperMap[propIndex];
        if (i >= 0) {
            PropertyWrapperBase* wrapper = (*gPropertyWrappers)[i];
            wrapper->blend(dst, a, b, prog);
            return true;
        }
    }
    
    return false;
}

void AnimationBase::setChanged(Node* node)
{
    ASSERT(!node || (node->document() && !node->document()->inPageCache()));
    node->setChanged(AnimationStyleChange);
}

double AnimationBase::duration() const
{
    return m_animation->duration();
}

bool AnimationBase::playStatePlaying() const
{
    return m_animation && m_animation->playState() == AnimPlayStatePlaying;
}

bool AnimationBase::animationsMatch(const Animation* anim) const
{
    return m_animation->animationsMatch(anim);
}

Element* AnimationBase::elementForEventDispatch()
{
    if (m_object->node() && m_object->node()->isElementNode())
        return static_cast<Element*>(m_object->node());
    return 0;
}
    
void AnimationBase::updateStateMachine(AnimStateInput input, double param)
{
    // If we get a RESTART then we force a new animation, regardless of state
    if (input == STATE_INPUT_MAKE_NEW) {
        if (m_animState == STATE_START_WAIT_STYLE_AVAILABLE)
            m_compAnim->setWaitingForStyleAvailable(false);
        m_animState = STATE_NEW;
        m_startTime = 0;
        m_pauseTime = -1;
        m_waitedForResponse = false;
        endAnimation(false);
        return;
    }
    else if (input == STATE_INPUT_RESTART_ANIMATION) {
        cancelTimers();
        if (m_animState == STATE_START_WAIT_STYLE_AVAILABLE)
            m_compAnim->setWaitingForStyleAvailable(false);
        m_animState = STATE_NEW;
        m_startTime = 0;
        m_pauseTime = -1;
        endAnimation(false);
        
        if (!paused())
            updateStateMachine(STATE_INPUT_START_ANIMATION, -1);
        return;
    }
    else if (input == STATE_INPUT_END_ANIMATION) {
        cancelTimers();
        if (m_animState == STATE_START_WAIT_STYLE_AVAILABLE)
            m_compAnim->setWaitingForStyleAvailable(false);
        m_animState = STATE_DONE;
        endAnimation(true);
        return;
    }
    else if (input == STATE_INPUT_PAUSE_OVERRIDE) {
        if (m_animState == STATE_START_WAIT_RESPONSE) {
            // If we are in the WAIT_RESPONSE state, the animation will get canceled before 
            // we get a response, so move to the next state
            endAnimation(false);
            updateStateMachine(STATE_INPUT_START_TIME_SET, currentTime());
        }
        return;
    }
    else if (input == STATE_INPUT_RESUME_OVERRIDE) {
        if (m_animState == STATE_LOOPING || m_animState == STATE_ENDING) {
            // Start the animation
            startAnimation(m_startTime);
        }
        return;
    }
    
    // Execute state machine
    switch(m_animState) {
        case STATE_NEW:
            ASSERT(input == STATE_INPUT_START_ANIMATION || input == STATE_INPUT_PLAY_STATE_RUNNING || input == STATE_INPUT_PLAY_STATE_PAUSED);
            if (input == STATE_INPUT_START_ANIMATION || input == STATE_INPUT_PLAY_STATE_RUNNING) {
                // Set the start timer to the initial delay (0 if no delay)
                m_waitedForResponse = false;
                m_animState = STATE_START_WAIT_TIMER;
                m_animationTimerCallback.startTimer(m_animation->delay(), EventNames::webkitAnimationStartEvent, m_animation->delay());
            }
            break;
        case STATE_START_WAIT_TIMER:
            ASSERT(input == STATE_INPUT_START_TIMER_FIRED || input == STATE_INPUT_PLAY_STATE_PAUSED);
            
            if (input == STATE_INPUT_START_TIMER_FIRED) {
                ASSERT(param >= 0);
                // Start timer has fired, tell the animation to start and wait for it to respond with start time
                m_animState = STATE_START_WAIT_STYLE_AVAILABLE;
                m_compAnim->setWaitingForStyleAvailable(true);
                
                // Trigger a render so we can start the animation
                setChanged(m_object->element());
                m_object->animation()->startUpdateRenderingDispatcher();
            }
            else {
                ASSERT(running());
                // We're waiting for the start timer to fire and we got a pause. Cancel the timer, pause and wait
                m_pauseTime = currentTime();
                cancelTimers();
                m_animState = STATE_PAUSED_WAIT_TIMER;
            }
            break;
        case STATE_START_WAIT_STYLE_AVAILABLE:
            ASSERT(input == STATE_INPUT_STYLE_AVAILABLE || input == STATE_INPUT_PLAY_STATE_PAUSED);
            
            m_compAnim->setWaitingForStyleAvailable(false);
            
            if (input == STATE_INPUT_STYLE_AVAILABLE) {
                // Start timer has fired, tell the animation to start and wait for it to respond with start time
                m_animState = STATE_START_WAIT_RESPONSE;
                
                overrideAnimations();
                
                // Send start event, if needed
                onAnimationStart(0.0f); // The elapsedTime is always 0 here
                
                // Start the animation
                if (overridden() || !startAnimation(0)) {
                    // We're not going to get a startTime callback, so fire the start time here
                    m_animState = STATE_START_WAIT_RESPONSE;
                    updateStateMachine(STATE_INPUT_START_TIME_SET, currentTime());
                }
                else
                    m_waitedForResponse = true;
            }
            else {
                ASSERT(running());
                // We're waiting for the a notification that the style has been setup. If we're asked to wait
                // at this point, the style must have been processed, so we can deal with this like we would
                // for WAIT_RESPONSE, except that we don't need to do an endAnimation().
                m_pauseTime = 0;
                m_animState = STATE_START_WAIT_RESPONSE;
            }
            break;
        case STATE_START_WAIT_RESPONSE:
            ASSERT(input == STATE_INPUT_START_TIME_SET || input == STATE_INPUT_PLAY_STATE_PAUSED);
            
            if (input == STATE_INPUT_START_TIME_SET) {
                ASSERT(param >= 0);
                // We have a start time, set it, unless the startTime is already set
                if (m_startTime <= 0)
                    m_startTime = param;
                
                // Decide when the end or loop event needs to fire
                primeEventTimers();
                
                // Trigger a render so we can start the animation
                setChanged(m_object->element());
                m_object->animation()->startUpdateRenderingDispatcher();
            }
            else {
                // We are pausing while waiting for a start response. Cancel the animation and wait. When 
                // we unpause, we will act as though the start timer just fired
                m_pauseTime = 0;
                endAnimation(false);
                m_animState = STATE_PAUSED_WAIT_RESPONSE;
            }
            break;
        case STATE_LOOPING:
            ASSERT(input == STATE_INPUT_LOOP_TIMER_FIRED || input == STATE_INPUT_PLAY_STATE_PAUSED);
            
            if (input == STATE_INPUT_LOOP_TIMER_FIRED) {
                ASSERT(param >= 0);
                // Loop timer fired, loop again or end.
                onAnimationIteration(param);
                primeEventTimers();
            }
            else {
                // We are pausing while running. Cancel the animation and wait
                m_pauseTime = currentTime();
                cancelTimers();
                endAnimation(false);
                m_animState = STATE_PAUSED_RUN;
            }
            break;
        case STATE_ENDING:
            ASSERT(input == STATE_INPUT_END_TIMER_FIRED || input == STATE_INPUT_PLAY_STATE_PAUSED);
            
            if (input == STATE_INPUT_END_TIMER_FIRED) {
                ASSERT(param >= 0);
                // End timer fired, finish up
                onAnimationEnd(param);
                
                resumeOverriddenAnimations();
                
                // Fire off another style change so we can set the final value
                setChanged(m_object->element());
                m_animState = STATE_DONE;
                m_object->animation()->startUpdateRenderingDispatcher();
                // |this| may be deleted here when we've been called from timerFired()
            }
            else {
                // We are pausing while running. Cancel the animation and wait
                m_pauseTime = currentTime();
                cancelTimers();
                endAnimation(false);
                m_animState = STATE_PAUSED_RUN;
            }
            // |this| may be deleted here
            break;
        case STATE_PAUSED_WAIT_TIMER:
            ASSERT(input == STATE_INPUT_PLAY_STATE_RUNNING);
            ASSERT(!running());
            // Update the times
            m_startTime += currentTime() - m_pauseTime;
            m_pauseTime = -1;
            
            // we were waiting for the start timer to fire, go back and wait again
            m_animState = STATE_NEW;
            updateStateMachine(STATE_INPUT_START_ANIMATION, 0);
            break;
        case STATE_PAUSED_WAIT_RESPONSE:
        case STATE_PAUSED_RUN:
            // We treat these two cases the same. The only difference is that, when we are in the WAIT_RESPONSE
            // state, we don't yet have a valid startTime, so we send 0 to startAnimation. When the START_TIME
            // event comes in qnd we were in the RUN state, we will notice that we have already set the 
            // startTime and will ignore it.
            ASSERT(input == STATE_INPUT_PLAY_STATE_RUNNING);
            ASSERT(!running());
            // Update the times
            if (m_animState == STATE_PAUSED_RUN)
                m_startTime += currentTime() - m_pauseTime;
            else
                m_startTime = 0;
            m_pauseTime = -1;
            
            // We were waiting for a begin time response from the animation, go back and wait again
            m_animState = STATE_START_WAIT_RESPONSE;
            
            // Start the animation
            if (overridden() || !startAnimation(m_startTime)) {
                // We're not going to get a startTime callback, so fire the start time here
                updateStateMachine(STATE_INPUT_START_TIME_SET, currentTime());
            }
            else
                m_waitedForResponse = true;
            break;
        case STATE_DONE:
            // We're done. Stay in this state until we are deleted
            break;
    }
    // |this| may be deleted here if we came out of STATE_ENDING when we've been called from timerFired()
}
    
void AnimationBase::animationTimerCallbackFired(const AtomicString& eventType, double elapsedTime)
{
    ASSERT(m_object->document() && !m_object->document()->inPageCache());
    
    // FIXME: use an enum
    if (eventType == EventNames::webkitAnimationStartEvent)
        updateStateMachine(STATE_INPUT_START_TIMER_FIRED, elapsedTime);
    else if (eventType == EventNames::webkitAnimationIterationEvent)
        updateStateMachine(STATE_INPUT_LOOP_TIMER_FIRED, elapsedTime);
    else if (eventType == EventNames::webkitAnimationEndEvent) {
        updateStateMachine(STATE_INPUT_END_TIMER_FIRED, elapsedTime);
        // |this| may be deleted here
    }
}

void AnimationBase::animationEventDispatcherFired(Element* element, const AtomicString& name, int property,
                                                  bool reset, const AtomicString& eventType, double elapsedTime)
{
    m_waitingForEndEvent = false;
    
    // Keep an atomic string on the stack to keep it alive until we exit this method
    // (since dispatching the event may cause |this| to be deleted, therefore removing
    // the last ref to the atomic string).
    AtomicString animName(name);
    AtomicString animEventType(eventType);
    // Make sure the element sticks around too
    RefPtr<Element> elementRetainer(element);
    
    ASSERT(!element || (element->document() && !element->document()->inPageCache()));
    if (!element)
        return;

    if (eventType == EventNames::webkitTransitionEndEvent)
        element->dispatchWebKitTransitionEvent(eventType, name, elapsedTime);
    else
        element->dispatchWebKitAnimationEvent(eventType, name, elapsedTime);

    // Restore the original (unanimated) style
    if (animEventType == EventNames::webkitAnimationEndEvent)
        if (element->renderer())
            setChanged(element->renderer()->element());
}

void AnimationBase::updatePlayState(bool run)
{
    if (paused() == run || isNew())
        updateStateMachine(run ? STATE_INPUT_PLAY_STATE_RUNNING : STATE_INPUT_PLAY_STATE_PAUSED, -1);
}

double AnimationBase::progress(double scale, double offset) const
{
    if (preActive())
        return 0;
    
    double elapsedTime = running() ? (currentTime() - m_startTime) : (m_pauseTime - m_startTime);
    if (running() && elapsedTime < 0)
        return 0;
    
    double dur = m_animation->duration();
    if (m_animation->iterationCount() > 0)
        dur *= m_animation->iterationCount();
    
    if (postActive() || !m_animation->duration() || (m_animation->iterationCount() > 0 && elapsedTime >= dur))
        return 1.0;
    
    // Compute the fractional time, taking into account direction.
    // There is no need to worry about iterations, we assume that we would have
    // short circuited above if we were done
    double fractionalTime = elapsedTime / m_animation->duration();
    int integralTime = (int) fractionalTime;
    fractionalTime -= integralTime;
    
    if (m_animation->direction() && (integralTime & 1))
        fractionalTime = 1 - fractionalTime;
    
    if (scale != 1 || offset != 0)
        fractionalTime = (fractionalTime - offset) * scale;
    
    if (m_animation->timingFunction().type() == LinearTimingFunction)
        return fractionalTime;
    
    // Cubic bezier.
    double result = solveCubicBezierFunction(m_animation->timingFunction().x1(),
                                            m_animation->timingFunction().y1(),
                                            m_animation->timingFunction().x2(),
                                            m_animation->timingFunction().y2(),
                                            fractionalTime, m_animation->duration());
    return result;
}

void AnimationBase::primeEventTimers()
{
    // Decide when the end or loop event needs to fire
    double ct = currentTime();
    const double elapsedDuration = ct - m_startTime;
    ASSERT(elapsedDuration >= 0);
    
    double totalDuration = -1;
    if (m_animation->iterationCount() > 0)
        totalDuration = m_animation->duration() * m_animation->iterationCount();

    double durationLeft = 0;
    double nextIterationTime = totalDuration;
    if (totalDuration < 0 || elapsedDuration < totalDuration) {
        durationLeft = m_animation->duration() - fmod(elapsedDuration, m_animation->duration());
        nextIterationTime = elapsedDuration + durationLeft;
    }
    
    // At this point, we may have 0 durationLeft, if we've gotten the event late and we are already
    // past totalDuration. In this case we still fire an end timer before processing the end. 
    // This defers the call to sendAnimationEvents to avoid re-entrant calls that destroy
    // the RenderObject, and therefore |this| before we're done with it.
    if (totalDuration < 0 || nextIterationTime < totalDuration) {
        // We are not at the end yet, send a loop event
        ASSERT(nextIterationTime > 0);
        m_animState = STATE_LOOPING;
        m_animationTimerCallback.startTimer(durationLeft, EventNames::webkitAnimationIterationEvent, nextIterationTime);
    }
    else {
        // We are at the end, send an end event
        m_animState = STATE_ENDING;
        m_animationTimerCallback.startTimer(durationLeft, EventNames::webkitAnimationEndEvent, nextIterationTime);
    }
}
  
}