/*
 * Copyright (C) 2015 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DDIALOG_H
#define DDIALOG_H

#include <QIcon>

#include "dabstractdialog.h"

class QAbstractButton;
class QButtonGroup;
class QLabel;
class QCloseEvent;
class QVBoxLayout;

DWIDGET_BEGIN_NAMESPACE

class DDialogPrivate;
class DDialog : public DAbstractDialog
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QPixmap iconPixmap READ iconPixmap WRITE setIconPixmap)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY textFormatChanged)
    Q_PROPERTY(bool onButtonClickedClose READ onButtonClickedClose WRITE setOnButtonClickedClose)

public:
    enum ButtonType {
        ButtonNormal,
        ButtonWarning,
        ButtonRecommend
    };

    explicit DDialog(QWidget *parent = 0);
    explicit DDialog(const QString &title, const QString& message, QWidget *parent = 0);

    int getButtonIndexByText(const QString &text) const;
    int buttonCount() const;
    int contentCount() const;
    QList<QAbstractButton*> getButtons() const;
    QList<QWidget*> getContents() const;
    QAbstractButton* getButton(int index) const;
    QWidget* getContent(int index) const;
    QString title() const;
    QString message() const;
    QIcon icon() const;
    QPixmap iconPixmap() const;
    Qt::TextFormat textFormat() const;
    bool onButtonClickedClose() const;

    void setContentLayoutContentsMargins(const QMargins &margins);
    QMargins contentLayoutContentsMargins() const;

Q_SIGNALS:
    void aboutToClose();
    void closed();
    void buttonClicked(int index, const QString &text);
    void titleChanged(QString title);
    void messageChanged(QString massage);
    void textFormatChanged(Qt::TextFormat textFormat);
    void sizeChanged(QSize size);
    void visibleChanged(bool visible);

public Q_SLOTS:
    int addButton(const QString &text, bool isDefault = false, ButtonType type = ButtonNormal);
    int addButtons(const QStringList &text);
    void insertButton(int index, const QString &text, bool isDefault = false, ButtonType type = ButtonNormal);
    void insertButton(int index, QAbstractButton* button, bool isDefault = false);
    void insertButtons(int index, const QStringList &text);
    void removeButton(int index);
    void removeButton(QAbstractButton *button);
    void removeButtonByText(const QString &text);
    void clearButtons();
    bool setDefaultButton(int index);
    bool setDefaultButton(const QString &str);
    void setDefaultButton(QAbstractButton *button);
    void addContent(QWidget *widget, Qt::Alignment alignment = Qt::AlignLeft);
    void insertContent(int index, QWidget *widget, Qt::Alignment alignment = Qt::AlignLeft);
    void removeContent(QWidget *widget, bool isDelete = true);
    void clearContents(bool isDelete = true);
    void setSpacing(int spacing);
    void addSpacing(int spacing);
    void insertSpacing(int index, int spacing);
    void clearSpacing();
    void setButtonText(int index, const QString &text);
    void setButtonIcon(int index, const QIcon &icon);
    void setTitle(const QString &title);
    void setMessage(const QString& message);
    Q_DECL_DEPRECATED void setIcon(const QIcon &icon);
    void setIcon(const QIcon &icon, const QSize &expectedSize);
    void setIconPixmap(const QPixmap &iconPixmap);
    void setTextFormat(Qt::TextFormat textFormat);
    void setOnButtonClickedClose(bool onButtonClickedClose);

    int exec() Q_DECL_OVERRIDE;

protected:
    explicit DDialog(DDialogPrivate &dd, QWidget *parent = 0);

    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) override;
    void childEvent(QChildEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void updateTextLayout();

    D_DECLARE_PRIVATE(DDialog)

    Q_PRIVATE_SLOT(d_func(), void _q_onButtonClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_defaultButtonTriggered())
};

DWIDGET_END_NAMESPACE

#endif // DDIALOG_H

