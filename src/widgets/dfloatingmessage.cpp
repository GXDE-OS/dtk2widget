#include "dfloatingmessage.h"

#include "dbuttonlist.h"
#include "dtypographylabel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QShowEvent>
#include <QTimer>

DWIDGET_BEGIN_NAMESPACE

DFloatingMessage::DFloatingMessage(MessageType type, QWidget *parent)
    : DFloatingWidget(parent)
    , m_type(type)
{
    initUi();
}

DFloatingMessage::MessageType DFloatingMessage::messageType() const
{
    return m_type;
}

QString DFloatingMessage::message() const
{
    return m_messageLabel->text();
}

QIcon DFloatingMessage::icon() const
{
    return m_icon;
}

int DFloatingMessage::duration() const
{
    return m_duration;
}

void DFloatingMessage::setActionWidget(QWidget *widget)
{
    if (m_actionWidget == widget)
        return;

    if (m_actionWidget) {
        m_layout->removeWidget(m_actionWidget);
        m_actionWidget->deleteLater();
    }

    m_actionWidget = widget;
    if (!m_actionWidget)
        return;

    m_actionWidget->setParent(m_content);
    const int insertIndex = m_closeButton ? m_layout->indexOf(m_closeButton) : m_layout->count();
    m_layout->insertWidget(insertIndex, m_actionWidget);
}

void DFloatingMessage::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
    updateGeometry();
}

void DFloatingMessage::setIcon(const QIcon &icon)
{
    m_icon = icon;
    m_iconLabel->setVisible(!m_icon.isNull());
    if (!m_icon.isNull())
        m_iconLabel->setPixmap(m_icon.pixmap(20, 20));
    updateGeometry();
}

void DFloatingMessage::setDuration(int msec)
{
    m_duration = qMax(0, msec);
    if (m_timer)
        m_timer->setInterval(m_duration);
}

void DFloatingMessage::showEvent(QShowEvent *event)
{
    if (m_timer && m_duration > 0)
        m_timer->start();

    DFloatingWidget::showEvent(event);
}

void DFloatingMessage::initUi()
{
    setBlurBackgroundEnabled(true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setMinimumHeight(44);
    setShadowBlurRadius(22);
    setShadowOffset(QPointF(0, 4));

    m_content = new QWidget(this);
    m_layout = new QHBoxLayout(m_content);
    m_iconLabel = new QLabel(m_content);
    m_messageLabel = new DTypographyLabel(QString(), DTypographyLabel::Body, m_content);

    m_messageLabel->setWordWrap(true);
    m_iconLabel->setFixedSize(20, 20);
    m_iconLabel->setVisible(false);

    m_layout->setContentsMargins(2, 0, 2, 0);
    m_layout->setSpacing(10);
    m_layout->addWidget(m_iconLabel);
    m_layout->addWidget(m_messageLabel, 1);

    if (m_type == ResidentType) {
        m_closeButton = new DIconButton(m_content);
        m_closeButton->setText(QStringLiteral("×"));
        m_closeButton->setFlat(true);
        m_closeButton->setCircleEnabled(true);
        m_closeButton->setFixedSize(24, 24);
        connect(m_closeButton, &DIconButton::clicked, this, [this] {
            close();
            Q_EMIT closeButtonClicked();
            Q_EMIT messageClosed();
        });
        m_layout->addWidget(m_closeButton);
    } else {
        m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        m_timer->setInterval(m_duration);
        connect(m_timer, &QTimer::timeout, this, [this] {
            close();
            Q_EMIT messageClosed();
        });
    }

    setWidget(m_content);
}

DWIDGET_END_NAMESPACE
