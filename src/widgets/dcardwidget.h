#ifndef DCARDWIDGET_H
#define DCARDWIDGET_H

#include "dframe.h"

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DCardWidget : public DFrame
{
    Q_OBJECT
    Q_PROPERTY(bool shadowEnabled READ shadowEnabled WRITE setShadowEnabled)
    Q_PROPERTY(qreal shadowBlurRadius READ shadowBlurRadius WRITE setShadowBlurRadius)
    Q_PROPERTY(QColor shadowColor READ shadowColor WRITE setShadowColor)
    Q_PROPERTY(QPointF shadowOffset READ shadowOffset WRITE setShadowOffset)

public:
    explicit DCardWidget(QWidget *parent = nullptr);

    bool shadowEnabled() const;
    qreal shadowBlurRadius() const;
    QColor shadowColor() const;
    QPointF shadowOffset() const;

public Q_SLOTS:
    void setShadowEnabled(bool enabled);
    void setShadowBlurRadius(qreal radius);
    void setShadowColor(const QColor &color);
    void setShadowOffset(const QPointF &offset);

protected:
    void changeEvent(QEvent *event) override;

private:
    void updateShadow();

    bool m_shadowEnabled = true;
    qreal m_shadowBlurRadius = 18.0;
    QColor m_shadowColor = QColor(0, 0, 0, 36);
    QPointF m_shadowOffset = QPointF(0, 4);
};

DWIDGET_END_NAMESPACE

#endif // DCARDWIDGET_H
