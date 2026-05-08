#include "dcardwidget.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>

DWIDGET_BEGIN_NAMESPACE

DCardWidget::DCardWidget(QWidget *parent)
    : DFrame(parent)
{
    setRadius(6);
    setLineWidth(1);
    updateShadow();
}

bool DCardWidget::shadowEnabled() const
{
    return m_shadowEnabled;
}

qreal DCardWidget::shadowBlurRadius() const
{
    return m_shadowBlurRadius;
}

QColor DCardWidget::shadowColor() const
{
    return m_shadowColor;
}

QPointF DCardWidget::shadowOffset() const
{
    return m_shadowOffset;
}

void DCardWidget::setShadowEnabled(bool enabled)
{
    if (m_shadowEnabled == enabled)
        return;

    m_shadowEnabled = enabled;
    updateShadow();
}

void DCardWidget::setShadowBlurRadius(qreal radius)
{
    radius = qMax<qreal>(0, radius);

    if (qFuzzyCompare(m_shadowBlurRadius, radius))
        return;

    m_shadowBlurRadius = radius;
    updateShadow();
}

void DCardWidget::setShadowColor(const QColor &color)
{
    if (m_shadowColor == color)
        return;

    m_shadowColor = color;
    updateShadow();
}

void DCardWidget::setShadowOffset(const QPointF &offset)
{
    if (m_shadowOffset == offset)
        return;

    m_shadowOffset = offset;
    updateShadow();
}

void DCardWidget::changeEvent(QEvent *event)
{
    DFrame::changeEvent(event);

    if (event->type() == QEvent::PaletteChange)
        updateShadow();
}

void DCardWidget::updateShadow()
{
    if (!m_shadowEnabled) {
        setGraphicsEffect(nullptr);
        return;
    }

    QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect *>(graphicsEffect());

    if (!effect) {
        effect = new QGraphicsDropShadowEffect(this);
        setGraphicsEffect(effect);
    }

    effect->setBlurRadius(m_shadowBlurRadius);
    effect->setColor(m_shadowColor);
    effect->setOffset(m_shadowOffset);
}

DWIDGET_END_NAMESPACE
