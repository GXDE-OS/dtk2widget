#include "containertab.h"

#include "dbackgroundgroup.h"
#include "dcardwidget.h"
#include "dfloatingwidget.h"
#include "dframe.h"
#include "dswitchbutton.h"
#include "dtypographylabel.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

static QLabel *createTitle(const QString &text, QWidget *parent)
{
    DTypographyLabel *label = new DTypographyLabel(text, DTypographyLabel::Title, parent);
    label->setEmphasis(true);
    return label;
}

static QLabel *createDescription(const QString &text, QWidget *parent)
{
    DTypographyLabel *label = new DTypographyLabel(text, DTypographyLabel::Body, parent);
    label->setWordWrap(true);
    label->setSecondary(true);
    return label;
}

static QWidget *createRow(const QString &title, const QString &description, QWidget *rightWidget, QWidget *parent)
{
    QWidget *row = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(row);
    QVBoxLayout *textLayout = new QVBoxLayout;

    QLabel *titleLabel = new QLabel(title, row);
    QLabel *descriptionLabel = createDescription(description, row);

    textLayout->setSpacing(2);
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descriptionLabel);

    layout->setContentsMargins(14, 10, 14, 10);
    layout->addLayout(textLayout);
    layout->addStretch();
    if (rightWidget)
        layout->addWidget(rightWidget);

    return row;
}

ContainerTab::ContainerTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(18);

    QLabel *heading = createTitle("Container widgets", this);
    QLabel *intro = createDescription("Reusable DTK2 surfaces collected from application-local patterns. These examples use small radii, flat fills, restrained shadows, and blur only where hierarchy benefits from it.", this);
    mainLayout->addWidget(heading);
    mainLayout->addWidget(intro);

    DFrame *plainFrame = new DFrame(this);
    plainFrame->setMinimumHeight(86);
    QVBoxLayout *plainLayout = new QVBoxLayout(plainFrame);
    plainLayout->setContentsMargins(18, 14, 18, 14);
    plainLayout->addWidget(createTitle("DFrame", plainFrame));
    plainLayout->addWidget(createDescription("Palette-aware rounded base frame for simple grouped regions.", plainFrame));
    mainLayout->addWidget(plainFrame);

    DCardWidget *card = new DCardWidget(this);
    card->setMinimumHeight(116);
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 14, 18, 14);
    cardLayout->setSpacing(8);
    cardLayout->addWidget(createTitle("DCardWidget", card));
    cardLayout->addWidget(createDescription("Use cards for content that needs slightly stronger emphasis than the page background.", card));
    QPushButton *cardButton = new QPushButton("Primary action", card);
    cardButton->setFixedWidth(140);
    cardLayout->addWidget(cardButton, 0, Qt::AlignLeft);
    mainLayout->addWidget(card);

    DBackgroundGroup *group = new DBackgroundGroup(new QVBoxLayout, this);
    group->layout()->setContentsMargins(0, 0, 0, 0);
    group->setItemMargins(QMargins(0, 0, 0, 0));
    group->setItemSpacing(1);
    group->setMinimumHeight(166);
    group->layout()->addWidget(createRow("DBackgroundGroup", "Paint a shared rounded background behind multiple rows.", new DSwitchButton(group), group));
    group->layout()->addWidget(createRow("Separated but flat", "Rows keep clear hierarchy without custom per-app frames.", new QPushButton("Edit", group), group));
    group->layout()->addWidget(createRow("Small radius", "Corners stay compact to match the current DTK2 visual style.", new QPushButton("Open", group), group));
    mainLayout->addWidget(group);

    DCardWidget *typographyCard = new DCardWidget(this);
    QVBoxLayout *typographyLayout = new QVBoxLayout(typographyCard);
    typographyLayout->setContentsMargins(18, 14, 18, 14);
    typographyLayout->setSpacing(8);
    typographyLayout->addWidget(new DSectionTitle("DTypographyLabel", typographyCard));
    typographyLayout->addWidget(new DTypographyLabel("LargeTitle: page or dialog focus", DTypographyLabel::LargeTitle, typographyCard));
    typographyLayout->addWidget(new DTypographyLabel("Title: grouped content title", DTypographyLabel::Title, typographyCard));
    typographyLayout->addWidget(new DTypographyLabel("Body: normal readable copy", DTypographyLabel::Body, typographyCard));
    DTypographyLabel *caption = new DTypographyLabel("Caption: supporting and secondary information", DTypographyLabel::Caption, typographyCard);
    caption->setSecondary(true);
    typographyLayout->addWidget(caption);
    mainLayout->addWidget(typographyCard);

    QWidget *floatingArea = new QWidget(this);
    floatingArea->setMinimumHeight(156);
    floatingArea->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #e8edf6, stop:1 #f7f3ea); border-radius: 6px;");
    QHBoxLayout *floatingAreaLayout = new QHBoxLayout(floatingArea);
    floatingAreaLayout->setContentsMargins(30, 22, 30, 22);

    DFloatingWidget *floating = new DFloatingWidget(floatingArea);
    QWidget *floatingContent = new QWidget(floating);
    QVBoxLayout *floatingLayout = new QVBoxLayout(floatingContent);
    floatingLayout->setContentsMargins(4, 2, 4, 2);
    floatingLayout->setSpacing(4);
    floatingLayout->addWidget(createTitle("DFloatingWidget", floatingContent));
    floatingLayout->addWidget(createDescription("Blur-capable elevated surface for popovers and transient panels.", floatingContent));
    floating->setWidget(floatingContent);
    floating->setBlurBackgroundEnabled(true);
    floating->setFixedWidth(360);

    floatingAreaLayout->addWidget(floating, 0, Qt::AlignCenter);
    mainLayout->addWidget(floatingArea);
    mainLayout->addStretch();
}
