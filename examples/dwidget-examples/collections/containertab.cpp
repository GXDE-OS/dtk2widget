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
#include "dtabbar.h"
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
        layout->addWidget(createDescription("按需打开一个标签页组件。这里把页面堆叠和文档标签分开，便于验证 DTabBar 的实际使用路径。", page));
        layout->addStretch();
        break;
    case 1: {
        DCardWidget *tabBarCard = new DCardWidget(page);
        QVBoxLayout *tabBarLayout = new QVBoxLayout(tabBarCard);
        tabBarLayout->setContentsMargins(18, 14, 18, 14);
        tabBarLayout->setSpacing(12);
        tabBarLayout->addWidget(new DSectionTitle("DTabBar：基础标签栏", tabBarCard));

        DTabBar *normalTabBar = new DTabBar(tabBarCard);
        normalTabBar->setDrawBase(false);
        normalTabBar->setVisibleAddButton(true);
        normalTabBar->addTab(QIcon::fromTheme("go-home"), "首页");
        normalTabBar->addTab(QIcon::fromTheme("folder"), "文件");
        normalTabBar->addTab(QIcon::fromTheme("applications-system"), "工具");
        normalTabBar->setCurrentIndex(0);
        normalTabBar->setFixedHeight(40);
        tabBarLayout->addWidget(normalTabBar);

        DTabBar *closableTabBar = new DTabBar(tabBarCard);
        closableTabBar->setDrawBase(false);
        closableTabBar->setTabsClosable(true);
        closableTabBar->setVisibleAddButton(false);
        closableTabBar->addTab("未保存文档");
        closableTabBar->addTab("下载任务");
        closableTabBar->addTab("同步日志");
        closableTabBar->setCurrentIndex(1);
        closableTabBar->setFixedHeight(40);
        tabBarLayout->addWidget(closableTabBar);

        DTabBar *scrollTabBar = new DTabBar(tabBarCard);
        scrollTabBar->setDrawBase(false);
        scrollTabBar->setUsesScrollButtons(true);
        scrollTabBar->setVisibleAddButton(false);
        scrollTabBar->setFixedWidth(360);
        scrollTabBar->setFixedHeight(40);
        for (int i = 1; i <= 10; ++i)
            scrollTabBar->addTab(QString("标签 %1").arg(i));
        tabBarLayout->addWidget(scrollTabBar, 0, Qt::AlignLeft);

        DTabBar *verticalTabBar = new DTabBar(tabBarCard);
        verticalTabBar->setDrawBase(false);
        verticalTabBar->setShape(QTabBar::RoundedWest);
        verticalTabBar->setVisibleAddButton(false);
        verticalTabBar->addTab("通用");
        verticalTabBar->addTab("网络");
        verticalTabBar->addTab("高级");
        verticalTabBar->setFixedSize(120, 140);
        tabBarLayout->addWidget(verticalTabBar, 0, Qt::AlignLeft);

        tabBarLayout->addWidget(createDescription("展示 DTabBar 的新增按钮、关闭按钮、滚动按钮和纵向标签形态。", tabBarCard));
        layout->addWidget(tabBarCard);
        layout->addStretch();
        break;
    }
    case 2: {
        DCardWidget *tabbedCard = new DCardWidget(page);
        QVBoxLayout *tabbedLayout = new QVBoxLayout(tabbedCard);
        tabbedLayout->setContentsMargins(18, 14, 18, 14);
        tabbedLayout->setSpacing(10);
        tabbedLayout->addWidget(new DSectionTitle("DTabbedStackWidget：标签页 + 页面堆叠", tabbedCard));

        DTabbedStackWidget *tabbedStack = new DTabbedStackWidget(tabbedCard);
        tabbedStack->addPage(createRow("首页", "适合首页、状态页等常驻内容。业务只需要提供页面 widget。", new QPushButton("刷新", tabbedStack), tabbedStack), QIcon::fromTheme("go-home"), "首页");
        tabbedStack->addPage(createRow("清理", "标签与 QStackedWidget 索引自动同步，减少应用重复连线。", new QPushButton("扫描", tabbedStack), tabbedStack), QIcon::fromTheme("edit-clear"), "清理");
        tabbedStack->addPage(createRow("工具", "覆盖系统助手、控制中心等常见的顶部标签页页面结构。", new QPushButton("打开", tabbedStack), tabbedStack), QIcon::fromTheme("applications-system"), "工具");
        tabbedLayout->addWidget(tabbedStack);

        layout->addWidget(tabbedCard);
        layout->addStretch();
        break;
    }
    case 3: {
        DCardWidget *documentTabCard = new DCardWidget(page);
        QVBoxLayout *documentTabLayout = new QVBoxLayout(documentTabCard);
        documentTabLayout->setContentsMargins(18, 14, 18, 14);
        documentTabLayout->setSpacing(10);
        documentTabLayout->addWidget(new DSectionTitle("DDocumentTabBar：文档标签", documentTabCard));

        DDocumentTabBar *documentTabBar = new DDocumentTabBar(documentTabCard);
        documentTabBar->setDragable(false);
        documentTabBar->setVisibleAddButton(false);
        documentTabBar->addDocument("/tmp/notes.txt", QIcon::fromTheme("text-x-generic"), "notes.txt");
        documentTabBar->addDocument("/tmp/report.md", QIcon::fromTheme("text-markdown"), "report.md");
        documentTabBar->addDocument("/tmp/readme.txt", QIcon::fromTheme("text-x-generic"), "readme.txt");
        documentTabBar->setDocumentModified(1, true);
        documentTabBar->setFixedHeight(40);
        documentTabLayout->addWidget(documentTabBar);
        documentTabLayout->addWidget(createDescription("最小用例：打开多个文档、记录 documentId、显示修改标记。示例中关闭拖拽和新增按钮，便于聚焦基础行为。", documentTabCard));

        layout->addWidget(documentTabCard);
        layout->addStretch();
        break;
    }
    default:
        layout->addWidget(createDescription("未知标签页示例。", page));
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
        layout->addWidget(createTitle("容器与视觉组件", page));
        layout->addWidget(createDescription("每个页面只加载一个组件组。用最小实用场景展示如何在应用里复用，而不是堆叠所有演示控件。", page));
        layout->addStretch();
        break;
    case 1: {
        DFrame *plainFrame = new DFrame(page);
        plainFrame->setMinimumHeight(86);
        QVBoxLayout *plainLayout = new QVBoxLayout(plainFrame);
        plainLayout->setContentsMargins(18, 14, 18, 14);
        plainLayout->addWidget(createTitle("DFrame", plainFrame));
        plainLayout->addWidget(createDescription("用于简单分区的圆角基础框架，颜色跟随当前调色板。", plainFrame));

        DCardWidget *card = new DCardWidget(page);
        card->setMinimumHeight(116);
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 14, 18, 14);
        cardLayout->setSpacing(8);
        cardLayout->addWidget(createTitle("DCardWidget", card));
        cardLayout->addWidget(createDescription("用于设置项、详情块、操作入口等需要比页面背景更突出的位置。", card));
        QPushButton *cardButton = new QPushButton("主要操作", card);
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
        group->layout()->addWidget(createRow("DBackgroundGroup", "为多行设置项绘制统一圆角背景。", new DSwitchButton(group), group));
        group->layout()->addWidget(createRow("扁平分隔", "行之间保持层级清晰，不需要应用重复写 frame 样式。", new QPushButton("编辑", group), group));
        group->layout()->addWidget(createRow("紧凑圆角", "适配 DTK2 当前更克制的视觉风格。", new QPushButton("打开", group), group));
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
        typographyLayout->addWidget(new DTypographyLabel("LargeTitle：页面或对话框主标题", DTypographyLabel::LargeTitle, typographyCard));
        typographyLayout->addWidget(new DTypographyLabel("Title：分组标题", DTypographyLabel::Title, typographyCard));
        typographyLayout->addWidget(new DTypographyLabel("Body：正文说明", DTypographyLabel::Body, typographyCard));
        DTypographyLabel *caption = new DTypographyLabel("Caption：补充或次要信息", DTypographyLabel::Caption, typographyCard);
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
        buttonLayout->addWidget(new DSectionTitle("悬停按钮与图文按钮", buttonCard));

        QHBoxLayout *buttonRow = new QHBoxLayout;
        DHoverButton *hoverButton = new DHoverButton(buttonCard);
        hoverButton->setNormalIcon(QIcon::fromTheme("go-previous"));
        hoverButton->setHoverIcon(QIcon::fromTheme("go-next"));
        hoverButton->setPressedIcon(QIcon::fromTheme("go-down"));
        hoverButton->setFixedSize(42, 32);

        DIconTextButton *iconTextButton = new DIconTextButton(QIcon::fromTheme("document-open"), "打开文件", buttonCard);
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
        buttonLayout->addWidget(createDescription("覆盖文件管理器、图片查看器等常见图标按钮场景，避免应用重复实现 hover/pressed 图标逻辑。", buttonCard));
        layout->addWidget(buttonCard);
        layout->addStretch();
        break;
    }
    case 5: {
        DCardWidget *messageCard = new DCardWidget(page);
        QVBoxLayout *messageLayout = new QVBoxLayout(messageCard);
        messageLayout->setContentsMargins(18, 14, 18, 14);
        messageLayout->setSpacing(10);
        messageLayout->addWidget(new DSectionTitle("DFloatingMessage：窗口内消息", messageCard));

        DFloatingMessage *transientMessage = new DFloatingMessage(DFloatingMessage::TransientType, messageCard);
        transientMessage->setIcon(QIcon::fromTheme("dialog-information"));
        transientMessage->setMessage("临时消息：到达配置时长后自动关闭。");
        transientMessage->setDuration(5000);

        DFloatingMessage *residentMessage = new DFloatingMessage(DFloatingMessage::ResidentType, messageCard);
        residentMessage->setIcon(QIcon::fromTheme("dialog-warning"));
        residentMessage->setMessage("常驻消息：保留在界面中，等待用户处理。");
        residentMessage->setActionWidget(new QPushButton("处理", residentMessage));

        messageLayout->addWidget(transientMessage);
        messageLayout->addWidget(residentMessage);
        messageLayout->addWidget(createDescription("用于窗口内部通知，比全局 toast 更适合表单校验、任务状态和可恢复错误。", messageCard));
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
        blurSurfaceLayout->addWidget(createDescription("可复用半透明面板，适合控制中心、启动器和带背景图的设置页面。", blurSurface));
        blurAreaLayout->addWidget(blurSurface, 0, Qt::AlignCenter);
        layout->addWidget(blurArea);
        layout->addStretch();
        break;
    }
    case 7: {
        QTabWidget *tabPages = new QTabWidget(page);
        tabPages->setDocumentMode(true);
        tabPages->setTabPosition(QTabWidget::West);
        tabPages->addTab(new QWidget(tabPages), "说明");
        tabPages->addTab(new QWidget(tabPages), "基础标签栏");
        tabPages->addTab(new QWidget(tabPages), "页面堆叠");
        tabPages->addTab(new QWidget(tabPages), "文档标签");
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
        floatingLayout->addWidget(createDescription("带阴影和可选 Blur 的浮层，适合弹出面板、预览卡片和临时工具。", floatingContent));
        floating->setWidget(floatingContent);
        floating->setBlurBackgroundEnabled(true);
        floating->setFixedWidth(360);
        floatingAreaLayout->addWidget(floating, 0, Qt::AlignCenter);

        layout->addWidget(floatingArea);
        layout->addStretch();
        break;
    }
    default:
        layout->addWidget(createDescription("未知容器组件。", page));
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
    tabs->setTabPosition(QTabWidget::West);
    tabs->addTab(new QWidget(tabs), "说明");
    tabs->addTab(new QWidget(tabs), "框架");
    tabs->addTab(new QWidget(tabs), "分组");
    tabs->addTab(new QWidget(tabs), "文字层级");
    tabs->addTab(new QWidget(tabs), "按钮");
    tabs->addTab(new QWidget(tabs), "消息");
    tabs->addTab(new QWidget(tabs), "Blur");
    tabs->addTab(new QWidget(tabs), "标签页");
    tabs->addTab(new QWidget(tabs), "浮层");

    connect(tabs, &QTabWidget::currentChanged, tabs, [tabs](int index) {
        loadContainerPage(tabs, index);
    });

    mainLayout->addWidget(tabs);
    loadContainerPage(tabs, 0);
}
