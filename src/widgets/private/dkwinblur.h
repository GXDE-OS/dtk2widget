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
 */

#ifndef DKWINBLUR_H
#define DKWINBLUR_H

#include <QObject>
#include <QHash>
#include <QRegion>

#include "dtkwidget_global.h"

struct wl_display;
struct wl_registry;
struct org_kde_kwin_blur_manager;
struct org_kde_kwin_blur;

QT_BEGIN_NAMESPACE
class QWindow;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

/*!
 * \~chinese \class DKWinBlurManager
 * \~chinese \brief 通过 org_kde_kwin_blur 协议让窗管模糊窗口背景。
 *
 * \~chinese DBlurEffectWidget 的 BehindWindowBlend 原本走的是
 * \~chinese DPlatformWindowHandle::setWindowBlurAreaByWM，那是 dxcb 平台插件提供的
 * \~chinese 接口，Wayland 下取不到，所以窗口背景不会被模糊。此处改用窗管提供的
 * \~chinese org_kde_kwin_blur 协议实现同样的效果。
 */
class DKWinBlurManager : public QObject {
    Q_OBJECT

public:
    static DKWinBlurManager* instance();

    // 窗管是否提供了 org_kde_kwin_blur_manager
    bool isValid();

    void setBlur(QWindow* window, const QRegion& region);
    void clearBlur(QWindow* window);

private:
    explicit DKWinBlurManager(QObject* parent = nullptr);
    ~DKWinBlurManager() override;

    bool ensureManager();
    void handleGlobal(wl_registry* registry, uint32_t name,
        const char* interface, uint32_t version);
    void releaseBlur(QWindow* window);

    static void registry_global(void* data, wl_registry* registry,
        uint32_t name, const char* interface, uint32_t version);
    static void registry_global_remove(void* data, wl_registry* registry,
        uint32_t name);

    wl_display* m_display = nullptr;
    wl_registry* m_registry = nullptr;
    org_kde_kwin_blur_manager* m_manager = nullptr;
    bool m_tried = false;
    QHash<QWindow *, org_kde_kwin_blur *> m_blurs;
};

DWIDGET_END_NAMESPACE

#endif  // DKWINBLUR_H
