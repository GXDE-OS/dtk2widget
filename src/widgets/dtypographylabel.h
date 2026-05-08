#ifndef DTYPOGRAPHYLABEL_H
#define DTYPOGRAPHYLABEL_H

#include "dtkwidget_global.h"

#include <QLabel>
#include <QWidget>

DWIDGET_BEGIN_NAMESPACE

class LIBDTKWIDGETSHARED_EXPORT DTypographyLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(TextRole textRole READ textRole WRITE setTextRole)
    Q_PROPERTY(bool emphasis READ emphasis WRITE setEmphasis)
    Q_PROPERTY(bool secondary READ isSecondary WRITE setSecondary)

public:
    enum TextRole {
        LargeTitle,
        Title,
        Body,
        Caption
    };
    Q_ENUM(TextRole)

    explicit DTypographyLabel(QWidget *parent = nullptr);
    explicit DTypographyLabel(const QString &text, QWidget *parent = nullptr);
    DTypographyLabel(const QString &text, TextRole role, QWidget *parent = nullptr);

    TextRole textRole() const;
    bool emphasis() const;
    bool isSecondary() const;

public Q_SLOTS:
    void setTextRole(TextRole role);
    void setEmphasis(bool emphasis);
    void setSecondary(bool secondary);

protected:
    void changeEvent(QEvent *event) override;

private:
    void updateTypography();
    void updatePalette();

    TextRole m_textRole = Body;
    bool m_emphasis = false;
    bool m_secondary = false;
};

class LIBDTKWIDGETSHARED_EXPORT DSectionTitle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    explicit DSectionTitle(QWidget *parent = nullptr);
    explicit DSectionTitle(const QString &text, QWidget *parent = nullptr);

    QString text() const;
    QSize sizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    QFont titleFont() const;

    QString m_text;
};

DWIDGET_END_NAMESPACE

#endif // DTYPOGRAPHYLABEL_H
