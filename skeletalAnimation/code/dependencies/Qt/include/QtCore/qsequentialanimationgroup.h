/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSEQUENTIALANIMATIONGROUP_H
#define QSEQUENTIALANIMATIONGROUP_H

#include <QtCore/qanimationgroup.h>

QT_BEGIN_NAMESPACE


#ifndef QT_NO_ANIMATION

class QPauseAnimation;
class QSequentialAnimationGroupPrivate;

class Q_CORE_EXPORT QSequentialAnimationGroup : public QAnimationGroup
{
    Q_OBJECT
    Q_PROPERTY(QAbstractAnimation* currentAnimation READ currentAnimation NOTIFY currentAnimationChanged)

public:
    QSequentialAnimationGroup(QObject *parent = Q_NULLPTR);
    ~QSequentialAnimationGroup();

    QPauseAnimation *addPause(int msecs);
    QPauseAnimation *insertPause(int index, int msecs);

    QAbstractAnimation *currentAnimation() const;
    int duration() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void currentAnimationChanged(QAbstractAnimation *current);

protected:
    QSequentialAnimationGroup(QSequentialAnimationGroupPrivate &dd, QObject *parent);
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    void updateCurrentTime(int) Q_DECL_OVERRIDE;
    void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState) Q_DECL_OVERRIDE;
    void updateDirection(QAbstractAnimation::Direction direction) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QSequentialAnimationGroup)
    Q_DECLARE_PRIVATE(QSequentialAnimationGroup)
    Q_PRIVATE_SLOT(d_func(), void _q_uncontrolledAnimationFinished())
};

#endif //QT_NO_ANIMATION

QT_END_NAMESPACE

#endif //QSEQUENTIALANIMATIONGROUP_H
