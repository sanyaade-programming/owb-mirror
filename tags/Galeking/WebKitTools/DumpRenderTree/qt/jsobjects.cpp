/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
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
#include <jsobjects.h>
#include <qwebpage.h>
#include <qwebhistory.h>
#include <qwebframe.h>
#include <qevent.h>
#include <qapplication.h>
#include <qevent.h>
#include <qtimer.h>

#include "DumpRenderTree.h"
#include "WorkQueueItem.h"
#include "WorkQueue.h"
extern void qt_dump_editing_callbacks(bool b);
extern void qt_dump_resource_load_callbacks(bool b);
extern void qt_drt_setJavaScriptProfilingEnabled(QWebFrame*, bool enabled);
extern bool qt_drt_pauseAnimation(QWebFrame*, const QString &name, double time, const QString &elementId);
extern bool qt_drt_pauseTransitionOfProperty(QWebFrame*, const QString &name, double time, const QString &elementId);
extern int qt_drt_numberOfActiveAnimations(QWebFrame*);

QWebFrame *findFrameNamed(const QString &frameName, QWebFrame *frame)
{
    if (frame->frameName() == frameName)
        return frame;

    foreach (QWebFrame *childFrame, frame->childFrames())
        if (QWebFrame *f = findFrameNamed(frameName, childFrame))
            return f;

    return 0;
}

void LoadItem::invoke() const
{
    //qDebug() << ">>>LoadItem::invoke";
    Q_ASSERT(m_webPage);

    QWebFrame *frame = 0;
    const QString t = target();
    if (t.isEmpty())
        frame = m_webPage->mainFrame();
    else
        frame = findFrameNamed(t, m_webPage->mainFrame());

    if (!frame)
        return;

    frame->load(url());
}

void ReloadItem::invoke() const
{
    //qDebug() << ">>>ReloadItem::invoke";
    Q_ASSERT(m_webPage);
    m_webPage->triggerAction(QWebPage::Reload);
}

void ScriptItem::invoke() const
{
    //qDebug() << ">>>ScriptItem::invoke";
    Q_ASSERT(m_webPage);
    m_webPage->mainFrame()->evaluateJavaScript(script());
}

void BackForwardItem::invoke() const
{
    //qDebug() << ">>>BackForwardItem::invoke";
    Q_ASSERT(m_webPage);
    if (m_howFar > 0) {
        for (int i = 0; i != m_howFar; ++i)
            m_webPage->triggerAction(QWebPage::Forward);
    } else {
        for (int i = 0; i != m_howFar; --i)
            m_webPage->triggerAction(QWebPage::Back);
    }
}

LayoutTestController::LayoutTestController(WebCore::DumpRenderTree *drt)
    : QObject()
    , m_drt(drt)
{
    m_timeoutTimer = 0;
    reset();
}

void LayoutTestController::reset()
{
    m_isLoading = true;
    m_textDump = false;
    m_dumpBackForwardList = false;
    m_dumpChildrenAsText = false;
    m_canOpenWindows = false;
    m_waitForDone = false;
    m_dumpTitleChanges = false;
    if (m_timeoutTimer) {
        killTimer(m_timeoutTimer);
        m_timeoutTimer = 0;
    }
    m_topLoadingFrame = 0;
    qt_dump_editing_callbacks(false);
    qt_dump_resource_load_callbacks(false);
}

void LayoutTestController::processWork()
{
    qDebug() << ">>>processWork";

    // quit doing work once a load is in progress
    while (WorkQueue::shared()->count() > 0 && !m_topLoadingFrame) {
        WorkQueueItem* item = WorkQueue::shared()->dequeue();
        Q_ASSERT(item);
        item->invoke();
        delete item;
    }

    // if we didn't start a new load, then we finished all the commands, so we're ready to dump state
    if (!m_topLoadingFrame && !shouldWaitUntilDone()) {
        emit done();
        m_isLoading = false;
    }
}

void LayoutTestController::maybeDump(bool)
{
    m_topLoadingFrame = 0;
    WorkQueue::shared()->setFrozen(true); // first complete load freezes the queue for the rest of this test

    if (!shouldWaitUntilDone()) {
        if (WorkQueue::shared()->count())
            QTimer::singleShot(0, this, SLOT(processWork()));
        else {
            emit done();
            m_isLoading = false;
        }
    }
}

void LayoutTestController::waitUntilDone()
{
    //qDebug() << ">>>>waitForDone";
    m_waitForDone = true;
    m_timeoutTimer = startTimer(11000);
}

void LayoutTestController::notifyDone()
{
    //qDebug() << ">>>>notifyDone";
    if (!m_timeoutTimer)
        return;
    killTimer(m_timeoutTimer);
    m_timeoutTimer = 0;
    emit done();
    m_isLoading = false;
}

int LayoutTestController::windowCount()
{
    return m_drt->windowCount();
}

void LayoutTestController::clearBackForwardList()
{
    m_drt->webPage()->history()->clear();
}

void LayoutTestController::dumpEditingCallbacks()
{
    qDebug() << ">>>dumpEditingCallbacks";
    qt_dump_editing_callbacks(true);
}

void LayoutTestController::dumpResourceLoadCallbacks()
{
    qt_dump_resource_load_callbacks(true);
}

void LayoutTestController::queueBackNavigation(int howFarBackward)
{
    //qDebug() << ">>>queueBackNavigation" << howFarBackward;
    WorkQueue::shared()->queue(new BackItem(howFarBackward, m_drt->webPage()));
}

void LayoutTestController::queueForwardNavigation(int howFarForward)
{
    //qDebug() << ">>>queueForwardNavigation" << howFarForward;
    WorkQueue::shared()->queue(new ForwardItem(howFarForward, m_drt->webPage()));
}

void LayoutTestController::queueLoad(const QString &url, const QString &target)
{
    //qDebug() << ">>>queueLoad" << url << target;
    QUrl mainResourceUrl = m_drt->webPage()->mainFrame()->url();
    QString absoluteUrl = mainResourceUrl.resolved(QUrl(url)).toEncoded();
    WorkQueue::shared()->queue(new LoadItem(absoluteUrl, target, m_drt->webPage()));
}

void LayoutTestController::queueReload()
{
    //qDebug() << ">>>queueReload";
    WorkQueue::shared()->queue(new ReloadItem(m_drt->webPage()));
}

void LayoutTestController::queueScript(const QString &url)
{
    //qDebug() << ">>>queueScript" << url;
    WorkQueue::shared()->queue(new ScriptItem(url, m_drt->webPage()));
}

void LayoutTestController::provisionalLoad()
{
    QWebFrame *frame = qobject_cast<QWebFrame*>(sender());
    if (!m_topLoadingFrame && m_isLoading)
        m_topLoadingFrame = frame;
}

void LayoutTestController::timerEvent(QTimerEvent *)
{
    qDebug() << ">>>>>>>>>>>>> timeout";
    notifyDone();
}

QString LayoutTestController::encodeHostName(const QString &host)
{
    QString encoded = QString::fromLatin1(QUrl::toAce(host + QLatin1String(".no")));
    encoded.truncate(encoded.length() - 3); // strip .no
    return encoded;
}

QString LayoutTestController::decodeHostName(const QString &host)
{
    QString decoded = QUrl::fromAce(host.toLatin1() + QByteArray(".no"));
    decoded.truncate(decoded.length() - 3);
    return decoded;
}

void LayoutTestController::setJavaScriptProfilingEnabled(bool enable)
{
    m_topLoadingFrame->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    qt_drt_setJavaScriptProfilingEnabled(m_topLoadingFrame, enable);
}

void LayoutTestController::setFixedLayoutSize(int width, int height)
{
    m_topLoadingFrame->page()->setFixedLayoutSize(QSize(width, height));
}


void LayoutTestController::setUseFixedLayout(bool enable)
{
    m_topLoadingFrame->page()->setUseFixedLayout(enable);
}

bool LayoutTestController::pauseAnimationAtTimeOnElementWithId(const QString &animationName,
                                                               double time,
                                                               const QString &elementId)
{
    QWebFrame *frame = m_drt->webPage()->mainFrame();
    Q_ASSERT(frame);
    return qt_drt_pauseAnimation(frame, animationName, time, elementId);
}

bool LayoutTestController::pauseTransitionAtTimeOnElementWithId(const QString &propertyName,
                                                                double time,
                                                                const QString &elementId)
{
    QWebFrame *frame = m_drt->webPage()->mainFrame();
    Q_ASSERT(frame);
    return qt_drt_pauseTransitionOfProperty(frame, propertyName, time, elementId);
}

unsigned LayoutTestController::numberOfActiveAnimations() const
{
    QWebFrame *frame = m_drt->webPage()->mainFrame();
    Q_ASSERT(frame);
    return qt_drt_numberOfActiveAnimations(frame);
}

EventSender::EventSender(QWebPage *parent)
    : QObject(parent)
{
    m_page = parent;
}

void EventSender::mouseDown()
{
//     qDebug() << "EventSender::mouseDown" << frame;
    QMouseEvent event(QEvent::MouseButtonPress, m_mousePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_page, &event);
}

void EventSender::mouseUp()
{
//     qDebug() << "EventSender::mouseUp" << frame;
    QMouseEvent event(QEvent::MouseButtonRelease, m_mousePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_page, &event);
}

void EventSender::mouseMoveTo(int x, int y)
{
//     qDebug() << "EventSender::mouseMoveTo" << x << y;
    m_mousePos = QPoint(x, y);
    QMouseEvent event(QEvent::MouseMove, m_mousePos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(m_page, &event);
}

void EventSender::leapForward(int ms)
{
    m_timeLeap += ms;
    qDebug() << "EventSender::leapForward" << ms;
}

void EventSender::keyDown(const QString &string, const QStringList &modifiers)
{
    QString s = string;
    Qt::KeyboardModifiers modifs = 0;
    for (int i = 0; i < modifiers.size(); ++i) {
        const QString &m = modifiers.at(i);
        if (m == "ctrlKey")
            modifs |= Qt::ControlModifier;
        else if (m == "shiftKey")
            modifs |= Qt::ShiftModifier;
        else if (m == "altKey")
            modifs |= Qt::AltModifier;
        else if (m == "metaKey")
            modifs |= Qt::MetaModifier;
    }
    int code = 0;
    if (string.length() == 1) {
        code = string.unicode()->unicode();
        qDebug() << ">>>>>>>>> keyDown" << code << (char)code;
        // map special keycodes used by the tests to something that works for Qt/X11
        if (code == '\t') {
            code = Qt::Key_Tab;
            if (modifs == Qt::ShiftModifier)
                code = Qt::Key_Backtab;
            s = QString();
        } else if (code == 127) {
            code = Qt::Key_Backspace;
            if (modifs == Qt::AltModifier)
                modifs = Qt::ControlModifier;
            s = QString();
        } else if (code == 'o' && modifs == Qt::ControlModifier) {
            s = QLatin1String("\n");
            code = '\n';
            modifs = 0;
        } else if (code == 'y' && modifs == Qt::ControlModifier) {
            s = QLatin1String("c");
            code = 'c';
        } else if (code == 'k' && modifs == Qt::ControlModifier) {
            s = QLatin1String("x");
            code = 'x';
        } else if (code == 'a' && modifs == Qt::ControlModifier) {
            s = QString();
            code = Qt::Key_Home;
            modifs = 0;
        } else if (code == 0xf702) {
            s = QString();
            code = Qt::Key_Left;
            if (modifs & Qt::MetaModifier) {
                code = Qt::Key_Home;
                modifs &= ~Qt::MetaModifier;
            }
        } else if (code == 0xf703) {
            s = QString();
            code = Qt::Key_Right;
            if (modifs & Qt::MetaModifier) {
                code = Qt::Key_End;
                modifs &= ~Qt::MetaModifier;
            }
        } else if (code == 0xf700) {
            s = QString();
            code = Qt::Key_Up;
            if (modifs & Qt::MetaModifier) {
                code = Qt::Key_PageUp;
                modifs &= ~Qt::MetaModifier;
            }
        } else if (code == 0xf701) {
            s = QString();
            code = Qt::Key_Down;
            if (modifs & Qt::MetaModifier) {
                code = Qt::Key_PageDown;
                modifs &= ~Qt::MetaModifier;
            }
        } else if (code == 'a' && modifs == Qt::ControlModifier) {
            s = QString();
            code = Qt::Key_Home;
            modifs = 0;
        } else {
            code = string.unicode()->toUpper().unicode();
        }
    }
    QKeyEvent event(QEvent::KeyPress, code, modifs, s);
    QApplication::sendEvent(m_page, &event);
}

QWebFrame *EventSender::frameUnderMouse() const
{
    QWebFrame *frame = m_page->mainFrame();

redo:
    QList<QWebFrame*> children = frame->childFrames();
    for (int i = 0; i < children.size(); ++i) {
        if (children.at(i)->geometry().contains(m_mousePos)) {
            frame = children.at(i);
            goto redo;
        }
    }
    if (frame->geometry().contains(m_mousePos))
        return frame;
    return 0;
}


TextInputController::TextInputController(QWebPage *parent)
    : QObject(parent)
{
}

void TextInputController::doCommand(const QString &command)
{
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    int keycode = 0;
    if (command == "moveBackwardAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Left;
    } else if(command =="moveDown:") {
        keycode = Qt::Key_Down;
    } else if(command =="moveDownAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Down;
    } else if(command =="moveForward:") {
        keycode = Qt::Key_Right;
    } else if(command =="moveForwardAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Right;
    } else if(command =="moveLeft:") {
        keycode = Qt::Key_Left;
    } else if(command =="moveLeftAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Left;
    } else if(command =="moveRight:") {
        keycode = Qt::Key_Right;
    } else if(command =="moveRightAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Right;
    } else if(command =="moveToBeginningOfDocument:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Home;
    } else if(command =="moveToBeginningOfLine:") {
        keycode = Qt::Key_Home;
//     } else if(command =="moveToBeginningOfParagraph:") {
    } else if(command =="moveToEndOfDocument:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_End;
    } else if(command =="moveToEndOfLine:") {
        keycode = Qt::Key_End;
//     } else if(command =="moveToEndOfParagraph:") {
    } else if(command =="moveUp:") {
        keycode = Qt::Key_Up;
    } else if(command =="moveUpAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Up;
    } else if(command =="moveWordBackward:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Up;
    } else if(command =="moveWordBackwardAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Left;
    } else if(command =="moveWordForward:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Right;
    } else if(command =="moveWordForwardAndModifySelection:") {
        modifiers |= Qt::ControlModifier;
        modifiers |= Qt::ShiftModifier;
        keycode = Qt::Key_Right;
    } else if(command =="moveWordLeft:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Left;
    } else if(command =="moveWordRight:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Left;
    } else if(command =="moveWordRightAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Right;
    } else if(command =="moveWordLeftAndModifySelection:") {
        modifiers |= Qt::ShiftModifier;
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Left;
    } else if(command =="pageDown:") {
        keycode = Qt::Key_PageDown;        
    } else if(command =="pageUp:") {
        keycode = Qt::Key_PageUp;        
    } else if(command == "deleteWordBackward:") {
        modifiers |= Qt::ControlModifier;
        keycode = Qt::Key_Backspace;
    } else if(command == "deleteBackward:") {
        keycode = Qt::Key_Backspace;
    } else if(command == "deleteForward:") {
        keycode = Qt::Key_Delete;
    }
    QKeyEvent event(QEvent::KeyPress, keycode, modifiers);
    QApplication::sendEvent(parent(), &event);
    QKeyEvent event2(QEvent::KeyRelease, keycode, modifiers);
    QApplication::sendEvent(parent(), &event2);
}
