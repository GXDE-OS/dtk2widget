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

#include "dbuttonlist.h"
#include "dthememanager.h"
#include "dconstants.h"
#include <QListWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QEvent>
#include <QDebug>

DWIDGET_BEGIN_NAMESPACE

DIconButton::DIconButton(QWidget *parent)
    : QPushButton(parent)
    , m_iconLabel(new QLabel(this))
{
    setFlat(true);
    setIconSize(QSize(20, 20));
    m_iconLabel->hide();
}

DIconButton::DIconButton(const QIcon &icon, QWidget *parent)
    : DIconButton(parent)
{
    setIcon(icon);
}

DIconButton::DIconButton(QStyle::StandardPixmap iconType, QWidget *parent)
    : DIconButton(parent)
{
    setIcon(iconType);
}

DIconButton::DIconButton(const QString &Icon, const QString &text, QWidget *parent):
    QPushButton(text, parent),
    m_icon(Icon),
    m_text(text)
{
    initIconLabel();
    initConnect();
}

bool DIconButton::circleEnabled() const
{
    return m_circleEnabled;
}

bool DIconButton::hasNewNotification() const
{
    return m_newNotification;
}

void DIconButton::setIcon(const QIcon &icon)
{
    QPushButton::setIcon(icon);
    updateGeometry();
    update();
}

void DIconButton::setIcon(QStyle::StandardPixmap iconType)
{
    setIcon(style()->standardIcon(iconType, nullptr, this));
}

void DIconButton::setCircleEnabled(bool enabled)
{
    if (m_circleEnabled == enabled)
        return;

    m_circleEnabled = enabled;
    updateGeometry();
    update();
}

void DIconButton::setNewNotification(bool hasNotification)
{
    if (m_newNotification == hasNotification)
        return;

    m_newNotification = hasNotification;
    update();
}

void DIconButton::initConnect(){
    connect(this, SIGNAL(toggled(bool)), m_iconLabel, SLOT(setVisible(bool)));
}

void DIconButton::initIconLabel(){
    m_iconLabel = new QLabel(this);
    setIconLeftMargin(20);
    m_iconLabel->hide();
    QImage image(m_icon);
    m_iconLabel->setPixmap(QPixmap::fromImage(image));
    m_iconLabel->setFixedSize(image.size());
}

void DIconButton::setIconLeftMargin(int leftMargin){
    m_iconLabel->move(leftMargin, y());
}

QSize DIconButton::sizeHint() const
{
    const QSize base = QPushButton::sizeHint();
    const int side = qMax(base.width(), base.height());
    return m_circleEnabled ? QSize(side, side) : base;
}

void DIconButton::paintEvent(QPaintEvent *event)
{
    if (!m_circleEnabled && !m_newNotification) {
        QPushButton::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_circleEnabled) {
        QStyleOptionButton option;
        initStyleOption(&option);
        QColor background = palette().color(QPalette::Button);
        if (option.state & QStyle::State_MouseOver)
            background = palette().color(QPalette::Light);
        if (option.state & QStyle::State_Sunken)
            background = palette().color(QPalette::Mid);

        painter.setPen(Qt::NoPen);
        painter.setBrush(background);
        painter.drawEllipse(rect().adjusted(1, 1, -1, -1));

        const QSize iconSz = iconSize().isValid() ? iconSize() : QSize(20, 20);
        const QRect iconArea(QPoint((width() - iconSz.width()) / 2, (height() - iconSz.height()) / 2), iconSz);
        const QIcon::Mode mode = isEnabled() ? (underMouse() ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
        icon().paint(&painter, iconArea, Qt::AlignCenter, mode, isChecked() ? QIcon::On : QIcon::Off);
    } else {
        QPushButton::paintEvent(event);
    }

    if (m_newNotification) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 87, 87));
        painter.drawEllipse(QRect(width() - 9, 3, 6, 6));
    }
}

void DIconButton::resizeEvent(QResizeEvent *event){
    int height = event->size().height();
    m_iconLabel->move(m_iconLabel->x(), (height - m_iconLabel->height())/ 2 );
    QPushButton::resizeEvent(event);
}

void DIconButton::setIconLabel(const QString &icon){
    m_icon = icon;
    QImage image(m_icon);
    m_iconLabel->setPixmap(QPixmap::fromImage(image));
    m_iconLabel->setFixedSize(image.size());
}

void DIconButton::hideIconLabel(){
    m_iconLabel->hide();
}

void DIconButton::updateStyle(){
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void DIconButton::enterEvent(QEvent *event){
    Q_EMIT mouseEntered(text());
    QPushButton::enterEvent(event);
}

void DIconButton::leaveEvent(QEvent *event){
    Q_EMIT mouseLeaved(text());
    QPushButton::leaveEvent(event);
}

void DIconButton::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (hasFocus()) {
            click();
            return;
        }
        break;
    default:
        break;
    }

    QPushButton::keyPressEvent(event);
}

/*!
 * \class DButtonList
 *
 * \~chinese \brief 已废弃，请考虑配合 QButtonGroup 使用 QRadioButton 来替代。
 * \~english \brief Deprecated. Consider use QRadioButton with QButtonGroup instead.
 *
 * \~chinese \deprecated 请考虑配合 QButtonGroup 使用 QRadioButton 来替代。
 * \~english \deprecated Consider use QRadioButton with QButtonGroup instead.
*/

DButtonList::DButtonList(QWidget *parent) : QListWidget(parent)
{
    DThemeManager::registerWidget(this);

    setSelectionMode(DButtonList::NoSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerItem);
    setResizeMode(Adjust);

    setItemSize(200, BUTTON_HEIGHT);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);

    initConnect();
}


void DButtonList::initMargins(int leftMargin, int rightMargin, int imageLeftMargin){
    m_leftMargin = leftMargin;
    m_rightMargin = rightMargin;
    m_imageLeftMargin = imageLeftMargin;
}

DButtonList::~DButtonList()
{

}

void DButtonList::initConnect(){
    connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setButtonChecked(int)));
}

void DButtonList::setItemWidth(int width){
    setGridSize(QSize(width, gridSize().height()));
    setFixedWidth(gridSize().width());
}

void DButtonList::setItemHeight(int height){
    setGridSize(QSize(gridSize().width(), height));
}

void DButtonList::setItemSize(int width, int height){
    setItemSize(QSize(width, height));
}

void DButtonList::setItemSize(QSize size){
    setGridSize(size);
    setFixedWidth(gridSize().width());

    for(int i=0; i< count(); i++){
        itemWidget(item(i))->setFixedHeight(size.height());
    }
}

void DButtonList::addButton(const QString &label){
    int index = count();
    addButton(label, index);
}

void DButtonList::addButton(const QString &label, int index){
    DIconButton* button = new DIconButton(":/images/dark/images/tick_hover.png", label, this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button->setCheckable(true);
    connect(button, SIGNAL(mouseEntered(QString)), this, SIGNAL(buttonMouseEntered(QString)));
    connect(button, SIGNAL(mouseLeaved(QString)), this, SIGNAL(buttonMouseLeaved(QString)));
    QFrame* borderFrame = new QFrame;
    borderFrame->setObjectName("BorderFrame");
    QVBoxLayout* borderLayout = new QVBoxLayout;
    borderLayout->addWidget(button);
    borderLayout->setSpacing(0);
    borderLayout->setContentsMargins(m_leftMargin, 0, m_rightMargin, 0);
    borderFrame->setLayout(borderLayout);

    m_buttonGroup->addButton(button, index);
    QListWidgetItem* item = new QListWidgetItem(this);
    addItem(item);
    setItemWidget(item, borderFrame);

    setItemSize(gridSize());

}


void DButtonList::addButtons(const QStringList &listLabels){
    for(int i= 0; i< listLabels.length(); i++) {
        addButton(listLabels.at(i), i);
    }
}

void DButtonList::setButtonChecked(int id){
   if (id < m_buttonGroup->buttons().length()){
       DIconButton* button = reinterpret_cast<DIconButton*>(m_buttonGroup->button(id));
       button->setChecked(true);
       if (m_buttonGroup->buttons().length() == 1){
            button->setProperty("state", "OnlyOne");
            button->hideIconLabel();
       }else{
            button->setProperty("state", "normal");
       }
       button->updateStyle();
       Q_EMIT buttonCheckedIndexChanged(id);
       Q_EMIT buttonChecked(button->text());
   }
}

void DButtonList::checkButtonByIndex(int index){
    if (index < m_buttonGroup->buttons().length()){
        DIconButton* button = reinterpret_cast<DIconButton*>(m_buttonGroup->button(index));
        button->click();
    }
}

DIconButton* DButtonList::getButtonByIndex(int index){
    if (index < m_buttonGroup->buttons().length()){
        DIconButton* button = reinterpret_cast<DIconButton*>(m_buttonGroup->button(index));
        return button;
    }else{
        qWarning() << "There is no this index:" << index;
    }
    return NULL;
}

void DButtonList::clear(){
    Q_FOREACH (QAbstractButton* button, m_buttonGroup->buttons()) {
//        qDebug() << static_cast<IconButton*>(button)->text();
        static_cast<DIconButton*>(button)->disconnect();
        m_buttonGroup->removeButton(static_cast<DIconButton*>(button));
    }
    QListWidget::clear();
}


DWIDGET_END_NAMESPACE
