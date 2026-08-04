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
 * The solution is from GXDE's fork of Deepin-Menu
 */

#ifndef DDESHELLMANAGER_H
#define DDESHELLMANAGER_H

#include <QObject>
#include <QHash>

#include "dtkwidget_global.h"

struct wl_display;
struct wl_registry;
struct dde_shell;
struct dde_shell_surface;

QT_BEGIN_NAMESPACE
class QWindow;
class QWidget;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

/*!
 * \~chinese \class DDdeShellManager
 * \~chinese \brief 通过 dde_shell 协议直接告诉窗管"这个窗口自己画标题栏"。
 *
 * \~chinese Wayland 下能阻止窗管画标题栏的只有 Qt::FramelessWindowHint，而
 * \~chinese QWidget::setWindowFlags 不是虚函数，已经编译好的程序调用它覆盖掉这个
 * \~chinese 标志时，库里拦不住（子类的虚表在编译时就定死了）。dde_shell 的
 * \~chinese NoTitleBar 属性直接作用于 wl_surface，不受 Qt 窗口标志影响，
 * \~chinese 因此对无需重新编译的老程序同样有效。
 */
class DDdeShellManager : public QObject {
    Q_OBJECT

public:
    static DDdeShellManager* instance();
    bool isValid();
    void setNoTitleBar(QWindow* window, bool noTitleBar);

    /*!
     * \~chinese \brief 给 window 装一个事件过滤器，在它的窗口创建后设置 NoTitleBar。
     *
     * \~chinese 用事件过滤器而不是重写虚函数：已经编译好的子类，其虚表在编译时就定死了，
     * \~chinese 后加到基类上的重写对它们不生效；而事件过滤器是 QCoreApplication 派发
     * \~chinese 事件时才查的，不受子类实现影响，所以老程序不用重新编译也能修好双标题栏。
     */
    static void watchWindow(QWidget* window);

private:
    explicit DDdeShellManager(QObject* parent = nullptr);
    ~DDdeShellManager() override;

    bool ensureManager();
    void handleGlobal(wl_registry* registry, uint32_t name,
        const char* interface, uint32_t version);
    dde_shell_surface *shellSurfaceFor(QWindow* window);
    static void registry_global(void* data, wl_registry* registry,
        uint32_t name, const char* interface, uint32_t version);
    static void registry_global_remove(void* data, wl_registry* registry,
        uint32_t name);

    wl_display* m_display = nullptr;
    wl_registry* m_registry = nullptr;
    dde_shell* m_manager = nullptr;
    bool m_tried = false;
    QHash<QWindow* , dde_shell_surface *> m_surfaces;
};

DWIDGET_END_NAMESPACE

#endif  // DDESHELLMANAGER_H
