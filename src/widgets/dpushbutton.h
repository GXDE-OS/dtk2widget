#ifndef DPUSHBUTTON_H
#define DPUSHBUTTON_H

#include "dtkwidget_global.h"

#include <QPainter>
#include <QPushButton>
#include <QStylePainter>
#include <QStyleOptionButton>

DWIDGET_BEGIN_NAMESPACE

class DPushButton : public QPushButton
{
public:
    explicit DPushButton(QWidget *parent = nullptr)
        : QPushButton(parent)
    {
    }

    explicit DPushButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
    }

    DPushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr)
        : QPushButton(icon, text, parent)
    {
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QStylePainter painter(this);
        QStyleOptionButton option;
        initStyleOption(&option);

        painter.drawControl(QStyle::CE_PushButtonBevel, option);

        QRect contents = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
        const bool hasIcon = !option.icon.isNull();
        const bool hasText = !option.text.isEmpty();
        const int spacing = hasIcon && hasText ? 4 : 0;
        const QSize iconSize = hasIcon ? option.iconSize : QSize();
        const int textWidth = hasText ? option.fontMetrics.horizontalAdvance(option.text) : 0;
        const int totalWidth = iconSize.width() + spacing + textWidth;
        int x = contents.x() + qMax(0, (contents.width() - totalWidth) / 2);

        QRect iconRect;
        QRect textRect;
        if (option.direction == Qt::RightToLeft) {
            if (hasText) {
                textRect = QRect(x, contents.y(), textWidth, contents.height());
                x += textWidth + spacing;
            }
            if (hasIcon) {
                iconRect = QRect(x, contents.y() + (contents.height() - iconSize.height()) / 2,
                                 iconSize.width(), iconSize.height());
            }
        } else {
            if (hasIcon) {
                iconRect = QRect(x, contents.y() + (contents.height() - iconSize.height()) / 2,
                                 iconSize.width(), iconSize.height());
                x += iconSize.width() + spacing;
            }
            if (hasText) {
                textRect = QRect(x, contents.y(), textWidth, contents.height());
            }
        }

        const QIcon::Mode iconMode = option.state & QStyle::State_Enabled
            ? (option.state & QStyle::State_MouseOver ? QIcon::Active : QIcon::Normal)
            : QIcon::Disabled;
        const QIcon::State iconState = option.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        if (hasIcon) {
            option.icon.paint(&painter, iconRect, Qt::AlignCenter, iconMode, iconState);
        }
        if (hasText) {
            style()->drawItemText(&painter, textRect, Qt::AlignCenter | Qt::TextShowMnemonic,
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
};

using GPushButton = DPushButton;

DWIDGET_END_NAMESPACE


#endif // DPUSHBUTTON_H
