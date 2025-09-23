#include "dtkwidget_global.h"
#include <QAbstractButton>
#include <QStyleOptionButton>
#include <QPainter>
#include <QFontMetrics>
#include <QWidget>
#include "dcommandlinkbutton.h"
DWIDGET_BEGIN_NAMESPACE

    enum Margins {
        LeftMargins = 3,
        RightMargins = LeftMargins,
        TopLeftMargins = 3,
        BottomMargins = LeftMargins,
        TextMargins = 4
    };


DCommandLinkButton::DCommandLinkButton(const QString &text, QWidget *parent)
    : QAbstractButton(parent)
{
    this->setText(text);
}


QSize DCommandLinkButton::sizeHint() const
{
    QString text = this->text();
    QSize size = fontMetrics().size(0, text);
    size += QSize(LeftMargins + RightMargins + TextMargins * 2, TopLeftMargins + BottomMargins);
    return size;
}

void DCommandLinkButton::initStyleOption(QStyleOptionButton *option) const
{
    option->initFrom(this);

    if (isDown())
        option->state |= QStyle::State_Sunken;

    QPalette pa = this->palette();
    option->palette.setBrush(QPalette::ButtonText, QColor(0, 122, 255)); // 设置为蓝色
    option->palette.setBrush(QPalette::Button, Qt::transparent); // 背景透明
    option->text = this->text();
}

void DCommandLinkButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QStyleOptionButton opt;
    initStyleOption(&opt);

    QPainter painter(this);
    painter.setPen(QColor(0, 122, 255));  // 蓝色文字
    style()->drawControl(QStyle::CE_PushButtonLabel, &opt, &painter, this);
}
void DCommandLinkButton::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    setCursor(Qt::PointingHandCursor);  // 鼠标进入时设置为手型
}

void DCommandLinkButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    unsetCursor();  // 鼠标离开时恢复默认
}
DWIDGET_END_NAMESPACE
