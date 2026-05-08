#ifndef DFLOATINGMESSAGE_H
#define DFLOATINGMESSAGE_H

#include "dfloatingwidget.h"

#include <QIcon>

class QLabel;
class QHBoxLayout;
class QTimer;

DWIDGET_BEGIN_NAMESPACE

class DTypographyLabel;
class DIconButton;

class LIBDTKWIDGETSHARED_EXPORT DFloatingMessage : public DFloatingWidget
{
    Q_OBJECT
    Q_PROPERTY(MessageType messageType READ messageType)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(int duration READ duration WRITE setDuration)

public:
    enum MessageType {
        TransientType,
        ResidentType
    };
    Q_ENUM(MessageType)

    explicit DFloatingMessage(MessageType type = TransientType, QWidget *parent = nullptr);

    MessageType messageType() const;
    QString message() const;
    QIcon icon() const;
    int duration() const;

    void setActionWidget(QWidget *widget);

public Q_SLOTS:
    void setMessage(const QString &message);
    void setIcon(const QIcon &icon);
    void setDuration(int msec);

Q_SIGNALS:
    void closeButtonClicked();
    void messageClosed();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void initUi();

    MessageType m_type = TransientType;
    QWidget *m_content = nullptr;
    QHBoxLayout *m_layout = nullptr;
    QLabel *m_iconLabel = nullptr;
    DTypographyLabel *m_messageLabel = nullptr;
    DIconButton *m_closeButton = nullptr;
    QWidget *m_actionWidget = nullptr;
    QTimer *m_timer = nullptr;
    QIcon m_icon;
    int m_duration = 4000;
};

DWIDGET_END_NAMESPACE

#endif // DFLOATINGMESSAGE_H
