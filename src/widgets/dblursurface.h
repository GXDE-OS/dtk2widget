#ifndef DBLURSURFACE_H
#define DBLURSURFACE_H

#include "dcardwidget.h"
#include "dblureffectwidget.h"

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DBlurSurface : public DCardWidget
{
    Q_OBJECT
    Q_PROPERTY(bool blurEnabled READ blurEnabled WRITE setBlurEnabled)
    Q_PROPERTY(quint8 maskAlpha READ maskAlpha WRITE setMaskAlpha)
    Q_PROPERTY(QColor maskColor READ maskColor WRITE setMaskColor)
    Q_PROPERTY(DBlurEffectWidget::BlendMode blendMode READ blendMode WRITE setBlendMode)

public:
    explicit DBlurSurface(QWidget *parent = nullptr);

    bool blurEnabled() const;
    quint8 maskAlpha() const;
    QColor maskColor() const;
    DBlurEffectWidget::BlendMode blendMode() const;
    DBlurEffectWidget *blurBackground() const;

public Q_SLOTS:
    void setBlurEnabled(bool enabled);
    void setMaskAlpha(quint8 alpha);
    void setMaskColor(const QColor &color);
    void setMaskColor(DBlurEffectWidget::MaskColorType type);
    void setBlendMode(DBlurEffectWidget::BlendMode mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;

private:
    void ensureBlurBackground();
    void updateBlurGeometry();
    void updateBlurMask();
    void updateBlurPalette();

    DBlurEffectWidget *m_blurBackground = nullptr;
    bool m_blurEnabled = true;
    bool m_customMaskColor = false;
    quint8 m_maskAlpha = 90;
    QColor m_maskColor;
    DBlurEffectWidget::BlendMode m_blendMode = DBlurEffectWidget::InWindowBlend;
};

DWIDGET_END_NAMESPACE

#endif // DBLURSURFACE_H
