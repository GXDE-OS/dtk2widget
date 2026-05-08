#include "dbackgroundgroup.h"

#include <QBoxLayout>
#include <QChildEvent>
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QStyle>

DWIDGET_BEGIN_NAMESPACE

DBackgroundGroup::DBackgroundGroup(QLayout *layout, QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setLayout(layout);
}

QMargins DBackgroundGroup::itemMargins() const
{
    return m_itemMargins;
}

int DBackgroundGroup::itemSpacing() const
{
    return m_itemSpacing;
}

int DBackgroundGroup::radius() const
{
    return m_radius;
}

bool DBackgroundGroup::useWidgetBackground() const
{
    return m_useWidgetBackground;
}

QPalette::ColorRole DBackgroundGroup::backgroundRole() const
{
    return m_backgroundRole;
}

void DBackgroundGroup::setLayout(QLayout *layout)
{
    QWidget::setLayout(layout);

    if (!layout)
        return;

    setItemMargins(layout->contentsMargins());
    updateLayoutSpacing();
}

void DBackgroundGroup::setItemMargins(const QMargins &margins)
{
    if (m_itemMargins == margins)
        return;

    m_itemMargins = margins;
    updateLayoutSpacing();
    update();
}

void DBackgroundGroup::setItemSpacing(int spacing)
{
    if (m_itemSpacing == spacing)
        return;

    m_itemSpacing = spacing;
    updateLayoutSpacing();
    update();
}

void DBackgroundGroup::setRadius(int radius)
{
    radius = qMax(0, radius);

    if (m_radius == radius)
        return;

    m_radius = radius;
    update();
}

void DBackgroundGroup::setUseWidgetBackground(bool useWidgetBackground)
{
    if (m_useWidgetBackground == useWidgetBackground)
        return;

    m_useWidgetBackground = useWidgetBackground;
    Q_EMIT useWidgetBackgroundChanged(useWidgetBackground);
    update();
}

void DBackgroundGroup::setBackgroundRole(QPalette::ColorRole role)
{
    if (m_backgroundRole == role)
        return;

    m_backgroundRole = role;
    update();
}

void DBackgroundGroup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QBrush background = m_useWidgetBackground
        ? palette().brush(m_backgroundRole)
        : palette().brush(QPalette::Window);
    const QPen border(QColor(palette().color(QPalette::Mid).red(),
                             palette().color(QPalette::Mid).green(),
                             palette().color(QPalette::Mid).blue(), 90), 1);

    const QList<QPair<QWidget *, ItemPosition> > items = visibleItems();
    for (const QPair<QWidget *, ItemPosition> &item : items) {
        if (!item.first)
            continue;

        QRect rect = item.first->geometry().adjusted(-m_itemMargins.left(),
                                                     -m_itemMargins.top(),
                                                     m_itemMargins.right(),
                                                     m_itemMargins.bottom());
        rect = rect.intersected(this->rect());

        QPainterPath path = itemPath(QRectF(rect).adjusted(0, 0, -1, -1), item.second);
        painter.fillPath(path, background);
        painter.setPen(border);
        painter.drawPath(path);
    }
}

bool DBackgroundGroup::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::ChildAdded:
    case QEvent::ChildRemoved:
    case QEvent::LayoutDirectionChange:
    case QEvent::LayoutRequest:
    case QEvent::Show:
        update();
        break;
    case QEvent::StyleChange:
        updateLayoutSpacing();
        update();
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

QList<QPair<QWidget *, DBackgroundGroup::ItemPosition> > DBackgroundGroup::visibleItems() const
{
    QList<QWidget *> widgets;
    const QObjectList children = this->children();

    for (QObject *object : children) {
        QWidget *widget = qobject_cast<QWidget *>(object);
        if (widget && widget->isVisible())
            widgets.append(widget);
    }

    QList<QPair<QWidget *, ItemPosition> > items;
    if (widgets.isEmpty())
        return items;

    if (widgets.size() == 1) {
        items.append(qMakePair(widgets.first(), OnlyOne));
        return items;
    }

    for (int i = 0; i < widgets.size(); ++i) {
        ItemPosition position = Middle;
        if (i == 0)
            position = layoutDirection() == Qt::LeftToRight ? Beginning : End;
        else if (i == widgets.size() - 1)
            position = layoutDirection() == Qt::LeftToRight ? End : Beginning;

        items.append(qMakePair(widgets.at(i), position));
    }

    return items;
}

QPainterPath DBackgroundGroup::itemPath(const QRectF &rect, ItemPosition position) const
{
    QPainterPath path;

    if (position == OnlyOne || m_radius <= 0) {
        if (m_radius > 0)
            path.addRoundedRect(rect, m_radius, m_radius);
        else
            path.addRect(rect);
        return path;
    }

    const qreal r = qMin<qreal>(m_radius, qMin(rect.width(), rect.height()) / 2.0);
    const bool horizontal = layoutOrientation() == Qt::Horizontal;

    if (position == Middle) {
        path.addRect(rect);
        return path;
    }

    if (horizontal) {
        const bool leftRounded = position == Beginning;
        const bool rightRounded = position == End;
        path.moveTo(rect.left() + (leftRounded ? r : 0), rect.top());
        path.lineTo(rect.right() - (rightRounded ? r : 0), rect.top());
        if (rightRounded)
            path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + r);
        path.lineTo(rect.right(), rect.bottom() - (rightRounded ? r : 0));
        if (rightRounded)
            path.quadTo(rect.right(), rect.bottom(), rect.right() - r, rect.bottom());
        path.lineTo(rect.left() + (leftRounded ? r : 0), rect.bottom());
        if (leftRounded)
            path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - r);
        path.lineTo(rect.left(), rect.top() + (leftRounded ? r : 0));
        if (leftRounded)
            path.quadTo(rect.left(), rect.top(), rect.left() + r, rect.top());
    } else {
        const bool topRounded = position == Beginning;
        const bool bottomRounded = position == End;
        path.moveTo(rect.left() + (topRounded ? r : 0), rect.top());
        path.lineTo(rect.right() - (topRounded ? r : 0), rect.top());
        if (topRounded)
            path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + r);
        path.lineTo(rect.right(), rect.bottom() - (bottomRounded ? r : 0));
        if (bottomRounded)
            path.quadTo(rect.right(), rect.bottom(), rect.right() - r, rect.bottom());
        path.lineTo(rect.left() + (bottomRounded ? r : 0), rect.bottom());
        if (bottomRounded)
            path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - r);
        path.lineTo(rect.left(), rect.top() + (topRounded ? r : 0));
        if (topRounded)
            path.quadTo(rect.left(), rect.top(), rect.left() + r, rect.top());
    }

    path.closeSubpath();
    return path;
}

Qt::Orientation DBackgroundGroup::layoutOrientation() const
{
    const QBoxLayout *boxLayout = qobject_cast<QBoxLayout *>(layout());
    if (!boxLayout)
        return Qt::Vertical;

    const QBoxLayout::Direction direction = boxLayout->direction();
    return direction == QBoxLayout::LeftToRight || direction == QBoxLayout::RightToLeft
        ? Qt::Horizontal
        : Qt::Vertical;
}

void DBackgroundGroup::updateLayoutSpacing()
{
    QLayout *currentLayout = layout();
    if (!currentLayout)
        return;

    int spacing = m_itemSpacing;
    if (spacing < 0) {
        const bool horizontal = layoutOrientation() == Qt::Horizontal;
        spacing = style()->pixelMetric(horizontal ? QStyle::PM_LayoutHorizontalSpacing
                                                  : QStyle::PM_LayoutVerticalSpacing,
                                       nullptr, this);
    }

    if (layoutOrientation() == Qt::Horizontal) {
        currentLayout->setSpacing(m_itemMargins.left() + m_itemMargins.right() + spacing);
    } else {
        currentLayout->setSpacing(m_itemMargins.top() + m_itemMargins.bottom() + spacing);
    }
}

DWIDGET_END_NAMESPACE
