#ifndef DHOVERBUTTON_H
#define DHOVERBUTTON_H

#include "dtkwidget_global.h"

#include <QIcon>
#include <QPushButton>

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DHoverButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QIcon normalIcon READ normalIcon WRITE setNormalIcon)
    Q_PROPERTY(QIcon hoverIcon READ hoverIcon WRITE setHoverIcon)
    Q_PROPERTY(QIcon pressedIcon READ pressedIcon WRITE setPressedIcon)

public:
    explicit DHoverButton(QWidget *parent = nullptr);
    DHoverButton(const QIcon &normalIcon, const QIcon &hoverIcon, QWidget *parent = nullptr);

    QIcon normalIcon() const;
    QIcon hoverIcon() const;
    QIcon pressedIcon() const;

public Q_SLOTS:
    void setNormalIcon(const QIcon &icon);
    void setHoverIcon(const QIcon &icon);
    void setPressedIcon(const QIcon &icon);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateIconForState();

    QIcon m_normalIcon;
    QIcon m_hoverIcon;
    QIcon m_pressedIcon;
    bool m_hovered = false;
    bool m_pressed = false;
};

DWIDGET_END_NAMESPACE

#endif // DHOVERBUTTON_H
