#include "dfloatingwidget.h"

#include "dblureffectwidget.h"

#include <QBoxLayout>
#include <QEvent>
#include <QPainterPath>
#include <QResizeEvent>
#include <QVBoxLayout>

DWIDGET_BEGIN_NAMESPACE

DFloatingWidget::DFloatingWidget(QWidget *parent)
    : DCardWidget(parent)
    , m_layout(new QVBoxLayout(this))
{
    setRadius(6);
    setShadowBlurRadius(24);
    setShadowColor(QColor(0, 0, 0, 48));
    setShadowOffset(QPointF(0, 6));

    m_layout->setContentsMargins(10, 8, 10, 8);
    m_layout->setSpacing(0);
}

QSize DFloatingWidget::sizeHint() const
{
    const QSize contentSize = m_widget ? m_widget->sizeHint() : QSize(120, 36);
    const QMargins margins = m_layout->contentsMargins();

    return contentSize + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
}

QWidget *DFloatingWidget::widget() const
{
    return m_widget;
}

DBlurEffectWidget *DFloatingWidget::blurBackground() const
{
    return m_blurBackground;
}

bool DFloatingWidget::blurBackgroundIsEnabled() const
{
    return m_blurBackground;
}

void DFloatingWidget::setWidget(QWidget *widget)
{
    if (m_widget == widget)
        return;

    if (m_widget)
        m_layout->removeWidget(m_widget);

    m_widget = widget;

    if (m_widget) {
        m_widget->setParent(this);
        m_layout->addWidget(m_widget);
        m_widget->raise();
    }

    updateGeometry();
}

void DFloatingWidget::setBlurBackgroundEnabled(bool enabled)
{
    if (enabled == bool(m_blurBackground))
        return;

    if (enabled) {
        m_blurBackground = new DBlurEffectWidget(this);
        m_blurBackground->setFocusPolicy(Qt::NoFocus);
        m_blurBackground->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_blurBackground->setBlendMode(DBlurEffectWidget::InWindowBlend);
        m_blurBackground->setMaskAlpha(80);
        m_blurBackground->lower();
        updateBlurGeometry();
        m_blurBackground->show();
    } else {
        m_blurBackground->deleteLater();
        m_blurBackground = nullptr;
    }

    setBackgroundRole(enabled ? QPalette::Window : QPalette::Base);
    update();
}

void DFloatingWidget::resizeEvent(QResizeEvent *event)
{
    DCardWidget::resizeEvent(event);
    updateBlurGeometry();
}

bool DFloatingWidget::event(QEvent *event)
{
    const bool result = DCardWidget::event(event);

    switch (event->type()) {
    case QEvent::Polish:
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        updateBlurMask();
        break;
    default:
        break;
    }

    return result;
}

void DFloatingWidget::updateBlurGeometry()
{
    if (!m_blurBackground)
        return;

    m_blurBackground->setGeometry(rect());
    updateBlurMask();
}

void DFloatingWidget::updateBlurMask()
{
    if (!m_blurBackground)
        return;

    const int r = radius();
    m_blurBackground->setBlurRectXRadius(r);
    m_blurBackground->setBlurRectYRadius(r);

    QPainterPath path;
    path.addRoundedRect(rect(), r, r);
    m_blurBackground->setMaskPath(path);
}

DWIDGET_END_NAMESPACE
