#ifndef DBACKGROUNDGROUP_H
#define DBACKGROUNDGROUP_H

#include "dtkwidget_global.h"

#include <QList>
#include <QMargins>
#include <QPair>
#include <QPalette>
#include <QPainterPath>
#include <QWidget>

class QBoxLayout;
class QLayout;

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DBackgroundGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QMargins itemMargins READ itemMargins WRITE setItemMargins)
    Q_PROPERTY(int itemSpacing READ itemSpacing WRITE setItemSpacing)
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(bool useWidgetBackground READ useWidgetBackground WRITE setUseWidgetBackground NOTIFY useWidgetBackgroundChanged)
    Q_PROPERTY(QPalette::ColorRole backgroundRole READ backgroundRole WRITE setBackgroundRole)

public:
    explicit DBackgroundGroup(QLayout *layout = nullptr, QWidget *parent = nullptr);

    QMargins itemMargins() const;
    int itemSpacing() const;
    int radius() const;
    bool useWidgetBackground() const;
    QPalette::ColorRole backgroundRole() const;

    void setLayout(QLayout *layout);

public Q_SLOTS:
    void setItemMargins(const QMargins &margins);
    void setItemSpacing(int spacing);
    void setRadius(int radius);
    void setUseWidgetBackground(bool useWidgetBackground);
    void setBackgroundRole(QPalette::ColorRole role);

Q_SIGNALS:
    void useWidgetBackgroundChanged(bool useWidgetBackground);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private:
    enum ItemPosition {
        Invalid,
        OnlyOne,
        Beginning,
        Middle,
        End
    };

    QList<QPair<QWidget *, ItemPosition> > visibleItems() const;
    QPainterPath itemPath(const QRectF &rect, ItemPosition position) const;
    Qt::Orientation layoutOrientation() const;
    void updateLayoutSpacing();

    QMargins m_itemMargins;
    int m_itemSpacing = -1;
    int m_radius = 6;
    bool m_useWidgetBackground = true;
    QPalette::ColorRole m_backgroundRole = QPalette::Base;

private:
    using QWidget::setLayout;
    using QWidget::setAutoFillBackground;
};

DWIDGET_END_NAMESPACE

#endif // DBACKGROUNDGROUP_H
