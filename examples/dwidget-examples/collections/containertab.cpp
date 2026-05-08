#include "containertab.h"

#include "dbackgroundgroup.h"
#include "dblursurface.h"
#include "dcardwidget.h"
#include "ddocumenttabbar.h"
#include "dfloatingmessage.h"
#include "dfloatingwidget.h"
#include "dframe.h"
#include "dbuttonlist.h"
#include "dhoverbutton.h"
#include "dicontextbutton.h"
#include "dswitchbutton.h"
#include "dtabbedstackwidget.h"
#include "dtypographylabel.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
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

static void loadTabsPage(QTabWidget *tabs, int index)
{
    QWidget *page = tabs->widget(index);
    if (!page || page->property("loaded").toBool())
        return;

    page->setProperty("loaded", true);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    switch (index) {
    case 0:
        layout->addWidget(createDescription("Open one tab component at a time. This isolates DTabbedStackWidget from DDocumentTabBar.", page));
        layout->addStretch();
        break;
    case 1: {
        DCardWidget *tabbedCard = new DCardWidget(page);
        QVBoxLayout *tabbedLayout = new QVBoxLayout(tabbedCard);
        tabbedLayout->setContentsMargins(18, 14, 18, 14);
        tabbedLayout->setSpacing(10);
        tabbedLayout->addWidget(new DSectionTitle("DTabbedStackWidget", tabbedCard));

        DTabbedStackWidget *tabbedStack = new DTabbedStackWidget(tabbedCard);
        tabbedStack->addPage(createRow("Home", "Start page content can keep its own monitor or refresh logic.", new QPushButton("Refresh", tabbedStack), tabbedStack), QIcon::fromTheme("go-home"), "Home");
        tabbedStack->addPage(createRow("Cleaner", "Applications can switch pages without manually wiring tab and stack indexes.", new QPushButton("Scan", tabbedStack), tabbedStack), QIcon::fromTheme("edit-clear"), "Cleaner");
        tabbedStack->addPage(createRow("Tools", "This covers the system-assistant titlebar tab plus stacked layout pattern.", new QPushButton("Open", tabbedStack), tabbedStack), QIcon::fromTheme("applications-system"), "Tools");
        tabbedLayout->addWidget(tabbedStack);

        layout->addWidget(tabbedCard);
        layout->addStretch();
        break;
    }
    case 2: {
        DCardWidget *documentTabCard = new DCardWidget(page);
        QVBoxLayout *documentTabLayout = new QVBoxLayout(documentTabCard);
        documentTabLayout->setContentsMargins(18, 14, 18, 14);
        documentTabLayout->setSpacing(10);
        documentTabLayout->addWidget(new DSectionTitle("DDocumentTabBar", documentTabCard));

        DDocumentTabBar *documentTabBar = new DDocumentTabBar(documentTabCard);
        documentTabBar->setDragable(false);
        documentTabBar->setVisibleAddButton(false);
        documentTabBar->addDocument("/tmp/notes.txt", QIcon::fromTheme("text-x-generic"), "notes.txt");
        documentTabBar->addDocument("/tmp/report.md", QIcon::fromTheme("text-markdown"), "report.md");
        documentTabBar->addDocument("/tmp/readme.txt", QIcon::fromTheme("text-x-generic"), "readme.txt");
        documentTabBar->setDocumentModified(1, true);
        documentTabBar->setFixedHeight(40);
        documentTabLayout->addWidget(documentTabBar);
        documentTabLayout->addWidget(createDescription("Document tabs are shown with drag and add-button disabled in the example to keep diagnostics focused.", documentTabCard));

        layout->addWidget(documentTabCard);
        layout->addStretch();
        break;
    }
    default:
        layout->addWidget(createDescription("Unknown tabs page.", page));
        layout->addStretch();
        break;
    }
}

static void loadContainerPage(QTabWidget *tabs, int index)
{
    QWidget *page = tabs->widget(index);
    if (!page || page->property("loaded").toBool())
        return;

    page->setProperty("loaded", true);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    switch (index) {
    case 0:
        layout->addWidget(createTitle("Container widgets", page));
        layout->addWidget(createDescription("Open one component page at a time. This keeps startup light and helps isolate any widget that blocks the UI.", page));
        layout->addStretch();
        break;
    case 1: {
        DFrame *plainFrame = new DFrame(page);
        plainFrame->setMinimumHeight(86);
        QVBoxLayout *plainLayout = new QVBoxLayout(plainFrame);
        plainLayout->setContentsMargins(18, 14, 18, 14);
        plainLayout->addWidget(createTitle("DFrame", plainFrame));
        plainLayout->addWidget(createDescription("Palette-aware rounded base frame for simple grouped regions.", plainFrame));

        DCardWidget *card = new DCardWidget(page);
        card->setMinimumHeight(116);
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 14, 18, 14);
        cardLayout->setSpacing(8);
        cardLayout->addWidget(createTitle("DCardWidget", card));
        cardLayout->addWidget(createDescription("Use cards for content that needs slightly stronger emphasis than the page background.", card));
        QPushButton *cardButton = new QPushButton("Primary action", card);
        cardButton->setFixedWidth(140);
        cardLayout->addWidget(cardButton, 0, Qt::AlignLeft);

        layout->addWidget(plainFrame);
        layout->addWidget(card);
        layout->addStretch();
        break;
    }
    case 2: {
        DBackgroundGroup *group = new DBackgroundGroup(new QVBoxLayout, page);
        group->layout()->setContentsMargins(0, 0, 0, 0);
        group->setItemMargins(QMargins(0, 0, 0, 0));
        group->setItemSpacing(1);
        group->setMinimumHeight(166);
        group->layout()->addWidget(createRow("DBackgroundGroup", "Paint a shared rounded background behind multiple rows.", new DSwitchButton(group), group));
        group->layout()->addWidget(createRow("Separated but flat", "Rows keep clear hierarchy without custom per-app frames.", new QPushButton("Edit", group), group));
        group->layout()->addWidget(createRow("Small radius", "Corners stay compact to match the current DTK2 visual style.", new QPushButton("Open", group), group));
        layout->addWidget(group);
        layout->addStretch();
        break;
    }
    case 3: {
        DCardWidget *typographyCard = new DCardWidget(page);
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
        layout->addWidget(typographyCard);
        layout->addStretch();
        break;
    }
    case 4: {
        DCardWidget *buttonCard = new DCardWidget(page);
        QVBoxLayout *buttonLayout = new QVBoxLayout(buttonCard);
        buttonLayout->setContentsMargins(18, 14, 18, 14);
        buttonLayout->setSpacing(10);
        buttonLayout->addWidget(new DSectionTitle("Hover and icon-text buttons", buttonCard));

        QHBoxLayout *buttonRow = new QHBoxLayout;
        DHoverButton *hoverButton = new DHoverButton(buttonCard);
        hoverButton->setNormalIcon(QIcon::fromTheme("go-previous"));
        hoverButton->setHoverIcon(QIcon::fromTheme("go-next"));
        hoverButton->setPressedIcon(QIcon::fromTheme("go-down"));
        hoverButton->setFixedSize(42, 32);

        DIconTextButton *iconTextButton = new DIconTextButton(QIcon::fromTheme("document-open"), "Open file", buttonCard);
        iconTextButton->setIconTextSpacing(8);

        DIconButton *iconButton = new DIconButton(QIcon::fromTheme("view-refresh"), buttonCard);
        iconButton->setCircleEnabled(true);
        iconButton->setNewNotification(true);
        iconButton->setFixedSize(34, 34);

        buttonRow->addWidget(hoverButton);
        buttonRow->addWidget(iconTextButton);
        buttonRow->addWidget(iconButton);
        buttonRow->addStretch();
        buttonLayout->addLayout(buttonRow);
        buttonLayout->addWidget(createDescription("These cover repeated file-manager and viewer patterns without importing app-specific menu or theme logic.", buttonCard));
        layout->addWidget(buttonCard);
        layout->addStretch();
        break;
    }
    case 5: {
        DCardWidget *messageCard = new DCardWidget(page);
        QVBoxLayout *messageLayout = new QVBoxLayout(messageCard);
        messageLayout->setContentsMargins(18, 14, 18, 14);
        messageLayout->setSpacing(10);
        messageLayout->addWidget(new DSectionTitle("DFloatingMessage", messageCard));

        DFloatingMessage *transientMessage = new DFloatingMessage(DFloatingMessage::TransientType, messageCard);
        transientMessage->setIcon(QIcon::fromTheme("dialog-information"));
        transientMessage->setMessage("Transient message: auto closes after the configured duration.");
        transientMessage->setDuration(5000);

        DFloatingMessage *residentMessage = new DFloatingMessage(DFloatingMessage::ResidentType, messageCard);
        residentMessage->setIcon(QIcon::fromTheme("dialog-warning"));
        residentMessage->setMessage("Resident message: keeps focus until the user closes it.");
        residentMessage->setActionWidget(new QPushButton("Action", residentMessage));

        messageLayout->addWidget(transientMessage);
        messageLayout->addWidget(residentMessage);
        messageLayout->addWidget(createDescription("Use this for reusable in-window notifications before moving app-specific toast logic into DTK2.", messageCard));
        layout->addWidget(messageCard);
        layout->addStretch();
        break;
    }
    case 6: {
        QWidget *blurArea = new QWidget(page);
        blurArea->setMinimumHeight(142);
        blurArea->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #d9ecff, stop:0.48 #f4e7ff, stop:1 #fff0d7); border-radius: 6px;");
        QHBoxLayout *blurAreaLayout = new QHBoxLayout(blurArea);
        blurAreaLayout->setContentsMargins(28, 22, 28, 22);

        DBlurSurface *blurSurface = new DBlurSurface(blurArea);
        blurSurface->setFixedWidth(420);
        blurSurface->setMaskAlpha(96);
        QVBoxLayout *blurSurfaceLayout = new QVBoxLayout(blurSurface);
        blurSurfaceLayout->setContentsMargins(18, 14, 18, 14);
        blurSurfaceLayout->setSpacing(6);
        blurSurfaceLayout->addWidget(createTitle("DBlurSurface", blurSurface));
        blurSurfaceLayout->addWidget(createDescription("Reusable translucent panel for control-center style grouped content.", blurSurface));
        blurAreaLayout->addWidget(blurSurface, 0, Qt::AlignCenter);
        layout->addWidget(blurArea);
        layout->addStretch();
        break;
    }
    case 7: {
        QTabWidget *tabPages = new QTabWidget(page);
        tabPages->setDocumentMode(true);
        tabPages->addTab(new QWidget(tabPages), "Overview");
        tabPages->addTab(new QWidget(tabPages), "Stacked Pages");
        tabPages->addTab(new QWidget(tabPages), "Document Tabs");
        QObject::connect(tabPages, &QTabWidget::currentChanged, tabPages, [tabPages](int childIndex) {
            loadTabsPage(tabPages, childIndex);
        });

        layout->addWidget(tabPages);
        loadTabsPage(tabPages, 0);
        layout->addStretch();
        break;
    }
    case 8: {
        QWidget *floatingArea = new QWidget(page);
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

        layout->addWidget(floatingArea);
        layout->addStretch();
        break;
    }
    default:
        layout->addWidget(createDescription("Unknown container page.", page));
        layout->addStretch();
        break;
    }
}

ContainerTab::ContainerTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->setDocumentMode(true);
    tabs->addTab(new QWidget(tabs), "Overview");
    tabs->addTab(new QWidget(tabs), "Frames");
    tabs->addTab(new QWidget(tabs), "Groups");
    tabs->addTab(new QWidget(tabs), "Typography");
    tabs->addTab(new QWidget(tabs), "Buttons");
    tabs->addTab(new QWidget(tabs), "Messages");
    tabs->addTab(new QWidget(tabs), "Blur");
    tabs->addTab(new QWidget(tabs), "Tabs");
    tabs->addTab(new QWidget(tabs), "Floating");

    connect(tabs, &QTabWidget::currentChanged, tabs, [tabs](int index) {
        loadContainerPage(tabs, index);
    });

    mainLayout->addWidget(tabs);
    loadContainerPage(tabs, 0);
}
