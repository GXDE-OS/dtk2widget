#include "dverticallistwidget.h"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

DWIDGET_USE_NAMESPACE

namespace {

const int UserDataRole = Qt::UserRole + 1;

class VerticalListDelegate : public QStyledItemDelegate
{
public:
    explicit VerticalListDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        const bool selected = option.state & QStyle::State_Selected;
        const bool hovered = option.state & QStyle::State_MouseOver;
        QColor backgroundColor = option.palette.color(QPalette::Base);
        QColor textColor = option.palette.color(QPalette::WindowText);

        if (selected) {
            backgroundColor = option.palette.color(QPalette::Highlight);
            textColor = option.palette.color(QPalette::HighlightedText);
        } else if (hovered) {
            backgroundColor = option.palette.color(QPalette::WindowText);
            backgroundColor.setAlpha(22);
        } else {
            backgroundColor.setAlpha(0);
        }

        painter->fillRect(option.rect, backgroundColor);

        QRect contentRect = option.rect.adjusted(13, 0, -13, 0);
        const QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            const QSize iconSize(16, 16);
            QRect iconRect(QPoint(0, 0), iconSize);
            iconRect.moveLeft(contentRect.left());
            iconRect.moveCenter(QPoint(iconRect.center().x(), contentRect.center().y()));
            icon.paint(painter, iconRect);
            contentRect.setLeft(iconRect.right() + 8);
        }

        painter->setPen(textColor);
        const QString text = option.fontMetrics.elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight, contentRect.width());
        painter->drawText(contentRect, Qt::AlignVCenter | Qt::AlignLeft, text);

        if (selected)
            painter->fillRect(QRect(option.rect.right() - 2, option.rect.top(), 3, option.rect.height()), option.palette.color(QPalette::Highlight));

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return QSize(160, 30);
    }
};

} // namespace

class DVerticalListWidget::DVerticalListWidgetPrivate
{
public:
    QStandardItemModel *model = nullptr;
};

DVerticalListWidget::DVerticalListWidget(QWidget *parent)
    : DListView(parent)
    , d(new DVerticalListWidgetPrivate)
{
    d->model = new QStandardItemModel(this);
    setModel(d->model);
    setItemDelegate(new VerticalListDelegate(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setFrameShape(QFrame::NoFrame);
    setMouseTracking(true);
    setSpacing(0);
    setUniformItemSizes(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setViewportMargins(0, 0, -verticalScrollBar()->sizeHint().width(), 0);
    verticalScrollBar()->setSingleStep(24);
    viewport()->setAutoFillBackground(false);
    setAutoFillBackground(false);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current) {
        Q_EMIT currentRowChanged(current.row());
    });
    connect(this, &QListView::clicked, this, [this](const QModelIndex &index) {
        Q_EMIT itemClicked(index.row());
    });
}

int DVerticalListWidget::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    QStandardItem *item = new QStandardItem(icon, text);
    item->setData(userData, UserDataRole);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    d->model->appendRow(item);
    Q_EMIT countChanged(count());
    return d->model->rowCount() - 1;
}

void DVerticalListWidget::addItems(const QStringList &texts)
{
    for (const QString &text : texts)
        addItem(text);
}

void DVerticalListWidget::clear()
{
    d->model->clear();
    Q_EMIT countChanged(0);
    Q_EMIT currentRowChanged(-1);
}

int DVerticalListWidget::count() const
{
    return d->model->rowCount();
}

int DVerticalListWidget::currentRow() const
{
    return currentIndex().row();
}

QString DVerticalListWidget::itemText(int row) const
{
    const QModelIndex index = d->model->index(row, 0);
    return index.data(Qt::DisplayRole).toString();
}

QVariant DVerticalListWidget::itemData(int row) const
{
    const QModelIndex index = d->model->index(row, 0);
    return index.data(UserDataRole);
}

void DVerticalListWidget::setCurrentRow(int row)
{
    if (row < 0 || row >= count())
        return;

    setCurrentIndex(d->model->index(row, 0));
}
