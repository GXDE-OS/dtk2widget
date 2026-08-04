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

#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QEvent>
#include <QDebug>

#include <qpa/qplatformnativeinterface.h>
#include <wayland-client.h>

#include "dde-shell-client-protocol.h"
#include "ddeshellmanager.h"

DWIDGET_BEGIN_NAMESPACE

namespace {

bool shellDebug() {
    static const bool on = qEnvironmentVariableIsSet("DTK_WINDOW_DEBUG");
    return on;
}

/*!
 * \~chinese 只有真正跑在 wayland 平台插件上时才能去拿 wl_display。
 * \~chinese DApplication::isWayland() 在 XWayland 会话下也返回 true，此时平台插件
 * \~chinese 其实是 xcb，"display" 拿到的是 X11 的 Display*，误当成 wl_display 用会崩溃。
 */
bool onWaylandPlatform() {
    return qGuiApp && qGuiApp->platformName().contains(QLatin1String("wayland"));
}

void* nativeIntegrationResource(const QByteArray& name) {
    if (QPlatformNativeInterface* native_int =
            QGuiApplication::platformNativeInterface()) {
        return native_int->nativeResourceForIntegration(name);
    }

    return nullptr;
}

wl_surface* surfaceForWindow(QWindow* window) {
    if (!window) {
        return nullptr;
    }

    if (QPlatformNativeInterface* native_int =
            QGuiApplication::platformNativeInterface()) {
        return static_cast<wl_surface *>(native_int->nativeResourceForWindow(
            "surface", window));
    }
    return nullptr;
}

class DNoTitleBarFilter : public QObject {
public:
    explicit DNoTitleBarFilter(QWidget* window)
        : QObject(window), m_window(window) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (watched == m_window && !m_applied
            && (event->type() == QEvent::Show
                || event->type() == QEvent::PlatformSurface)) {
            if (QWindow* handle = m_window->windowHandle()) {
                m_applied = true;
                DDdeShellManager::instance()->setNoTitleBar(handle, true);
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QWidget* m_window = nullptr;
    bool m_applied = false;
};

}  // namespace

void DDdeShellManager::watchWindow(QWidget* window) {
    if (window) {
        window->installEventFilter(new DNoTitleBarFilter(window));
    }
}

DDdeShellManager* DDdeShellManager::instance() {
    static DDdeShellManager* self = new DDdeShellManager(qGuiApp);
    return self;
}

DDdeShellManager::DDdeShellManager(QObject* parent) : QObject(parent) {}

DDdeShellManager::~DDdeShellManager() {
    const QList<dde_shell_surface *> surfaces = m_surfaces.values();
    for (dde_shell_surface* s : surfaces) {
        if (s) {
            dde_shell_surface_destroy(s);
        }
    }
    m_surfaces.clear();
}

void DDdeShellManager::registry_global(void* data, wl_registry* registry,
        uint32_t name, const char* interface, uint32_t version) {
    static_cast<DDdeShellManager *>(data)->handleGlobal(registry, name,
        interface, version);
}

void DDdeShellManager::registry_global_remove(void*, wl_registry*, uint32_t) {}

void DDdeShellManager::handleGlobal(wl_registry* registry, uint32_t name,
        const char* interface, uint32_t /*version*/) {
    if (qstrcmp(interface, "dde_shell") == 0) {
        m_manager = static_cast<dde_shell *>(
            wl_registry_bind(registry, name, &dde_shell_interface, 2));
    }
}

bool DDdeShellManager::ensureManager() {
    if (m_tried) {
        return m_manager != nullptr;
    }

    m_tried = true;

    if (!onWaylandPlatform()) {
        return false;
    }

    m_display = static_cast<wl_display *>(nativeIntegrationResource("display"));
    if (!m_display) {
        return false;
    }

    m_registry = wl_display_get_registry(m_display);
    if (!m_registry) {
        return false;
    }

    static const wl_registry_listener listener = {
        registry_global,
        registry_global_remove,
    };

    wl_registry_add_listener(m_registry, &listener, this);
    wl_display_roundtrip(m_display);

    if (shellDebug())
        qDebug() << "(DWindow) Bound dde-shell:" << (m_manager != nullptr);

    return m_manager != nullptr;
}

bool DDdeShellManager::isValid() {
    return ensureManager() && m_manager != nullptr;
}

dde_shell_surface* DDdeShellManager::shellSurfaceFor(QWindow* window) {
    if (!window) {
        return nullptr;
    }

    QHash<QWindow *, dde_shell_surface *>::const_iterator it =
        m_surfaces.constFind(window);
    if (it != m_surfaces.constEnd()) {
        return it.value();
    }

    if (!ensureManager() || !m_manager) {
        return nullptr;
    }

    wl_surface* surface = surfaceForWindow(window);
    if (!surface) {
        return nullptr;
    }

    dde_shell_surface* ss = dde_shell_get_shell_surface(m_manager, surface);
    if (!ss) {
        return nullptr;
    }

    m_surfaces.insert(window, ss);
    connect(window, &QObject::destroyed, this, [this, window]() {
        if (dde_shell_surface* s = m_surfaces.take(window)) {
            dde_shell_surface_destroy(s);
        }
    });
    return ss;
}

void DDdeShellManager::setNoTitleBar(QWindow* window, bool noTitleBar) {
    dde_shell_surface* ss = shellSurfaceFor(window);
    if (!ss) {
        return;
    }

    // NoTitleBar: 1 = CSD
    wl_array arr;
    wl_array_init(&arr);
    if (int *p = static_cast<int *>(wl_array_add(&arr, sizeof(int)))) {
        *p = noTitleBar ? 1 : 0;
    }

    dde_shell_surface_set_property(ss, DDE_SHELL_PROPERTY_NOTITLEBAR, &arr);
    wl_array_release(&arr);
    wl_display_flush(m_display);

    if (shellDebug())
        qDebug() << "(DWindow) setNoTitleBar" << noTitleBar << "for" << window;
}

DWIDGET_END_NAMESPACE
