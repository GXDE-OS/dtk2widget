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

#include <QMenu>
#include <QAction>
#include <QWindow>
#include <QSurfaceFormat>
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QStyleFactory>
#include <QStyleOption>
#include <QApplication>
#include <QTimer>
#include <QImage>
#include <QDebug>

#include "dmenueffect.h"
#include "dkwinblur.h"
#include "ddeshellmanager.h"
#include "dapplication.h"

QT_BEGIN_NAMESPACE
void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality,
    bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

namespace {

const int kRadius = 8;
const int kShadowMargin = 18;
const int kShadowBlur = 7;
const int kShadowOffsetY = 3;

bool menuDebug() {
    static const bool on = qEnvironmentVariableIsSet("DTK_MENU_DEBUG");
    return on;
}

bool menuBlurEnabled() {
    return qgetenv("DTK_MENU_BLUR") != "0";
}

qreal menuAlpha() {
    bool ok = false;
    const double v = qEnvironmentVariable("DTK_MENU_ALPHA").toDouble(&ok);
    return ok ? v : 0.45;
}

bool menuBlurActive() {
    return menuBlurEnabled() && DKWinBlurManager::instance()->isValid();
}

QRegion roundedRegion(const QRect& rect, int radius) {
    if (radius <= 0 || rect.isEmpty()) {
        return QRegion(rect);
    }

    QRegion region(rect.adjusted(radius, 0, -radius, 0));
    region += QRegion(rect.adjusted(0, radius, 0, -radius));

    const int d = radius * 2;
    region += QRegion(rect.x(), rect.y(), d, d, QRegion::Ellipse)
        & QRect(rect.x(), rect.y(), radius, radius);
    region += QRegion(rect.right() - d + 1, rect.y(), d, d, QRegion::Ellipse)
        & QRect(rect.right() - radius + 1, rect.y(), radius, radius);
    region += QRegion(rect.x(), rect.bottom() - d + 1, d, d, QRegion::Ellipse)
        & QRect(rect.x(), rect.bottom() - radius + 1, radius, radius);
    region += QRegion(rect.right() - d + 1, rect.bottom() - d + 1, d, d,
        QRegion::Ellipse)
        & QRect(rect.right() - radius + 1, rect.bottom() - radius + 1,
            radius, radius);
    return region;
}

const char* kInstalledProperty = "_dtk_menu_effect_installed";

QMenu* parentMenuOf(QMenu* menu) {
    if (!menu) {
        return nullptr;
    }

    if (QMenu* pm = qobject_cast<QMenu*>(menu->parentWidget())) {
        return pm;
    }

    if (QAction* act = menu->menuAction()) {
        const QList<QWidget *> widgets = act->associatedWidgets();
        for (QWidget* w : widgets) {
            if (QMenu* pm = qobject_cast<QMenu*>(w)) {
                return pm;
            }
        }
    }

    return nullptr;
}

}  // namespace

DMenuProxyStyle::DMenuProxyStyle(QStyle* baseStyle) : QProxyStyle(baseStyle) {}

void DMenuProxyStyle::drawPrimitive(PrimitiveElement element,
        const QStyleOption* option, QPainter* painter,
        const QWidget* widget) const {
    if (element == PE_PanelMenu) {
        drawMenuDecoration(option, painter, widget);
        return;
    }

    if (element == PE_FrameMenu) {
        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void DMenuProxyStyle::drawMenuDecoration(const QStyleOption* option,
        QPainter* painter, const QWidget* widget) const {
    Q_UNUSED(option)

    if (!widget) {
        return;
    }

    const QMargins sm = widget->contentsMargins();
    const QRect content(sm.left(), sm.top(),
        widget->width() - sm.left() - sm.right(),
        widget->height() - sm.top() - sm.bottom());
    QPainterPath bgPath;
    bgPath.addRoundedRect(content, kRadius, kRadius);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(widget->rect(), Qt::transparent);
    painter->restore();

    // 只有留出了投影空间才画投影
    if (!sm.isNull()) {
        QImage shadow(widget->size(), QImage::Format_ARGB32_Premultiplied);
        shadow.fill(Qt::transparent);
        {
            QPainter sp(&shadow);
            sp.setRenderHint(QPainter::Antialiasing);
            sp.fillPath(bgPath.translated(0, kShadowOffsetY), QColor(0, 0, 0));
        }

        painter->save();
        QPainterPath outside;
        outside.addRect(widget->rect());
        outside = outside.subtracted(bgPath);
        painter->setClipPath(outside);
        painter->setOpacity(0.18);
        qt_blurImage(painter, shadow, kShadowBlur * 2.0, true, true);
        painter->restore();
    }

    const bool blurOn = menuBlurActive();
    QColor bgColor = widget->palette().color(QPalette::Window);
    if (blurOn)
        bgColor.setAlphaF(menuAlpha());
    painter->fillPath(bgPath, bgColor);
    painter->strokePath(bgPath, QPen(QColor(0, 0, 0, 20), 1));

    painter->setClipPath(bgPath);
}

void DMenuEffect::install(QMenu* menu) {
    if (!menu || !DApplication::isWayland()) {
        return;
    }

    if (menu->property(kInstalledProperty).toBool()) {
        return;
    }

    menu->setProperty(kInstalledProperty, true);

    // 菜单弹出窗口在这个窗管下会被当成普通 toplevel 加上服务端标题栏，
    // 和主窗口一样告诉窗管不要画
    DDdeShellManager::watchWindow(menu);

    menu->setAttribute(Qt::WA_TranslucentBackground);
    if (QWindow* win = menu->windowHandle()) {
        QSurfaceFormat fmt = win->requestedFormat();
        if (fmt.alphaBufferSize() <= 0) {
            fmt.setAlphaBufferSize(8);
            win->setFormat(fmt);
        }
    }

    menu->setContentsMargins(kShadowMargin, kShadowMargin, kShadowMargin,
        kShadowMargin);

    QStyle* base = QStyleFactory::create(QStringLiteral("dlight"));
    DMenuProxyStyle* proxy = new DMenuProxyStyle(base);
    proxy->setParent(menu);
    menu->setStyle(proxy);

    new DMenuEffect(menu);
}

DMenuEffect::DMenuEffect(QMenu* menu) : QObject(menu), m_menu(menu) {
    m_menu->installEventFilter(this);
}

bool DMenuEffect::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_menu) {
        switch (event->type()) {
        case QEvent::Show:
            alignPanelToAnchor();
            QTimer::singleShot(0, this, [this]() {
                if (m_menu)
                    m_menu->update();
                updateBlur();
            });
            break;
        case QEvent::Move:
            maybeFlipSubmenu();
            QTimer::singleShot(0, this, [this]() { updateBlur(); });
            break;
        case QEvent::Resize:
            QTimer::singleShot(0, this, [this]() { updateBlur(); });
            break;
        case QEvent::Hide:
            clearBlur();
            break;
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}

/*!
 * \~chinese 菜单被撑大了 kShadowMargin，直接弹出来会整体偏移，这里把窗口挪回去，
 * \~chinese 让真正的菜单面板对准原本的弹出位置。
 */
void DMenuEffect::alignPanelToAnchor() {
    if (!m_menu)
        return;

    m_flipTried = false;

    const QMargins sm = m_menu->contentsMargins();
    QPoint winPos = m_menu->pos();  // 弹出窗口的 pos() 即全局坐标

    if (QMenu* pm = parentMenuOf(m_menu)) {
        const QMargins pmm = pm->contentsMargins();
        const int parentLeft = pm->mapToGlobal(QPoint(0, 0)).x();
        const bool opensLeft = winPos.x() < parentLeft;

        if (opensLeft) {
            const int parentPanelLeft = parentLeft + pmm.left();
            winPos.setX(parentPanelLeft - m_menu->width() + sm.right());
        } else {
            const int parentPanelRight = parentLeft + pm->width() - pmm.right();
            winPos.setX(parentPanelRight - sm.left());
        }
    } else {
        winPos -= QPoint(sm.left(), sm.top());
    }

    // 记在 move() 之前：move() 会同步派发 QEvent::Move，maybeFlipSubmenu() 要靠
    // 「实到位置 vs 请求位置」来判断是不是被合成器拽回来了。
    m_requestedPos = winPos;
    m_menu->move(winPos);
}

void DMenuEffect::maybeFlipSubmenu() {
    if (!m_menu || m_flipTried)
        return;

    QMenu* pm = parentMenuOf(m_menu);
    if (!pm)
        return;

    if (m_menu->pos().x() >= m_requestedPos.x())
        return;

    m_flipTried = true;

    const QMargins sm = m_menu->contentsMargins();
    const QMargins pmm = pm->contentsMargins();
    const int parentPanelLeft = pm->mapToGlobal(QPoint(0, 0)).x() + pmm.left();

    const QPoint flipped(parentPanelLeft - m_menu->width() + sm.right(),
        m_requestedPos.y());

    if (menuDebug()) {
        qDebug() << "(DMenu) Submenu constrained, flipping to parent's left:"
                 << m_requestedPos << "->" << flipped;
    }

    m_requestedPos = flipped;
    m_menu->move(flipped);
}

void DMenuEffect::updateBlur() {
    if (!m_menu || !m_menu->isVisible()) {
        return;
    }

    QWindow* window = m_menu->windowHandle();
    if (!window) {
        return;
    }

    if (!menuBlurEnabled()) {
        clearBlur();
        return;
    }

    const QMargins sm = m_menu->contentsMargins();
    const QRect panel(sm.left(), sm.top(),
        m_menu->width() - sm.left() - sm.right(),
        m_menu->height() - sm.top() - sm.bottom());

    if (panel.isEmpty()) {
        return;
    }

    const QRegion region = roundedRegion(panel, kRadius);

    if (menuDebug())
        qDebug() << "(DMenu) Blur updated: " << m_menu->size() << panel;

    DKWinBlurManager::instance()->setBlur(window, region);
}

void DMenuEffect::clearBlur() {
    if (!m_menu) {
        return;
    }

    if (QWindow* window = m_menu->windowHandle()) {
        DKWinBlurManager::instance()->clearBlur(window);
    }
}

DWIDGET_END_NAMESPACE
