#include "dicontextbutton.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>

DWIDGET_BEGIN_NAMESPACE

DIconTextButton::DIconTextButton(QWidget *parent)
    : QPushButton(parent)
{
    setIconSize(QSize(20, 20));
}

DIconTextButton::DIconTextButton(const QIcon &icon, const QString &text, QWidget *parent)
    : DIconTextButton(parent)
{
    setIcon(icon);
    setText(text);
}

int DIconTextButton::iconTextSpacing() const
{
    return m_iconTextSpacing;
}

void DIconTextButton::setIconTextSpacing(int spacing)
{
    spacing = qMax(0, spacing);

    if (m_iconTextSpacing == spacing)
        return;

    m_iconTextSpacing = spacing;
    updateGeometry();
    update();
}

void DIconTextButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStylePainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    painter.drawControl(QStyle::CE_PushButtonBevel, option);

    const QRect iconArea = iconRect(option);
    const QRect labelArea = textRect(option);
    const QIcon::Mode mode = option.state & QStyle::State_Enabled
        ? (option.state & QStyle::State_MouseOver ? QIcon::Active : QIcon::Normal)
        : QIcon::Disabled;
    const QIcon::State state = option.state & QStyle::State_On ? QIcon::On : QIcon::Off;

    if (!option.icon.isNull())
        option.icon.paint(&painter, iconArea, Qt::AlignCenter, mode, state);

    if (!option.text.isEmpty()) {
        style()->drawItemText(&painter, labelArea, Qt::AlignCenter | Qt::TextShowMnemonic,
                              option.palette, option.state & QStyle::State_Enabled,
                              option.text, QPalette::ButtonText);
    }

    if (option.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect focusOption;
        focusOption.QStyleOption::operator=(option);
        focusOption.rect = style()->subElementRect(QStyle::SE_PushButtonFocusRect, &option, this);
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, focusOption);
    }
}

QSize DIconTextButton::sizeHint() const
{
    QSize size = QPushButton::sizeHint();
    if (!icon().isNull() && !text().isEmpty())
        size.rwidth() += m_iconTextSpacing;
    return size;
}

QRect DIconTextButton::iconRect(const QStyleOptionButton &option) const
{
    if (option.icon.isNull())
        return QRect();

    const QRect contents = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
    const int textWidth = option.text.isEmpty() ? 0 : option.fontMetrics.horizontalAdvance(option.text);
    const int spacing = textWidth > 0 ? m_iconTextSpacing : 0;
    const int totalWidth = option.iconSize.width() + spacing + textWidth;
    int x = contents.x() + qMax(0, (contents.width() - totalWidth) / 2);

    if (option.direction == Qt::RightToLeft && textWidth > 0)
        x += textWidth + spacing;

    return QRect(x, contents.y() + (contents.height() - option.iconSize.height()) / 2,
                 option.iconSize.width(), option.iconSize.height());
}

QRect DIconTextButton::textRect(const QStyleOptionButton &option) const
{
    if (option.text.isEmpty())
        return QRect();

    const QRect contents = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
    const int textWidth = option.fontMetrics.horizontalAdvance(option.text);
    const int iconWidth = option.icon.isNull() ? 0 : option.iconSize.width();
    const int spacing = iconWidth > 0 ? m_iconTextSpacing : 0;
    const int totalWidth = iconWidth + spacing + textWidth;
    int x = contents.x() + qMax(0, (contents.width() - totalWidth) / 2);

    if (option.direction == Qt::LeftToRight && iconWidth > 0)
        x += iconWidth + spacing;

    return QRect(x, contents.y(), textWidth, contents.height());
}

DWIDGET_END_NAMESPACE
