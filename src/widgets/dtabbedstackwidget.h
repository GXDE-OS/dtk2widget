#ifndef DTABBEDSTACKWIDGET_H
#define DTABBEDSTACKWIDGET_H

#include "dtkwidget_global.h"

#include <QIcon>
#include <QTabBar>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedWidget;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE

class DTabBar;

class LIBDTKWIDGETSHARED_EXPORT DTabbedStackWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool tabsVisible READ tabsVisible WRITE setTabsVisible)
    Q_PROPERTY(QTabBar::Shape tabShape READ tabShape WRITE setTabShape)

public:
    explicit DTabbedStackWidget(QWidget *parent = nullptr);

    DTabBar *tabBar() const;
    QStackedWidget *stackedWidget() const;

    int addPage(QWidget *page, const QString &text);
    int addPage(QWidget *page, const QIcon &icon, const QString &text);
    int insertPage(int index, QWidget *page, const QString &text);
    int insertPage(int index, QWidget *page, const QIcon &icon, const QString &text);
    void removePage(int index);

    QWidget *page(int index) const;
    int indexOf(QWidget *page) const;
    int currentIndex() const;
    QWidget *currentPage() const;
    int count() const;

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);
    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

    bool tabsVisible() const;
    QTabBar::Shape tabShape() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentPage(QWidget *page);
    void setTabsVisible(bool visible);
    void setTabShape(QTabBar::Shape shape);

Q_SIGNALS:
    void currentChanged(int index);
    void currentPageChanged(QWidget *page);
    void pageAdded(int index, QWidget *page);
    void pageRemoved(int index, QWidget *page);

private:
    DTabBar *m_tabBar = nullptr;
    QStackedWidget *m_stackedWidget = nullptr;
};

DWIDGET_END_NAMESPACE

#endif // DTABBEDSTACKWIDGET_H
