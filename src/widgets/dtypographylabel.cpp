#include "dtypographylabel.h"

#include "dthememanager.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>

DWIDGET_BEGIN_NAMESPACE

DTypographyLabel::DTypographyLabel(QWidget *parent)
    : QLabel(parent)
{
    DThemeManager::registerWidget(this);
    updateTypography();
}

DTypographyLabel::DTypographyLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    DThemeManager::registerWidget(this);
    updateTypography();
}

DTypographyLabel::DTypographyLabel(const QString &text, TextRole role, QWidget *parent)
    : QLabel(text, parent)
    , m_textRole(role)
{
    DThemeManager::registerWidget(this);
    updateTypography();
}

DTypographyLabel::TextRole DTypographyLabel::textRole() const
{
    return m_textRole;
}

bool DTypographyLabel::emphasis() const
{
    return m_emphasis;
}

bool DTypographyLabel::isSecondary() const
{
    return m_secondary;
}

void DTypographyLabel::setTextRole(TextRole role)
{
    if (m_textRole == role)
        return;

    m_textRole = role;
    updateTypography();
}

void DTypographyLabel::setEmphasis(bool emphasis)
{
    if (m_emphasis == emphasis)
        return;

    m_emphasis = emphasis;
    updateTypography();
}

void DTypographyLabel::setSecondary(bool secondary)
{
    if (m_secondary == secondary)
        return;

    m_secondary = secondary;
    updatePalette();
}

void DTypographyLabel::changeEvent(QEvent *event)
{
    QLabel::changeEvent(event);

    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::FontChange)
        updateTypography();
}

void DTypographyLabel::updateTypography()
{
    QFont f = font();

    switch (m_textRole) {
    case LargeTitle:
        f.setPixelSize(18);
        f.setWeight(QFont::DemiBold);
        break;
    case Title:
        f.setPixelSize(14);
        f.setWeight(m_emphasis ? QFont::DemiBold : QFont::Medium);
        break;
    case Body:
        f.setPixelSize(12);
        f.setWeight(m_emphasis ? QFont::DemiBold : QFont::Normal);
        break;
    case Caption:
        f.setPixelSize(11);
        f.setWeight(m_emphasis ? QFont::Medium : QFont::Normal);
        break;
    }

    QLabel::setFont(f);
    updatePalette();
    updateGeometry();
}

void DTypographyLabel::updatePalette()
{
    QPalette p = palette();
    QColor color = p.color(QPalette::WindowText);
    if (m_secondary)
        color.setAlpha(150);
    p.setColor(QPalette::WindowText, color);
    setPalette(p);
}

DSectionTitle::DSectionTitle(QWidget *parent)
    : QWidget(parent)
{
    DThemeManager::registerWidget(this);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

DSectionTitle::DSectionTitle(const QString &text, QWidget *parent)
    : QWidget(parent)
    , m_text(text)
{
    DThemeManager::registerWidget(this);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

QString DSectionTitle::text() const
{
    return m_text;
}

QSize DSectionTitle::sizeHint() const
{
    QFontMetrics fm(titleFont());
    return QSize(fm.horizontalAdvance(m_text) + 80, qMax(24, fm.height() + 6));
}

void DSectionTitle::setText(const QString &text)
{
    if (m_text == text)
        return;

    m_text = text;
    updateGeometry();
    update();
}

void DSectionTitle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(titleFont());

    QFontMetrics fm(painter.font());
    const int textWidth = fm.horizontalAdvance(m_text);
    const QRect textRect(0, 0, textWidth, height());
    const int lineX = textRect.right() + 14;
    const int lineY = height() / 2;

    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_text);

    QColor lineColor = palette().color(QPalette::Mid);
    lineColor.setAlpha(90);
    painter.setPen(QPen(lineColor, 1));
    painter.drawLine(lineX, lineY, width(), lineY);
}

void DSectionTitle::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::FontChange) {
        updateGeometry();
        update();
    }
}

QFont DSectionTitle::titleFont() const
{
    QFont f = font();
    f.setPixelSize(14);
    f.setWeight(QFont::DemiBold);
    return f;
}

DWIDGET_END_NAMESPACE
