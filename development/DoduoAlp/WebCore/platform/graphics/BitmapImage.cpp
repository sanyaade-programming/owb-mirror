/*
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2004, 2005, 2006, 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "BitmapImage.h"

#include "FloatRect.h"
#include "ImageObserver.h"
#include "IntRect.h"
#include "PlatformString.h"
#include "SystemTime.h"
#include "Timer.h"
#include <wtf/Vector.h>
#include "MIMETypeRegistry.h"

namespace WebCore {

// Animated images >5MB are considered large enough that we'll only hang on to
// one frame at a time.
const unsigned cLargeAnimationCutoff = 5242880;

// When an animated image is more than five minutes out of date, don't try to
// resync on repaint, so we don't waste CPU cycles on an edge case the user
// doesn't care about.
const double cAnimationResyncCutoff = 5 * 60;

BitmapImage::BitmapImage(ImageObserver* observer)
    : Image(observer)
    , m_currentFrame(0)
    , m_frames(0)
    , m_frameTimer(0)
    , m_repetitionCount(0)
    , m_repetitionsComplete(0)
    , m_desiredFrameStartTime(0)
    , m_isSolidColor(false)
    , m_animatingImageType(true)
    , m_animationFinished(false)
    , m_allDataReceived(false)
    , m_haveSize(false)
    , m_sizeAvailable(false)
    , m_hasUniformFrameSize(true)
    , m_decodedSize(0)
    , m_haveFrameCount(false)
    , m_frameCount(0)
{
    initPlatformData();
}

BitmapImage::~BitmapImage()
{
    invalidatePlatformData();
    stopAnimation();
}

void BitmapImage::destroyDecodedData(bool incremental)
{
    // Destroy the cached images and release them.
    if (m_frames.size()) {
        int sizeChange = 0;
        int frameSize = m_size.width() * m_size.height() * 4;
        for (unsigned i = incremental ? m_frames.size() - 1 : 0; i < m_frames.size(); i++) {
            if (m_frames[i].m_frame) {
                sizeChange -= frameSize;
                m_frames[i].clear();
            }
        }

        // We just always invalidate our platform data, even in the incremental case.
        // This could be better, but it's not a big deal.
        m_isSolidColor = false;
        invalidatePlatformData();
        
        if (sizeChange) {
            m_decodedSize += sizeChange;
            if (imageObserver())
                imageObserver()->decodedSizeChanged(this, sizeChange);
        }
        
        if (!incremental) {
            // Reset the image source, since Image I/O has an underlying cache that it uses
            // while animating that it seems to never clear.
            m_source.clear();
            m_source.setData(m_data.get(), m_allDataReceived);
        }
    }
}

void BitmapImage::cacheFrame(size_t index)
{
    size_t numFrames = frameCount();
    ASSERT(m_decodedSize == 0 || numFrames > 1);
    
    if (!m_frames.size() && shouldAnimate()) {            
        // Snag the repetition count.  Note that the repetition count may not be
        // accurate yet for GIFs; if we haven't gotten the count from the source
        // image yet, it will default to cAnimationLoopOnce, and we'll try and
        // read it again once the whole image is decoded.
        m_repetitionCount = m_source.repetitionCount();
        if (m_repetitionCount == cAnimationNone)
            m_animatingImageType = false;
    }
    
    if (m_frames.size() < numFrames)
        m_frames.grow(numFrames);

    m_frames[index].m_frame = m_source.createFrameAtIndex(index);
    if (numFrames == 1 && m_frames[index].m_frame)
        checkForSolidColor();

    if (shouldAnimate())
        m_frames[index].m_duration = m_source.frameDurationAtIndex(index);
    m_frames[index].m_hasAlpha = m_source.frameHasAlphaAtIndex(index);

    int sizeChange;
    if (index) {
        IntSize frameSize = m_source.frameSizeAtIndex(index);
        if (frameSize != m_size)
            m_hasUniformFrameSize = false;
        sizeChange = m_frames[index].m_frame ? frameSize.width() * frameSize.height() * 4 : 0;
    } else
        sizeChange = m_frames[index].m_frame ? m_size.width() * m_size.height() * 4 : 0;

    if (sizeChange) {
        m_decodedSize += sizeChange;
        if (imageObserver())
            imageObserver()->decodedSizeChanged(this, sizeChange);
    }
}

IntSize BitmapImage::size() const
{
    if (m_sizeAvailable && !m_haveSize) {
        m_size = m_source.size();
        m_haveSize = true;
    }
    return m_size;
}

IntSize BitmapImage::currentFrameSize() const
{
    if (!m_currentFrame || m_hasUniformFrameSize)
        return size();
    return m_source.frameSizeAtIndex(m_currentFrame);
}

bool BitmapImage::dataChanged(bool allDataReceived)
{
    destroyDecodedData(true);
    
    // Feed all the data we've seen so far to the image decoder.
    m_allDataReceived = allDataReceived;
    m_source.setData(m_data.get(), allDataReceived);
    
    // Clear the frame count.
    m_haveFrameCount = false;

    m_hasUniformFrameSize = true;

    // Image properties will not be available until the first frame of the file
    // reaches kCGImageStatusIncomplete.
    return isSizeAvailable();
}

size_t BitmapImage::frameCount()
{
    if (!m_haveFrameCount) {
        m_haveFrameCount = true;
        m_frameCount = m_source.frameCount();
    }
    return m_frameCount;
}

bool BitmapImage::isSizeAvailable()
{
    if (m_sizeAvailable)
        return true;

    m_sizeAvailable = m_source.isSizeAvailable();

    return m_sizeAvailable;
}

NativeImagePtr BitmapImage::frameAtIndex(size_t index)
{
    if (index >= frameCount())
        return 0;

    if (index >= m_frames.size() || !m_frames[index].m_frame)
        cacheFrame(index);

    return m_frames[index].m_frame;
}

float BitmapImage::frameDurationAtIndex(size_t index)
{
    if (index >= frameCount())
        return 0;

    if (index >= m_frames.size() || !m_frames[index].m_frame)
        cacheFrame(index);

    return m_frames[index].m_duration;
}

bool BitmapImage::frameHasAlphaAtIndex(size_t index)
{
    if (index >= frameCount())
        return true;

    if (index >= m_frames.size() || !m_frames[index].m_frame)
        cacheFrame(index);

    return m_frames[index].m_hasAlpha;
}

bool BitmapImage::shouldAnimate()
{
    return (m_animatingImageType && !m_animationFinished && imageObserver());
}

void BitmapImage::startAnimation()
{
    if (m_frameTimer || !shouldAnimate() || frameCount() <= 1)
        return;

    // Don't advance the animation until the current frame has completely loaded.
    if (!m_source.frameIsCompleteAtIndex(m_currentFrame))
        return;

    // Don't advance past the last frame if we haven't decoded the whole image
    // yet and our repetition count is potentially unset.  The repetition count
    // in a GIF can potentially come after all the rest of the image data, so
    // wait on it.
    if (!m_allDataReceived && m_repetitionCount == cAnimationLoopOnce && m_currentFrame >= (frameCount() - 1))
        return;

    // Determine time for next frame to start.  By ignoring paint and timer lag
    // in this calculation, we make the animation appear to run at its desired
    // rate regardless of how fast it's being repainted.
    const double currentDuration = frameDurationAtIndex(m_currentFrame);
    const double time = currentTime();
    if (!m_desiredFrameStartTime)
        m_desiredFrameStartTime = time + currentDuration;
    else {
        m_desiredFrameStartTime += currentDuration;
        // If we're too far behind, the user probably doesn't care about
        // resyncing and we could burn a lot of time looping through frames
        // below.  Just reset the timings.
        if ((time - m_desiredFrameStartTime) > cAnimationResyncCutoff)
            m_desiredFrameStartTime = time + currentDuration;
    }

    if (time < m_desiredFrameStartTime) {
        // Haven't yet reached time for next frame to start; delay until then.
        m_frameTimer = new Timer<BitmapImage>(this, &BitmapImage::advanceAnimation);
        m_frameTimer->startOneShot(m_desiredFrameStartTime - time);
    } else {
        // We've already reached or passed the time for the next frame to start.
        // See if we've also passed the time for frames after that to start, in
        // case we need to skip some frames entirely.
        size_t nextFrame = (m_currentFrame + 1) % frameCount();
        while (m_source.frameIsCompleteAtIndex(nextFrame)) {
            // Should we skip the current frame?
            double nextFrameStartTime = m_desiredFrameStartTime + frameDurationAtIndex(nextFrame);
            if (time < nextFrameStartTime)
                break;

            // Yes; skip over it without notifying our observers.
            if (!internalAdvanceAnimation(true))
                return;
            m_desiredFrameStartTime = nextFrameStartTime;
            nextFrame = (nextFrame + 1) % frameCount();
        }

        // Draw the next frame immediately.  Note that m_desiredFrameStartTime
        // may be in the past, meaning the next time through this function we'll
        // kick off the next advancement sooner than this frame's duration would
        // suggest.
        internalAdvanceAnimation(false);
    }
}

void BitmapImage::stopAnimation()
{
    // This timer is used to animate all occurrences of this image.  Don't invalidate
    // the timer unless all renderers have stopped drawing.
    delete m_frameTimer;
    m_frameTimer = 0;
}

void BitmapImage::resetAnimation()
{
    stopAnimation();
    m_currentFrame = 0;
    m_repetitionsComplete = 0;
    m_animationFinished = false;
    int frameSize = m_size.width() * m_size.height() * 4;
    
    // For extremely large animations, when the animation is reset, we just throw everything away.
    if (frameCount() * frameSize > cLargeAnimationCutoff)
        destroyDecodedData();
}

void BitmapImage::advanceAnimation(Timer<BitmapImage>* timer)
{
    internalAdvanceAnimation(false);
}

bool BitmapImage::internalAdvanceAnimation(bool skippingFrames)
{
    // Stop the animation.
    stopAnimation();
    
    // See if anyone is still paying attention to this animation.  If not, we don't
    // advance and will remain suspended at the current frame until the animation is resumed.
    if (!skippingFrames && imageObserver()->shouldPauseAnimation(this))
        return false;

    m_currentFrame++;
    if (m_currentFrame >= frameCount()) {
        m_repetitionsComplete += 1;
        // Get the repetition count again.  If we weren't able to get a
        // repetition count before, we should have decoded the whole image by
        // now, so it should now be available.
        m_repetitionCount = m_source.repetitionCount();
        if (m_repetitionCount && m_repetitionsComplete >= m_repetitionCount) {
            m_animationFinished = true;
            m_desiredFrameStartTime = 0;
            m_currentFrame--;
            if (skippingFrames) {
                // Uh oh.  We tried to skip past the end of the animation.  We'd
                // better draw this last frame.
                notifyObserverAndTrimDecodedData();
            }
            return false;
        }
        m_currentFrame = 0;
    }

    if (!skippingFrames)
        notifyObserverAndTrimDecodedData();

    // We do not advance the animation explicitly.  We rely on a subsequent draw of the image
    // to force a request for the next frame via startAnimation().  This allows images that move offscreen while
    // scrolling to stop animating (thus saving memory from additional decoded frames and
    // CPU time spent doing the decoding).
    return true;
}

void BitmapImage::notifyObserverAndTrimDecodedData()
{
    // Notify our observer that the animation has advanced.
    imageObserver()->animationAdvanced(this);

    // For large animated images, go ahead and throw away frames as we go to save
    // footprint.
    int frameSize = m_size.width() * m_size.height() * 4;
    if (frameCount() * frameSize > cLargeAnimationCutoff) {
        // Destroy all of our frames and just redecode every time.
        destroyDecodedData();

        // Go ahead and decode the next frame.
        frameAtIndex(m_currentFrame);
    }
}

}
