#include "dtabbedstackwidget.h"

#include "dtabbar.h"

#include <QStackedWidget>
#include <QVBoxLayout>

DWIDGET_BEGIN_NAMESPACE

DTabbedStackWidget::DTabbedStackWidget(QWidget *parent)
    : QWidget(parent)
    , m_tabBar(new DTabBar(this))
    , m_stackedWidget(new QStackedWidget(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addWidget(m_tabBar, 0, Qt::AlignLeft);
    layout->addWidget(m_stackedWidget);

    m_tabBar->setDrawBase(false);
    m_tabBar->setExpanding(false);
    m_tabBar->setVisibleAddButton(false);

    connect(m_tabBar, &DTabBar::currentChanged, this, &DTabbedStackWidget::setCurrentIndex);
    connect(m_stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        if (m_tabBar->currentIndex() != index)
            m_tabBar->setCurrentIndex(index);

        Q_EMIT currentChanged(index);
        Q_EMIT currentPageChanged(m_stackedWidget->currentWidget());
    });
}

DTabBar *DTabbedStackWidget::tabBar() const
{
    return m_tabBar;
}

QStackedWidget *DTabbedStackWidget::stackedWidget() const
{
    return m_stackedWidget;
}

int DTabbedStackWidget::addPage(QWidget *page, const QString &text)
{
    return insertPage(count(), page, text);
}

int DTabbedStackWidget::addPage(QWidget *page, const QIcon &icon, const QString &text)
{
    return insertPage(count(), page, icon, text);
}

int DTabbedStackWidget::insertPage(int index, QWidget *page, const QString &text)
{
    if (!page)
        return -1;

    index = qBound(0, index, count());
    const int pageIndex = m_stackedWidget->insertWidget(index, page);
    m_tabBar->insertTab(pageIndex, text);

    if (count() == 1)
        setCurrentIndex(0);

    Q_EMIT pageAdded(pageIndex, page);
    return pageIndex;
}

int DTabbedStackWidget::insertPage(int index, QWidget *page, const QIcon &icon, const QString &text)
{
    if (!page)
        return -1;

    index = qBound(0, index, count());
    const int pageIndex = m_stackedWidget->insertWidget(index, page);
    m_tabBar->insertTab(pageIndex, icon, text);

    if (count() == 1)
        setCurrentIndex(0);

    Q_EMIT pageAdded(pageIndex, page);
    return pageIndex;
}

void DTabbedStackWidget::removePage(int index)
{
    QWidget *removedPage = page(index);
    if (!removedPage)
        return;

    m_tabBar->removeTab(index);
    m_stackedWidget->removeWidget(removedPage);
    Q_EMIT pageRemoved(index, removedPage);
}

QWidget *DTabbedStackWidget::page(int index) const
{
    return m_stackedWidget->widget(index);
}

int DTabbedStackWidget::indexOf(QWidget *page) const
{
    return m_stackedWidget->indexOf(page);
}

int DTabbedStackWidget::currentIndex() const
{
    return m_stackedWidget->currentIndex();
}

QWidget *DTabbedStackWidget::currentPage() const
{
    return m_stackedWidget->currentWidget();
}

int DTabbedStackWidget::count() const
{
    return m_stackedWidget->count();
}

QString DTabbedStackWidget::tabText(int index) const
{
    return m_tabBar->tabText(index);
}

void DTabbedStackWidget::setTabText(int index, const QString &text)
{
    m_tabBar->setTabText(index, text);
}

QIcon DTabbedStackWidget::tabIcon(int index) const
{
    return m_tabBar->tabIcon(index);
}

void DTabbedStackWidget::setTabIcon(int index, const QIcon &icon)
{
    m_tabBar->setTabIcon(index, icon);
}

bool DTabbedStackWidget::tabsVisible() const
{
    return m_tabBar->isVisible();
}

QTabBar::Shape DTabbedStackWidget::tabShape() const
{
    return m_tabBar->shape();
}

void DTabbedStackWidget::setCurrentIndex(int index)
{
    if (index < 0 || index >= count() || currentIndex() == index)
        return;

    m_stackedWidget->setCurrentIndex(index);
}

void DTabbedStackWidget::setCurrentPage(QWidget *page)
{
    setCurrentIndex(indexOf(page));
}

void DTabbedStackWidget::setTabsVisible(bool visible)
{
    m_tabBar->setVisible(visible);
}

void DTabbedStackWidget::setTabShape(QTabBar::Shape shape)
{
    m_tabBar->setShape(shape);
}

DWIDGET_END_NAMESPACE
