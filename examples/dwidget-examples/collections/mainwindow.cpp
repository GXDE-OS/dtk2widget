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
        layout->addWidget(descriptionLabel("Select Containers or Palettes below. Legacy WidgetsTab is intentionally not loaded here because it contains global monitor and MPRIS demos that can block diagnosis.", page));
        layout->addStretch();
        break;
    case 1:
        layout->addWidget(new ContainerTab(page));
        break;
    case 2:
        layout->addWidget(new PaletteTab(page));
        break;
    default:
        layout->addWidget(descriptionLabel("Unknown GXDE widget page.", page));
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
    m_mainTab->addTab(new QWidget(m_mainTab), "Overview");
    m_mainTab->addTab(new QWidget(m_mainTab), "GXDE Widgets");
    m_mainTab->addTab(new QWidget(m_mainTab), "Basic Controls");
    m_mainTab->addTab(new QWidget(m_mainTab), "Inputs");
    m_mainTab->addTab(new QWidget(m_mainTab), "Effects & Lists");
    m_mainTab->addTab(new QWidget(m_mainTab), "Multimedia");

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
        layout->addWidget(descriptionLabel("Open each section from the tabs above. Sections are loaded lazily so a broken demo area can be isolated without hiding the whole window.", page));
        layout->addStretch();
        break;
    case 1: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new QWidget(tabs), "Overview");
        tabs->addTab(new QWidget(tabs), "Containers");
        tabs->addTab(new QWidget(tabs), "Palettes");
        connect(tabs, &QTabWidget::currentChanged, tabs, [tabs](int childIndex) {
            loadGxdeWidgetPage(tabs, childIndex);
        });
        loadGxdeWidgetPage(tabs, 0);
        layout->addWidget(tabs);
        break;
    }
    case 2: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new LineTab(tabs), "Line");
        tabs->addTab(new BarTab(tabs), "Bar");
        tabs->addTab(new ButtonTab(tabs), "Button");
        tabs->addTab(new ButtonListTab(tabs), "ButtonList");
        tabs->addTab(new Segmentedcontrol(tabs), "Segmented Control");
        layout->addWidget(tabs);
        break;
    }
    case 3: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new InputTab(tabs), "Input");
        tabs->addTab(new SliderTab(tabs), "Slider");
        layout->addWidget(tabs);
        break;
    }
    case 4: {
        QTabWidget *tabs = childTabs(page);
        tabs->addTab(new IndicatorTab(tabs), "Indicator");
        tabs->addTab(new GraphicsEffectTab(tabs), "GraphicsEffect");
        tabs->addTab(new SimpleListViewTab(tabs), "SimpleListView");
        layout->addWidget(tabs);
        break;
    }
    case 5:
#ifndef DTK_NO_MULTIMEDIA
        layout->addWidget(new CameraForm(page));
#else
        layout->addWidget(descriptionLabel("Multimedia examples are disabled in this build.", page));
#endif
        break;
    default:
        layout->addWidget(descriptionLabel("Unknown section.", page));
        break;
    }
}

MainWindow::~MainWindow()
{

}
