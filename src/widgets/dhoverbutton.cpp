#include "dhoverbutton.h"

#include <QEvent>
#include <QMouseEvent>

DWIDGET_BEGIN_NAMESPACE

DHoverButton::DHoverButton(QWidget *parent)
    : QPushButton(parent)
{
    setFlat(true);
    setIconSize(QSize(16, 16));
}

DHoverButton::DHoverButton(const QIcon &normalIcon, const QIcon &hoverIcon, QWidget *parent)
    : DHoverButton(parent)
{
    setNormalIcon(normalIcon);
    setHoverIcon(hoverIcon);
}

QIcon DHoverButton::normalIcon() const
{
    return m_normalIcon;
}

QIcon DHoverButton::hoverIcon() const
{
    return m_hoverIcon;
}

QIcon DHoverButton::pressedIcon() const
{
    return m_pressedIcon;
}

void DHoverButton::setNormalIcon(const QIcon &icon)
{
    m_normalIcon = icon;
    updateIconForState();
}

void DHoverButton::setHoverIcon(const QIcon &icon)
{
    m_hoverIcon = icon;
    updateIconForState();
}

void DHoverButton::setPressedIcon(const QIcon &icon)
{
    m_pressedIcon = icon;
    updateIconForState();
}

void DHoverButton::enterEvent(QEvent *event)
{
    m_hovered = true;
    updateIconForState();
    QPushButton::enterEvent(event);
}

void DHoverButton::leaveEvent(QEvent *event)
{
    m_hovered = false;
    m_pressed = false;
    updateIconForState();
    QPushButton::leaveEvent(event);
}

void DHoverButton::mousePressEvent(QMouseEvent *event)
{
    m_pressed = true;
    updateIconForState();
    QPushButton::mousePressEvent(event);
}

void DHoverButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;
    updateIconForState();
    QPushButton::mouseReleaseEvent(event);
}

void DHoverButton::changeEvent(QEvent *event)
{
    QPushButton::changeEvent(event);

    if (event->type() == QEvent::EnabledChange)
        updateIconForState();
}

void DHoverButton::updateIconForState()
{
    if (!isEnabled()) {
        QPushButton::setIcon(m_normalIcon);
        return;
    }

    if (m_pressed && !m_pressedIcon.isNull()) {
        QPushButton::setIcon(m_pressedIcon);
    } else if (m_hovered && !m_hoverIcon.isNull()) {
        QPushButton::setIcon(m_hoverIcon);
    } else {
        QPushButton::setIcon(m_normalIcon);
    }
}

DWIDGET_END_NAMESPACE
