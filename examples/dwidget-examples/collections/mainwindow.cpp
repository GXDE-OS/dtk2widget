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
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QStackedWidget>
#include <QFontDatabase>
#include <QTextCodec>
#include <QDebug>
#include <QDir>
#include <QTemporaryFile>

#include <functional>

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

static QWidget *sideTabs(const QStringList &titles, QWidget *parent, const std::function<void(QStackedWidget *, int)> &loader)
{
    QWidget *container = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    QListWidget *nav = new QListWidget(container);
    nav->setObjectName("ExampleSideNav");
    nav->setFixedWidth(150);
    nav->setFrameShape(QFrame::NoFrame);
    nav->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QStackedWidget *stack = new QStackedWidget(container);
    for (const QString &title : titles) {
        nav->addItem(title);
        stack->addWidget(new QWidget(stack));
    }

    QObject::connect(nav, &QListWidget::currentRowChanged, stack, [stack, loader](int row) {
        if (row < 0)
            return;
        stack->setCurrentIndex(row);
        loader(stack, row);
    });

    layout->addWidget(nav);
    layout->addWidget(stack, 1);
    nav->setCurrentRow(0);
    return container;
}

static void loadGxdeWidgetPage(QStackedWidget *tabs, int index)
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

    QLabel *hint = descriptionLabel("DTK2 示例按应用场景分类，右上角菜单可切换主题和窗口背景模式，用于验证背景、透明窗口与 Blur。", this);
    hint->setObjectName("OverviewHint");
    mainLayout->addWidget(hint);

    initTabWidget();
    mainLayout->addWidget(m_mainStack->parentWidget());

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("CollectionsCentralWidget");
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    initTitlebarMenu();
    applyWindowPreset(DMainWindow::DefaultWindow);
    updateCentralBackground();
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

    QMenu *themeMenu = m_titleMenu->addMenu("主题");
    m_lightAction = themeMenu->addAction("浅色主题");
    m_darkAction = themeMenu->addAction("深色主题");
    m_systemThemeAction = themeMenu->addAction("跟随系统主题");
    for (QAction *action : {m_lightAction, m_darkAction, m_systemThemeAction}) {
        action->setCheckable(true);
        m_themeGroup->addAction(action);
    }

    connect(m_lightAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->setTheme("light");
        updateCentralBackground();
        updateThemeActions();
    });
    connect(m_darkAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->setTheme("dark");
        updateCentralBackground();
        updateThemeActions();
    });
    connect(m_systemThemeAction, &QAction::triggered, this, [themeManager, this] {
        themeManager->FollowSystemDefaultTheme();
        updateCentralBackground();
        updateThemeActions();
    });

    QMenu *backgroundMenu = m_titleMenu->addMenu("窗口背景");
    m_backgroundGroup = new QActionGroup(this);
    m_backgroundGroup->setExclusive(true);

    m_setWindowBackgroundAction = backgroundMenu->addAction("设置窗口背景");
    m_removeWindowBackgroundAction = backgroundMenu->addAction("移除窗口背景");
    m_transparentAction = backgroundMenu->addAction("透明背景");
    m_blurWindowAction = backgroundMenu->addAction("窗口 Blur");
    for (QAction *action : {m_setWindowBackgroundAction, m_removeWindowBackgroundAction, m_transparentAction, m_blurWindowAction}) {
        action->setCheckable(true);
        m_backgroundGroup->addAction(action);
    }
    m_setWindowBackgroundAction->setChecked(true);
    connect(m_backgroundGroup, &QActionGroup::triggered, this, &MainWindow::setWindowBackgroundMode);

    m_titleMenu->addSeparator();
    QAction *fullscreenAction = m_titleMenu->addAction("切换全屏");
    connect(fullscreenAction, &QAction::triggered, this, [this] {
        isFullScreen() ? showNormal() : showFullScreen();
    });

    titlebar()->setMenu(m_titleMenu);
    updateThemeActions();
}

void MainWindow::setWindowBackgroundMode(QAction *action)
{
    if (action == m_setWindowBackgroundAction) {
        const QString fileName = QFileDialog::getOpenFileName(this,
                                                              "选择窗口背景图片",
                                                              m_windowBackgroundPath.isEmpty() ? QDir::homePath() : m_windowBackgroundPath,
                                                              "图片文件 (*.jpg *.jpeg *.png *.bmp *.gif *.svg);;所有文件 (*.*)");
        if (!fileName.isEmpty()) {
            m_windowBackgroundPath = fileName;
        } else if (m_windowBackgroundPath.isEmpty()) {
            m_removeWindowBackgroundAction->setChecked(true);
        }
    }

    updateCentralBackground();
}

void MainWindow::updateCentralBackground()
{
    const bool imageBackground = !m_setWindowBackgroundAction || m_setWindowBackgroundAction->isChecked();
    const bool transparentBackground = m_transparentAction && m_transparentAction->isChecked();
    const bool blurBackground = m_blurWindowAction && m_blurWindowAction->isChecked();

    // Keep DMainWindow's built-in background actions hidden; this example owns
    // background switching through the titlebar submenu above.
    setEnableWindowBackground(false);
    setEnableBlurWindow(blurBackground);
    setTranslucentBackground(transparentBackground || blurBackground);
    titlebar()->setBackgroundTransparent(transparentBackground || blurBackground);
    titlebar()->setBlurBackground(blurBackground);

    if (centralWidget()) {
        centralWidget()->setAttribute(Qt::WA_TranslucentBackground, transparentBackground || blurBackground);
        centralWidget()->setAutoFillBackground(!(transparentBackground || blurBackground));
        if (transparentBackground || blurBackground) {
            centralWidget()->setStyleSheet(QString());
        } else if (imageBackground) {
            const QString backgroundPath = m_windowBackgroundPath.isEmpty() ? ":/images/default_background.jpg" : m_windowBackgroundPath;
            centralWidget()->setStyleSheet(QString("#CollectionsCentralWidget { background-image: url(\"%1\"); background-position: center; }"
                                           "#CollectionsCentralWidget QLabel { background: transparent; }").arg(backgroundPath));
        } else {
            centralWidget()->setStyleSheet(QString());
        }
        centralWidget()->setPalette(qApp->palette());
    }

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
    QWidget *mainTabs = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(mainTabs);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    m_mainNav = new QListWidget(mainTabs);
    m_mainNav->setObjectName("MainExampleNav");
    m_mainNav->setFixedWidth(156);
    m_mainNav->setFrameShape(QFrame::NoFrame);
    m_mainNav->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_mainStack = new QStackedWidget(mainTabs);
    const QStringList sections = QStringList() << "总览" << "新增组件" << "基础控件" << "输入与编辑" << "列表与反馈" << "多媒体";
    for (const QString &section : sections) {
        m_mainNav->addItem(section);
        m_mainStack->addWidget(new QWidget(m_mainStack));
    }

    layout->addWidget(m_mainNav);
    layout->addWidget(m_mainStack, 1);

    connect(m_mainNav, &QListWidget::currentRowChanged, this, [this](int index) {
        if (index < 0)
            return;
        m_mainStack->setCurrentIndex(index);
        loadSection(index);
    });
    m_mainNav->setCurrentRow(0);
}

void MainWindow::loadSection(int index)
{
    QWidget *page = m_mainStack->widget(index);
    if (!page || page->property("loaded").toBool())
        return;

    page->setProperty("loaded", true);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);

    switch (index) {
    case 0:
        layout->addWidget(descriptionLabel("DTK2 示例按实际使用场景重新整理。优先展示最小可运行组合：一个标题、一段说明、一个控件或一组常见交互。", page));
        layout->addWidget(useCaseCard("窗口与背景", "当前示例使用 DMainWindow。右上角菜单可切换浅色/深色主题，并在设置窗口背景、移除窗口背景、透明背景和窗口 Blur 之间切换。", page));
        layout->addWidget(useCaseCard("业务页面骨架", "新增组件页演示卡片、分组、按钮、消息、标签页和浮层，适合设置中心、文件管理器、安装器等应用复用。", page));
        layout->addWidget(useCaseCard("基础控件", "按钮、输入、滑块、进度、列表等旧示例保留为独立分类，避免一次加载过多组件。", page));
        layout->addStretch();
        break;
    case 1: {
        layout->addWidget(sideTabs(QStringList() << "使用说明" << "容器与视觉" << "调色板", page, loadGxdeWidgetPage));
        break;
    }
    case 2: {
        layout->addWidget(sideTabs(QStringList() << "分割线" << "进度条" << "按钮" << "按钮列表" << "分段控制", page,
            [](QStackedWidget *stack, int childIndex) {
                QWidget *child = stack->widget(childIndex);
                if (!child || child->property("loaded").toBool())
                    return;
                child->setProperty("loaded", true);
                QVBoxLayout *childLayout = new QVBoxLayout(child);
                childLayout->setContentsMargins(0, 0, 0, 0);
                switch (childIndex) {
                case 0: childLayout->addWidget(new LineTab(child)); break;
                case 1: childLayout->addWidget(new BarTab(child)); break;
                case 2: childLayout->addWidget(new ButtonTab(child)); break;
                case 3: childLayout->addWidget(new ButtonListTab(child)); break;
                case 4: childLayout->addWidget(new Segmentedcontrol(child)); break;
                default: break;
                }
            }));
        break;
    }
    case 3: {
        layout->addWidget(sideTabs(QStringList() << "输入框" << "滑块", page,
            [](QStackedWidget *stack, int childIndex) {
                QWidget *child = stack->widget(childIndex);
                if (!child || child->property("loaded").toBool())
                    return;
                child->setProperty("loaded", true);
                QVBoxLayout *childLayout = new QVBoxLayout(child);
                childLayout->setContentsMargins(0, 0, 0, 0);
                childLayout->addWidget(childIndex == 0 ? static_cast<QWidget *>(new InputTab(child)) : static_cast<QWidget *>(new SliderTab(child)));
            }));
        break;
    }
    case 4: {
        layout->addWidget(sideTabs(QStringList() << "状态指示" << "图形效果" << "列表视图", page,
            [](QStackedWidget *stack, int childIndex) {
                QWidget *child = stack->widget(childIndex);
                if (!child || child->property("loaded").toBool())
                    return;
                child->setProperty("loaded", true);
                QVBoxLayout *childLayout = new QVBoxLayout(child);
                childLayout->setContentsMargins(0, 0, 0, 0);
                switch (childIndex) {
                case 0: childLayout->addWidget(new IndicatorTab(child)); break;
                case 1: childLayout->addWidget(new GraphicsEffectTab(child)); break;
                case 2: childLayout->addWidget(new SimpleListViewTab(child)); break;
                default: break;
                }
            }));
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
