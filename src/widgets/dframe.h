#ifndef DFRAME_H
#define DFRAME_H

#include "dtkwidget_global.h"

#include <QFrame>

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DFrame : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool frameRounded READ frameRounded WRITE setFrameRounded)
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QPalette::ColorRole backgroundRole READ backgroundRole WRITE setBackgroundRole)

public:
    explicit DFrame(QWidget *parent = nullptr);

    bool frameRounded() const;
    int radius() const;
    QPalette::ColorRole backgroundRole() const;

public Q_SLOTS:
    void setFrameRounded(bool rounded);
    void setRadius(int radius);
    void setBackgroundRole(QPalette::ColorRole role);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_frameRounded = true;
    int m_radius = 6;
    QPalette::ColorRole m_backgroundRole = QPalette::Base;
};

class LIBDTKWIDGETSHARED_EXPORT DHorizontalLine : public QFrame
{
    Q_OBJECT

public:
    explicit DHorizontalLine(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
};

class LIBDTKWIDGETSHARED_EXPORT DVerticalLine : public QFrame
{
    Q_OBJECT

public:
    explicit DVerticalLine(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
};

DWIDGET_END_NAMESPACE

#endif // DFRAME_H
