// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCOMMANDLINKBUTTON_H
#define DCOMMANDLINKBUTTON_H

#include "qstyleoption.h"
#include <dtkwidget_global.h>
#include <QAbstractButton>

DWIDGET_BEGIN_NAMESPACE

class DCommandLinkButton : public QAbstractButton
{
    Q_OBJECT

public:
    explicit DCommandLinkButton(const QString &text, QWidget *parent = nullptr);

    QSize sizeHint() const override;

protected:
    void initStyleOption(QStyleOptionButton *option) const;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;


};

DWIDGET_END_NAMESPACE

#endif // DCOMMANDLINKBUTTON_H
