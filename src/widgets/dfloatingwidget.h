#ifndef DFLOATINGWIDGET_H
#define DFLOATINGWIDGET_H

#include "dcardwidget.h"

class QBoxLayout;

DWIDGET_BEGIN_NAMESPACE

class DBlurEffectWidget;

class LIBDTKWIDGETSHARED_EXPORT DFloatingWidget : public DCardWidget
{
    Q_OBJECT
    Q_PROPERTY(bool blurBackgroundEnabled READ blurBackgroundIsEnabled WRITE setBlurBackgroundEnabled)

public:
    explicit DFloatingWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    QWidget *widget() const;
    DBlurEffectWidget *blurBackground() const;
    bool blurBackgroundIsEnabled() const;

public Q_SLOTS:
    void setWidget(QWidget *widget);
    void setBlurBackgroundEnabled(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;

private:
    void updateBlurGeometry();
    void updateBlurMask();

    QBoxLayout *m_layout = nullptr;
    QWidget *m_widget = nullptr;
    DBlurEffectWidget *m_blurBackground = nullptr;
};

DWIDGET_END_NAMESPACE

#endif // DFLOATINGWIDGET_H
