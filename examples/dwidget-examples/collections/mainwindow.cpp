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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    DThemeManager *themeManager = DThemeManager::instance();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    QHBoxLayout *styleLayout = new QHBoxLayout();
    QPushButton *darkButton = new QPushButton("Dark", this);
    QPushButton *lightBUtton = new QPushButton("Light", this);
    QPushButton *systemThemeButton = new QPushButton("Follow System", this);
    QPushButton *fullscreenButtons = new QPushButton("Fullscreen", this);
    QLabel *hint = new QLabel("DTK2 Collections is split into sections and loads each section on demand.", this);

    themeManager->setTheme(lightBUtton, "light");

    connect(darkButton, &QPushButton::clicked, [ = ] {
        themeManager->setTheme("dark");
    });
    connect(lightBUtton, &QPushButton::clicked, [ = ] {
        themeManager->setTheme("light");
    });
    connect(systemThemeButton, &QPushButton::clicked, [ = ] {
        themeManager->FollowSystemDefaultTheme();
    });
    connect(fullscreenButtons, &QPushButton::clicked, [ = ] {
        if (!isFullScreen())
        {
            showFullScreen();
        } else
        {
            showNormal();
        }
    });

    styleLayout->addWidget(darkButton);
    styleLayout->addWidget(lightBUtton);
    styleLayout->addWidget(systemThemeButton);
    styleLayout->addWidget(fullscreenButtons);
    styleLayout->addSpacing(12);
    styleLayout->addWidget(hint);
    styleLayout->addStretch();

    mainLayout->addLayout(styleLayout);

    initTabWidget();
    mainLayout->addWidget(m_mainTab);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setAutoFillBackground(true);

    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    setWindowTitle("DTK2 Widget Collections");
    setMinimumSize(960, 640);
    resize(1180, 760);
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
        tabs->addTab(new WidgetsTab(tabs), "Widgets");
        tabs->addTab(new ContainerTab(tabs), "Containers");
        tabs->addTab(new PaletteTab(tabs), "Palettes");
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
