#include "dblursurface.h"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>

DWIDGET_BEGIN_NAMESPACE

DBlurSurface::DBlurSurface(QWidget *parent)
    : DCardWidget(parent)
{
    setRadius(6);
    setLineWidth(1);
    setBackgroundRole(QPalette::Window);
    setShadowBlurRadius(16);
    setShadowColor(QColor(0, 0, 0, 28));
    setShadowOffset(QPointF(0, 3));

    ensureBlurBackground();
}

bool DBlurSurface::blurEnabled() const
{
    return m_blurEnabled;
}

quint8 DBlurSurface::maskAlpha() const
{
    return m_maskAlpha;
}

QColor DBlurSurface::maskColor() const
{
    return m_customMaskColor ? m_maskColor : palette().color(QPalette::Window);
}

DBlurEffectWidget::BlendMode DBlurSurface::blendMode() const
{
    return m_blendMode;
}

DBlurEffectWidget *DBlurSurface::blurBackground() const
{
    return m_blurBackground;
}

void DBlurSurface::setBlurEnabled(bool enabled)
{
    if (m_blurEnabled == enabled)
        return;

    m_blurEnabled = enabled;

    if (m_blurEnabled) {
        ensureBlurBackground();
    } else if (m_blurBackground) {
        m_blurBackground->deleteLater();
        m_blurBackground = nullptr;
    }

    update();
}

void DBlurSurface::setMaskAlpha(quint8 alpha)
{
    if (m_maskAlpha == alpha)
        return;

    m_maskAlpha = alpha;
    updateBlurPalette();
}

void DBlurSurface::setMaskColor(const QColor &color)
{
    if (m_customMaskColor && m_maskColor == color)
        return;

    m_customMaskColor = true;
    m_maskColor = color;
    updateBlurPalette();
}

void DBlurSurface::setMaskColor(DBlurEffectWidget::MaskColorType type)
{
    ensureBlurBackground();

    if (m_blurBackground)
        m_blurBackground->setMaskColor(type);

    m_customMaskColor = false;
    update();
}

void DBlurSurface::setBlendMode(DBlurEffectWidget::BlendMode mode)
{
    if (m_blendMode == mode)
        return;

    m_blendMode = mode;

    if (m_blurBackground)
        m_blurBackground->setBlendMode(m_blendMode);
}

void DBlurSurface::paintEvent(QPaintEvent *event)
{
    if (!m_blurEnabled) {
        DCardWidget::paintEvent(event);
        return;
    }

    Q_UNUSED(event)

    updateBlurMask();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF borderRect = rect().adjusted(0, 0, -1, -1);
    QPainterPath path;
    path.addRoundedRect(borderRect, radius(), radius());

    QColor tint = maskColor();
    tint.setAlpha(qMin<int>(m_maskAlpha, 44));
    painter.fillPath(path, tint);

    if (lineWidth() > 0) {
        QColor borderColor = palette().color(QPalette::Mid);
        borderColor.setAlpha(80);
        painter.setPen(QPen(borderColor, lineWidth()));
        painter.drawPath(path);
    }
}

void DBlurSurface::resizeEvent(QResizeEvent *event)
{
    DCardWidget::resizeEvent(event);
    updateBlurGeometry();
}

bool DBlurSurface::event(QEvent *event)
{
    const bool result = DCardWidget::event(event);

    switch (event->type()) {
    case QEvent::Polish:
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        updateBlurMask();
        updateBlurPalette();
        break;
    default:
        break;
    }

    return result;
}

void DBlurSurface::ensureBlurBackground()
{
    if (!m_blurEnabled || m_blurBackground)
        return;

    m_blurBackground = new DBlurEffectWidget(this);
    m_blurBackground->setFocusPolicy(Qt::NoFocus);
    m_blurBackground->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_blurBackground->lower();
    updateBlurPalette();
    updateBlurGeometry();
    m_blurBackground->show();
}

void DBlurSurface::updateBlurGeometry()
{
    if (!m_blurBackground)
        return;

    m_blurBackground->setGeometry(rect());
    updateBlurMask();
}

void DBlurSurface::updateBlurMask()
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

void DBlurSurface::updateBlurPalette()
{
    if (!m_blurBackground)
        return;

    m_blurBackground->setBlendMode(m_blendMode);
    m_blurBackground->setMaskAlpha(m_maskAlpha);
    m_blurBackground->setMaskColor(maskColor());
    update();
}

DWIDGET_END_NAMESPACE
