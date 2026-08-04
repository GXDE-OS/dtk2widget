/*
 * Copyright (C) 2026 CharOfString <markus_verify@126.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 * This class is exported from deepin-menu.
 */

#ifndef DMENUEFFECT_H
#define DMENUEFFECT_H

#include <QObject>
#include <QPoint>
#include <QProxyStyle>

#include "dtkwidget_global.h"

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

/*!
 * \~chinese \class DMenuProxyStyle
 * \~chinese \brief 自己画菜单的圆角背景和投影。
 *
 * \~chinese X11 下菜单的圆角、投影、模糊都由 dxcb 平台插件提供（_d_windowRadius、
 * \~chinese _d_shadowRadius、_d_enableBlurWindow 等窗口属性），Wayland 下没有 dxcb，
 * \~chinese 这些全部落空，菜单会退化成直角、无阴影、无模糊的原生 popup，所以这里
 * \~chinese 接管 PE_PanelMenu 自己画。
 */
class DMenuProxyStyle : public QProxyStyle {
    Q_OBJECT

public:
    explicit DMenuProxyStyle(QStyle* baseStyle);
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
        QPainter* painter, const QWidget* widget = nullptr) const override;

private:
    void drawMenuDecoration(const QStyleOption* option, QPainter* painter,
        const QWidget* widget) const;
};

/*!
 * \~chinese \class DMenuEffect
 * \~chinese \brief 给菜单加上投影留白、圆角背景和窗管背景模糊。
 */
class DMenuEffect : public QObject {
    Q_OBJECT

public:
    static void install(QMenu* menu);

    explicit DMenuEffect(QMenu* menu);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void alignPanelToAnchor();
    void maybeFlipSubmenu();
    void updateBlur();
    void clearBlur();

    QMenu* m_menu = nullptr;
    QPoint m_requestedPos;
    bool m_flipTried = false;
};

DWIDGET_END_NAMESPACE

#endif  // DMENUEFFECT_H
