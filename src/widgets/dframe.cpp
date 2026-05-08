#include "dframe.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionFrame>

DWIDGET_BEGIN_NAMESPACE

DFrame::DFrame(QWidget *parent)
    : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setFrameShape(QFrame::StyledPanel);
}

bool DFrame::frameRounded() const
{
    return m_frameRounded;
}

int DFrame::radius() const
{
    return m_radius;
}

QPalette::ColorRole DFrame::backgroundRole() const
{
    return m_backgroundRole;
}

void DFrame::setFrameRounded(bool rounded)
{
    if (m_frameRounded == rounded)
        return;

    m_frameRounded = rounded;
    update();
}

void DFrame::setRadius(int radius)
{
    radius = qMax(0, radius);

    if (m_radius == radius)
        return;

    m_radius = radius;
    update();
}

void DFrame::setBackgroundRole(QPalette::ColorRole role)
{
    if (m_backgroundRole == role)
        return;

    m_backgroundRole = role;
    update();
}

void DFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOptionFrame option;
    initStyleOption(&option);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF borderRect = rect().adjusted(0, 0, -1, -1);
    const QPainterPath path = [this, borderRect]() {
        QPainterPath painterPath;
        if (m_frameRounded) {
            painterPath.addRoundedRect(borderRect, m_radius, m_radius);
        } else {
            painterPath.addRect(borderRect);
        }
        return painterPath;
    }();

    painter.fillPath(path, palette().brush(m_backgroundRole));

    if (lineWidth() > 0) {
        const QColor borderColor = palette().color(QPalette::Mid);
        painter.setPen(QPen(borderColor, lineWidth()));
        painter.drawPath(path);
    }
}

DHorizontalLine::DHorizontalLine(QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags)
{
    setFrameShape(QFrame::HLine);
}

DVerticalLine::DVerticalLine(QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags)
{
    setFrameShape(QFrame::VLine);
}

DWIDGET_END_NAMESPACE
