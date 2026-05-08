#ifndef DICONTEXTBUTTON_H
#define DICONTEXTBUTTON_H

#include "dtkwidget_global.h"

#include <QIcon>
#include <QPushButton>
#include <QStyleOptionButton>

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DIconTextButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int iconTextSpacing READ iconTextSpacing WRITE setIconTextSpacing)

public:
    explicit DIconTextButton(QWidget *parent = nullptr);
    DIconTextButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);

    int iconTextSpacing() const;

public Q_SLOTS:
    void setIconTextSpacing(int spacing);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    QRect iconRect(const QStyleOptionButton &option) const;
    QRect textRect(const QStyleOptionButton &option) const;

    int m_iconTextSpacing = 6;
};

DWIDGET_END_NAMESPACE

#endif // DICONTEXTBUTTON_H
