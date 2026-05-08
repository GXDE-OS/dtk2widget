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

#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFontDatabase>
#include <QTextCodec>
#include <QDebug>
#include <QTemporaryFile>

#include "qsettingbackend.h"
#include "dsettingsdialog.h"
#include "dsettingsoption.h"
#include "dsettings.h"

#include "dslider.h"
#include "dthememanager.h"
#include "dtkwidget_global.h"
#include "dswitchbutton.h"
#include "dtitlebar.h"
#include "segmentedcontrol.h"

#include <DApplication>

#include "mainwindow.h"
#include "buttonlisttab.h"
#include "graphicseffecttab.h"
#include "simplelistviewtab.h"

#ifndef DTK_NO_MULTIMEDIA
#include "cameraform.h"
#endif

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static QLabel *descriptionLabel(const QString &text, QWidget *parent)
{
    QLabel *label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setMargin(12);
    return label;
}

static QFrame *useCaseCard(const QString &title, const QString &description, QWidget *parent)
{
    QFrame *card = new QFrame(parent);
    card->setFrameShape(QFrame::StyledPanel);
    card->setObjectName("UseCaseCard");

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(6);

    QLabel *titleLabel = new QLabel(title, card);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel(description, card));

    return card;
}

static QTabWidget *childTabs(QWidget *parent)
{
    QTabWidget *tabs = new QTabWidget(parent);
    tabs->setDocumentMode(true);
    return tabs;
}

static void loadGxdeWidgetPage(QTabWidget *tabs, int index)
{
    QWidget *page = tabs->widget(index);
    if (!page || page->property("loaded").toBool())
        return;

    page->setProperty("loaded", true);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);

    switch (index) {
    case 0:
        layout->addWidget(descriptionLabel("这里集中展示近期从 GXDE 应用中抽取的可复用组件。每个页面只保留最小实用用例，便于直接复制到应用中验证。", page));
        layout->addWidget(useCaseCard("容器与视觉", "用于设置页、详情页、列表分组和浮层：DCardWidget、DBackgroundGroup、DBlurSurface、DTabbedStackWidget。", page));
        layout->addWidget(useCaseCard("调色板", "展示内置浅色/深色调色板，以及应用如何通过 DThemeManager 切换主题。", page));
        layout->addStretch();
        break;
    case 1:
        layout->addWidget(new ContainerTab(page));
        break;
    case 2:
        layout->addWidget(new PaletteTab(page));
        break;
    default:
        layout->addWidget(descriptionLabel("未知新增组件页面。", page));
        break;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QLabel *hint = descriptionLabel("DTK2 示例按应用场景分类，右上角菜单可切换主题、背景和透明窗口，用于验证 Blur 与窗口背景。", this);
    hint->setObjectName("OverviewHint");
    mainLayout->addWidget(hint);

    initTabWidget();
    mainLayout->addWidget(m_mainTab);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("CollectionsCentralWidget");
    centralWidget->setLayout(mainLayout);

    initTitlebarMenu();
    applyWindowPreset(DMainWindow::DefaultWindow);
    applyDemoBackground(true);
    setCentralWidget(centralWidget);
    setWindowTitle("DTK2 组件示例");
    titlebar()->setTitle(windowTitle());
    setMinimumSize(960, 640);
    resize(1180, 760);
}

void MainWindow::initTitlebarMenu()
{
    DThemeManager *themeManager = DThemeManager::instance();

    m_titleMenu = new QMenu(this);
    m_themeGroup = new QActionGroup(this);
    m_themeGroup->setExclusive(true);

    m_lightAction = m_titleMenu->addAction("浅色主题");
    m_darkAction = m_titleMenu->addAction("深色主题");
    m_systemThemeAction = m_titleMenu->addAction("跟随系统主题");
    for (QAction *action : {m_lightAction, m_darkAction, m_systemThemeAction}) {
        action->setCheckable(true);
        m_themeGroup->addAction(action);
    }

    connect(m_lightAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->setTheme("light");
        updateThemeActions();
    });
    connect(m_darkAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->setTheme("dark");
        updateThemeActions();
    });
    connect(m_systemThemeAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->FollowSystemDefaultTheme();
        updateThemeActions();
    });

    m_titleMenu->addSeparator();
    m_backgroundAction = m_titleMenu->addAction("使用示例背景");
    m_backgroundAction->setCheckable(true);
    m_backgroundAction->setChecked(true);
    connect(m_backgroundAction, &QAction::toggled, this, &MainWindow::applyDemoBackground);

    m_transparentAction = m_titleMenu->addAction("透明内容背景");
    m_transparentAction->setCheckable(true);
    connect(m_transparentAction, &QAction::toggled, this, &MainWindow::setDemoBackgroundTransparent);

    m_blurWindowAction = m_titleMenu->addAction("启用窗口 Blur");
    m_blurWindowAction->setCheckable(true);
    connect(m_blurWindowAction, &QAction::toggled, this, [this](bool enabled) {
        setEnableBlurWindow(enabled);
        setTranslucentBackground(enabled || (m_transparentAction && m_transparentAction->isChecked()));
        titlebar()->setBackgroundTransparent(enabled);
        titlebar()->setBlurBackground(enabled);
    });

    m_titleMenu->addSeparator();
    QAction *fullscreenAction = m_titleMenu->addAction("切换全屏");
    connect(fullscreenAction, &QAction::triggered, this, [this] {
        isFullScreen() ? showNormal() : showFullScreen();
    });

    titlebar()->setMenu(m_titleMenu);
    updateThemeActions();
}

void MainWindow::applyDemoBackground(bool enabled)
{
    setEnableWindowBackground(enabled);
    if (centralWidget() && !(m_transparentAction && m_transparentAction->isChecked())) {
        centralWidget()->setStyleSheet(enabled
            ? "#CollectionsCentralWidget { background-image: url(:/images/default_background.jpg); background-position: center; }"
            : QString());
    }
    background()->refresh();
    refreshBackground();
}

void MainWindow::setDemoBackgroundTransparent(bool transparent)
{
    if (centralWidget()) {
        centralWidget()->setAttribute(Qt::WA_TranslucentBackground, transparent);
        centralWidget()->setAutoFillBackground(!transparent);
        centralWidget()->setStyleSheet(transparent
            ? "#CollectionsCentralWidget { background: transparent; }"
            : (m_backgroundAction && m_backgroundAction->isChecked()
                ? "#CollectionsCentralWidget { background-image: url(:/images/default_background.jpg); background-position: center; }"
                : QString()));
    }

    setTranslucentBackground(transparent || (m_blurWindowAction && m_blurWindowAction->isChecked()));
    titlebar()->setBackgroundTransparent(transparent);
    refreshBackground();
}

void MainWindow::updateThemeActions()
{
    const QString theme = DThemeManager::instance()->theme(this);
    if (m_lightAction)
        m_lightAction->setChecked(theme != "dark");
    if (m_darkAction)
        m_darkAction->setChecked(theme == "dark");
}

void MainWindow::menuItemInvoked(QAction *action)
{
    if (action->text() == "dfm-settings") {
        QTemporaryFile tmpFile;
        tmpFile.open();
        auto backend = new Dtk::Core::QSettingBackend(tmpFile.fileName());

        auto settings = Dtk::Core::DSettings::fromJsonFile(":/resources/data/dfm-settings.json");
        settings->setBackend(backend);

        DSettingsDialog dsd(this);
        dsd.updateSettings(settings);
        dsd.exec();
        return;
    }

    if (action->text() == "dt-settings") {
        QTemporaryFile tmpFile;
        tmpFile.open();
        auto backend = new Dtk::Core::QSettingBackend(tmpFile.fileName());

        auto settings = Dtk::Core::DSettings::fromJsonFile(":/resources/data/dt-settings.json");
        settings->setBackend(backend);

        QFontDatabase fontDatabase;
        auto fontFamliy = settings->option("base.font.family");
        QMap<QString, QVariant> fontDatas;

        QStringList values = fontDatabase.families();
        QStringList keys;
        for (auto &v : values) {
            keys << v.toLower().trimmed();
        }
        fontDatas.insert("keys", keys);
        fontDatas.insert("values", values);
        fontFamliy->setData("items", fontDatas);

        // or you can set default value by json
        if (fontFamliy->value().toString().isEmpty()) {
            fontFamliy->setValue("droid serif");
        }

        connect(fontFamliy, &DSettingsOption::valueChanged,
        this, [](QVariant value) {
            qDebug() << "fontFamliy change" << value;
        });

        QStringList codings;
        for (auto coding : QTextCodec::availableCodecs()) {
            codings << coding;
        }

        auto encoding = settings->option("advance.encoding.encoding");
        encoding->setData("items", codings);
        encoding->setValue(0);

        DSettingsDialog dsd(this);
        dsd.updateSettings(settings);
        dsd.exec();
        return;
    }

    QMessageBox::warning(this, "menu clieck",  action->text() + ", was cliecked");
    qDebug() << "click" << action << action->isChecked();
}

void MainWindow::initTabWidget()
{
    m_mainTab = new QTabWidget(this);
    m_mainTab->setDocumentMode(true);
    m_mainTab->addTab(new QWidget(m_mainTab), "总览");
    m_mainTab->addTab(new QWidget(m_mainTab), "新增组件");
    m_mainTab->addTab(new QWidget(m_mainTab), "基础控件");
    m_mainTab->addTab(new QWidget(m_mainTab), "输入与编辑");
    m_mainTab->addTab(new QWidget(m_mainTab), "列表与反馈");
    m_mainTab->addTab(new QWidget(m_mainTab), "多媒体");

    connect(m_mainTab, &QTabWidget::currentChanged, this, &MainWindow::loadSection);
    loadSection(0);
}

void MainWindow::loadSection(int index)
{
    QWidget *page = m_mainTab->widget(index);
    if (!page || page->property("loaded").toBool())
        return;

    page->setProperty("loaded", true);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);

    switch (index) {
    case 0:
        layout->addWidget(descriptionLabel("DTK2 示例按实际使用场景重新整理。优先展示最小可运行组合：一个标题、一段说明、一个控件或一组常见交互。", page));
        layout->addWidget(useCaseCard("窗口与背景", "当前示例使用 DMainWindow。右上角菜单可切换浅色/深色主题、示例背景、透明内容背景和窗口 Blur。", page));
        layout->addWidget(useCaseCard("业务页面骨架", "新增组件页演示卡片、分组、按钮、消息、标签页和浮层，适合设置中心、文件管理器、安装器等应用复用。", page));
        layout->addWidget(useCaseCard("基础控件", "按钮、输入、滑块、进度、列表等旧示例保留为独立分类，避免一次加载过多组件。", page));
        layout->addStretch();
        break;
    case 1: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new QWidget(tabs), "使用说明");
        tabs->addTab(new QWidget(tabs), "容器与视觉");
        tabs->addTab(new QWidget(tabs), "调色板");
        connect(tabs, &QTabWidget::currentChanged, tabs, [tabs](int childIndex) {
            loadGxdeWidgetPage(tabs, childIndex);
        });
        loadGxdeWidgetPage(tabs, 0);
        layout->addWidget(tabs);
        break;
    }
    case 2: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new LineTab(tabs), "分割线");
        tabs->addTab(new BarTab(tabs), "进度条");
        tabs->addTab(new ButtonTab(tabs), "按钮");
        tabs->addTab(new ButtonListTab(tabs), "按钮列表");
        tabs->addTab(new Segmentedcontrol(tabs), "分段控制");
        layout->addWidget(tabs);
        break;
    }
    case 3: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new InputTab(tabs), "输入框");
        tabs->addTab(new SliderTab(tabs), "滑块");
        layout->addWidget(tabs);
        break;
    }
    case 4: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new IndicatorTab(tabs), "状态指示");
        tabs->addTab(new GraphicsEffectTab(tabs), "图形效果");
        tabs->addTab(new SimpleListViewTab(tabs), "列表视图");
        layout->addWidget(tabs);
        break;
    }
    case 5:
#ifndef DTK_NO_MULTIMEDIA
        layout->addWidget(new CameraForm(page));
#else
        layout->addWidget(descriptionLabel("当前构建禁用了多媒体示例。", page));
#endif
        break;
    default:
        layout->addWidget(descriptionLabel("未知分类。", page));
        break;
    }
}

MainWindow::~MainWindow()
{

}
