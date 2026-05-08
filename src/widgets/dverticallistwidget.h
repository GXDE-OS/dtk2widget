#ifndef DVERTICALLISTWIDGET_H
#define DVERTICALLISTWIDGET_H

#include "dlistview.h"

#include <QIcon>
#include <QStringList>
#include <QVariant>

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DVerticalListWidget : public DListView
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int currentRow READ currentRow WRITE setCurrentRow NOTIFY currentRowChanged)

public:
    explicit DVerticalListWidget(QWidget *parent = nullptr);

    int addItem(const QString &text, const QIcon &icon = QIcon(), const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);
    void clear();

    int count() const;
    int currentRow() const;
    QString itemText(int row) const;
    QVariant itemData(int row) const;

public Q_SLOTS:
    void setCurrentRow(int row);

Q_SIGNALS:
    void countChanged(int count);
    void currentRowChanged(int row);
    void itemClicked(int row);

private:
    class DVerticalListWidgetPrivate;
    DVerticalListWidgetPrivate *d = nullptr;
};

DWIDGET_END_NAMESPACE

#endif // DVERTICALLISTWIDGET_H
